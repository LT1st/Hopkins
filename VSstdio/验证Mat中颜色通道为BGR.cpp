//#include <opencv2/opencv.hpp>
//#include <iostream>  
//#include <stdio.h>
//#include <string>
//#include <sstream>
//
//using namespace cv;
//using namespace std;
//
//int main()
//{
//
//	Mat srcImage(1, 1, CV_8UC3, Scalar(255, 120, 4));
//	Mat srcImage1 = Mat(Size(1, 1), CV_8UC3, Scalar(255, 165, 0));
//
//	Mat tempImage = srcImage1.clone();
//
//	int g = tempImage.at<Vec3b>(0, 0)[0];
//	int b = tempImage.at<Vec3b>(0, 0)[1];
//	int r = tempImage.at<Vec3b>(0, 0)[2];
//
//	cout << g<<"\t" << b <<"\t"<< r << endl;
//
//	return 0;
//}
#include<iostream>
// opencv函数库的头文件
#include"opencv2/opencv.hpp"
int main(void)
{
	// 把下面的图片路径改为自己的路径，注意 '/'E:\0卢韬\摄影
	const char* imagePath = "E:/0penCV/tst_pic1.jpg";
	cv::namedWindow("src");
	cv::namedWindow("srcTmp");
	cv::Mat srcImg = cv::imread(imagePath);
	if (srcImg.empty())
	{
		cv::destroyWindow("src");
		cv::destroyWindow("srcTmp");
		std::cerr << imagePath << " " << " not found\n";
		return -1;
	} 
	//cv::Mat srcImgTmp = srcImg;
	cv::Mat srcImgTmp = srcImg.clone();
	cv::Point imgCenter = cv::Point(srcImg.size().width / 2, srcImg.size().height / 2);
	// 彩色图转灰色图
	cv::circle(srcImg, imgCenter, 13, cv::Scalar(0, 0, 255), 4);
	// 显示图片
	cv::imshow("src", srcImg);
	cv::imshow("srcTmp", srcImgTmp);
	cv::waitKey(0);
	return 0;
}