
#include <stdio.h>
#include <iostream>
#include <Windows.h>

#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;
using namespace cv::cuda;

int main()
{
	cout << "hello world" << endl;

	///*cuda::printCudaDeviceInfo(cuda::getDevice());*/
	int num_devices = cuda::getCudaEnabledDeviceCount();
	if (num_devices <= 0)
	{
		std::cout << "there is no cuda device found" << std::endl;
		return 0;
	}

	cuda::setDevice(0);

	cv::Mat src = cv::imread("E://picture.jpg", 1);
	cuda::GpuMat device_img;
	device_img.upload(src);


	cuda::resize(device_img, device_img, cv::Size(src.cols / 2, src.rows / 2));
	cuda::cvtColor(device_img, device_img, CV_BGR2GRAY);
	cuda::threshold(device_img, device_img, 128, 255, CV_THRESH_BINARY);

	cv::Mat dst;
	device_img.download(dst);
	imshow("dst", dst);
	waitKey(0);

	return 0;
