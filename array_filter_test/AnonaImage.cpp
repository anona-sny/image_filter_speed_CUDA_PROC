#include "AnonaImage.h"
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <fstream>
#include <iostream> 
#include <vector>

using namespace cv;
using namespace std;

AnonaImage::AnonaImage() {
	this->error = false;
	this->errorText = "";
	this->data.width = 0;
	this->data.height = 0;
}

inline bool AnonaImage::if_file_exist(const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}

bool AnonaImage::isError() {
	return this->error;
}

string AnonaImage::getError() {
	return this->errorText;
}

void AnonaImage::clearErrors() {
	this->error = false;
	this->errorText = "";
}

void AnonaImage::readFile(string url) {
	if (this->error) {
		cout << "Error before execution function: " << this->errorText << endl;
		return;
	}
	if (if_file_exist(url)) {
		if (!haveImageReader(url)) {
			this->error = true;
			this->errorText = "Cannot open file";
			return;
		} else {
			this->image = imread(url, -1 | -1);
			this->data.width = image.size().width;
			this->data.height = image.size().height;
			return;
		}
	}
	this->error = true;
	this->errorText = "Cannot open file";
}

void AnonaImage::saveFile(string url) {
	if (this->error) {
		cout << "Error before execution function: " << this->errorText << endl;
		return;
	}
	try {
		imwrite(url, this->image);
	} catch (Exception e) {
		this->error = true;
		this->errorText = "Problems with saving file";
	}
}

void AnonaImage::imageToArray() {
	if (this->error) {
		cout << "Error before execution function: " << this->errorText << endl;
		return;
	}
	//printMatInfo(imageOriginal, "origin");
	Mat dst;
	Mat RGBA(this->image.size(), CV_32FC4);
	cvtColor(this->image, RGBA, COLOR_RGB2RGBA, 4);
	RGBA.convertTo(dst, CV_32FC4);
	//printMatInfo(dst, "transformed");
	int size = dst.size().height*dst.size().width;	// size of float array

	AnonaImageData dataNEW;
	dataNEW.width = this->data.width;
	dataNEW.height = this->data.height;

	dataNEW.redArray = vector<float>(size, 0);
	dataNEW.greenArray = vector<float>(size, 0);
	dataNEW.blueArray = vector<float>(size, 0);
	dataNEW.alphaArray = vector<float>(size, 0);

	int width = dataNEW.width;
	int height = dataNEW.height;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			//cout << y << ", " << x << endl;
			Vec4f dataColor = dst.at<Vec4f>(y, x);
			if (y*width + x == 0) {
				cout << dataColor;
			}
			// BGRA
			dataNEW.redArray[y*width + x] = dataColor.val[2];
			dataNEW.greenArray[y*width + x] = dataColor.val[1];
			dataNEW.blueArray[y*width + x] = dataColor.val[0];
			dataNEW.alphaArray[y*width + x] = dataColor.val[3];
		}
	}

	this->setData(dataNEW);
}

void AnonaImage::arrayToImage() {
	if (this->error) {
		cout << "Error before execution function: " << this->errorText << endl;
		return;
	}

	Mat output(this->data.height, this->data.width, CV_32FC4);
	int x = 0;
	int y = 0;
	int dataSize = this->data.width*this->data.height;
	for (long i = 0; i < dataSize; i++)
	{
		x = i % this->data.width;
		y = i / this->data.width;
		Vec4f vector(this->data.blueArray[i], this->data.greenArray[i], this->data.redArray[i], this->data.alphaArray[i]);
		output.at<Vec4f>(y, x) = vector;
	}
	Mat basic(this->data.height, this->data.width, CV_8U);
	output.convertTo(basic, CV_8U);
	this->image = basic;
}

AnonaImageData * AnonaImage::getData() {
	return &this->data;
}

Mat AnonaImage::getImage() {
	return this->image;
}

void AnonaImage::setData(AnonaImageData data) {
	if (data.width == this->data.width && data.height == this->data.height) {
		this->data = data;
	}
}

Size2i AnonaImage::imageSize() {
	Size2i size(this->data.width, this->data.height);
	return size;
}