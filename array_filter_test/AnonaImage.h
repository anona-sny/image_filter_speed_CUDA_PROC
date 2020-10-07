#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <fstream>
#include <vector>

class AnonaImageData {
	public:
	int width;
	int height;
	int channels;
	int bytesForPixel;
	std::vector<float> redArray;
	std::vector<float> greenArray;
	std::vector<float> blueArray;
	std::vector<float> alphaArray;
};

#pragma once
class AnonaImage {
	private:
	// attributes
	cv::Mat image;
	AnonaImageData data;
	bool error;
	std::string errorText;
	// methods
	inline bool if_file_exist(const std::string& name);

	public:
	AnonaImage();
	void readFile(std::string url);
	void saveFile(std::string url);
	void imageToArray();
	void arrayToImage();
	void clearErrors();
	bool isError();
	std::string getError();
	AnonaImageData * getData();
	void setData(AnonaImageData data);
	cv::Mat getImage();
	cv::Size2i imageSize();
};

