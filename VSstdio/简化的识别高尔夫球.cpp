
#include <opencv2/opencv.hpp>
#include <iostream>
#include<time.h>
using namespace std;
using namespace cv;


int main()
{
	//Mat imgread = imread("2.jpg");
	Mat imgread = imread("tst_Color.png");

	if (!imgread.data)
	{ printf("读取图片错误，请确定目录下是否有imread函数指定的图片存在~！ \n"); return false; }
	resize(imgread, imgread, Size(640, 480));
	imshow("原始图", imgread);


	//取单通道
	Mat img = imgread, img2;
	vector<Mat>channels;
	split(img, channels);
	//img = channels.at(0);
	//imshow("0", img);
	img = channels.at(1);
	imshow("1", img);
	//img = channels.at(2);
	//imshow("2", img);
	//img = channels.at(3);
	//imshow("3", img);
//	img2 = channels.at(0);

	////腐蚀
	////MORPH_RECT为覆盖的形状  CROSS圆  kernel为大小 锚点默认中心Point(-1,-1)
	//Mat element = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	//Mat dst;
	//erode(img, img, element);
	//imshow("腐蚀", img);

	////膨胀
	//Mat element2dila = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	//Mat dila = img;
	//morphologyEx(img, img, MORPH_DILATE, element2dila);
	//imshow("膨胀", img);

	//cvAdaptiveThreshold(img, img,);

	//霍夫园变换
	//GaussianBlur(img, img, Size(7, 7), 2, 2);
	vector<Vec3f> circles;
	HoughCircles(img, circles, HOUGH_GRADIENT, 1.5, 24, 300, 38, 0, 15);

	//写圆圈
	for (size_t i = 0; i < circles.size(); i++)
	{

		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);

		circle(imgread, center, 3, Scalar(0, 255, 0), -1, 8, 0);

		circle(imgread, center, radius, Scalar(155, 50, 255), 3, 8, 0);
	}
	imshow("preFinal", imgread);

//	//腐蚀
////MORPH_RECT为覆盖的形状  CROSS圆  kernel为大小 锚点默认中心Point(-1,-1)
//	erode(img2, img2, element);
//	imshow("腐蚀1", img2);
//
//	//膨胀
//
//	morphologyEx(img2, img2, MORPH_DILATE, element2dila);
//	imshow("膨胀1", img2);
//
//	//cvAdaptiveThreshold(img, img,);
//
//	//霍夫园变换
//	GaussianBlur(img2, img2, Size(7, 7), 2, 2);
//	HoughCircles(img2, circles, HOUGH_GRADIENT, 1.5, 30, 30, 30, 0, 0);
//
//	//写圆圈
//	for (size_t i = 0; i < circles.size(); i++)
//	{
//
//		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
//		int radius = cvRound(circles[i][2]);
//
//		circle(imgread, center, 3, Scalar(0, 255, 0), -1, 8, 0);
//
//		circle(imgread, center, radius, Scalar(155, 50, 255), 3, 8, 0);
//	}
//
//
//	imshow("Final", imgread);

	waitKey(0);

	return 0;
}
