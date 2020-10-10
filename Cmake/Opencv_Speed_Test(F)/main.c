#include <opencv2/opencv.hpp>
#include <iostream>  
#include <stdio.h>
#include <string>
#include <sstream>

using namespace cv;
using namespace std;

int main()
{
	Mat srcImage(600, 600, CV_8UC3, Scalar(255, 120, 4));
	Mat srcImage1 = Mat(Size(600, 600), CV_8UC3, Scalar(255, 165, 0));
	Mat tempImage1 = srcImage1.clone();
	Mat tempImage2 = srcImage1.clone();
	Mat tempImage3 = srcImage1.clone();

	//at读取
	int time1 = getTickCount() * 1000 / getTickFrequency();
	for (int i = 0; i < 10; i++)//次数循环
	{
		for (int h = 0; h < 600; h++)//高度
		{
			for (int w = 0; w < 600; w++)//宽度
			{
				
				tempImage1.at<Vec3b>(w, h)[0] = 1;
				tempImage1.at<Vec3b>(w, h)[1] = 1;
				tempImage1.at<Vec3b>(w, h)[2] = 1;
				
			}
		}

	}
	int time2 = getTickCount() * 1000 / getTickFrequency();
	cout << "at读取图像\t" <<  time2-time1 << endl;
	
	//指针读取
	for (int i = 0; i < 10; i++)//次数循环
	{
		for (int h = 0; h < 600; h++)//高度
		{
			uchar* data_in = tempImage2.ptr<uchar>(h);
			uchar* data_out = tempImage2.ptr<uchar>(h);
			for (int w = 0; w < 600*3; w++)//宽度
			{
				data_in[w] = 2;
			}
		}

	}
	int time3 = getTickCount() * 1000 / getTickFrequency();
	cout << "指针读取图像\t" << time3-time2 << endl;

	//迭代器读取
	Mat_<Vec3b>::iterator it = tempImage3.begin<Vec3b>();  //初始位置的迭代器
	Mat_<Vec3b>::iterator itend = tempImage3.end<Vec3b>();  //终止位置的迭代器
	for (; it != itend; ++it)
	{
		(*it)[0] = 2;
		(*it)[1] = 2;
		(*it)[2] = 2;
	}

	int time4 = getTickCount() * 1000 / getTickFrequency();
	cout << "迭代器读取图像\t" << time4-time3 << endl;
	cout << "总时间\t" << time1 << endl;
	return 0;
}