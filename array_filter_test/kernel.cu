#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "stdio.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream> 
#include <sys/stat.h>
#include <string>
#include <fstream>
#include "AnonaImage.h"
#include <math.h> 

using namespace cv;
using namespace std;
using namespace std::chrono;

struct Filter;

AnonaImageData testCPUMatrixFilter(AnonaImageData * data, Filter filter);

struct Filter {
	vector<float> matrix;
	float divider;
	int radius;
};


int main(int argc, char *argv[])
{
	cout << "You have entered " << argc
		<< " arguments:" << "\n";
	for (int i = 0; i < argc; ++i) {
		cout << "Argument cislo. " << i << ": " << argv[i] << "\n";
	}

	std::vector <std::string> sources;
	std::string imgSRC;
	for (int i = 0; i < argc; ++i) {
		if (std::string(argv[i]) == "--image") {
			if (i + 1 < argc) { // Make sure we aren't at the end of argv!
				i++;
				imgSRC = argv[i]; // Increment 'i' so we don't get the argument as the next argv[i].
			} else { // Uh-oh, there was no argument to the destination option.
				std::cerr << "--image option requires one argument." << std::endl;
				return 1;
			}
		} else {
			sources.push_back(argv[i]);
		}
	}
	cout << "Image: " << imgSRC << std::endl;

	AnonaImage image;
	image.readFile(imgSRC);
	if (image.isError()) {
		cerr << image.getError() << endl;
		return 1;
	}
	imshow("ImageDisplay", image.getImage());
	waitKey(0);

	image.imageToArray();

	/*
	Filter filter;
	filter.matrix.reserve(9);
	filter.matrix = {
		0.5,1,2,1,0.5,
		1,7,10,7,1,
		2,10,50,10,2,
		1,7,10,7,1,
		0.5,1,2,1,0.5
	};
	filter.divider = 136;
	filter.radius = 2;*/

	Filter filter;
	filter.matrix.reserve(9);
	filter.matrix = {
		0, 5, 0,
		5, 10, 5,
		0, 5, 0
	};
	filter.divider = 30;
	filter.radius = 1;

	image.setData(testCPUMatrixFilter(image.getData(), filter));
	image.arrayToImage();

	imshow("ImageDisplay", image.getImage());
	waitKey(0);

	image.saveFile("C:/Users/anona/Desktop/out.jpeg");

	return 0;
}

AnonaImageData testCPUMatrixFilter(AnonaImageData * data, Filter filter) {
	auto start = steady_clock::now();
	// c++ need copy for non pointer working
	AnonaImageData dataCopy;
	dataCopy.width = data->width;
	dataCopy.height = data->height;

	int x;
	int y;
	int width = data->width;
	int height = data->height;
	int size = width * height;

	vector<float> filterMatrix = filter.matrix;

	vector<float> redChannel(size, 0);
	vector<float> greenChannel(size, 0);
	vector<float> blueChannel(size, 0);
	vector<float> alphaChannel(size, 0);

	int radius = filter.radius;
	int radius2Plus = radius * 2 + 1;
	int sizeOfFilterArray = (2 * radius + 1)*(2 * radius + 1);
	float output[4] = { 0, 0, 0, 0 };

	int index = 0;
	int index1 = 0;
	int index2 = 0;

	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			index = h * width + w;
			if (w < radius || h < radius || w > width - radius - 1 || h > height - radius - 1) {
				redChannel[index] = data->redArray[index];
				greenChannel[index] = data->greenArray[index];
				blueChannel[index] = data->blueArray[index];
				alphaChannel[index] = data->alphaArray[index];
			} else {
				output[0] = 0;
				output[1] = 0;
				output[2] = 0;
				output[3] = 0;
				// image cropped by (radius) pixel around border
				for (int i = 0; i < radius2Plus; i++) {
					for (int j = 0; j < radius2Plus; j++) {
						index1 = (h + (i - radius)) * width + (w + (j - radius));
						index2 = i * radius2Plus + j;
						output[0] += data->redArray[index1] * filterMatrix[index2];
						output[1] += data->greenArray[index1] * filterMatrix[index2];
						output[2] += data->blueArray[index1] * filterMatrix[index2];
						output[3] += data->alphaArray[index1] * filterMatrix[index2];
					}
				}
				redChannel[index] = output[0] / filter.divider;
				greenChannel[index] = output[1] / filter.divider;
				blueChannel[index] = output[2] / filter.divider;
				alphaChannel[index] = output[3] / filter.divider;
			}
		}
	}

	dataCopy.redArray = redChannel;
	dataCopy.greenArray = greenChannel;
	dataCopy.blueArray = blueChannel;
	dataCopy.alphaArray = alphaChannel;

	auto end = steady_clock::now();
	nanoseconds a = duration_cast<nanoseconds>(end - start);
	std::cout << "It took me " << a.count() << " nano." << endl;

	return dataCopy;
}