
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
	{ printf("��ȡͼƬ������ȷ��Ŀ¼���Ƿ���imread����ָ����ͼƬ����~�� \n"); return false; }
	resize(imgread, imgread, Size(640, 480));
	imshow("ԭʼͼ", imgread);


	//ȡ��ͨ��
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

	////��ʴ
	////MORPH_RECTΪ���ǵ���״  CROSSԲ  kernelΪ��С ê��Ĭ������Point(-1,-1)
	//Mat element = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	//Mat dst;
	//erode(img, img, element);
	//imshow("��ʴ", img);

	////����
	//Mat element2dila = getStructuringElement(MORPH_ELLIPSE, Size(12, 12));
	//Mat dila = img;
	//morphologyEx(img, img, MORPH_DILATE, element2dila);
	//imshow("����", img);

	//cvAdaptiveThreshold(img, img,);

	//����԰�任
	//GaussianBlur(img, img, Size(7, 7), 2, 2);
	vector<Vec3f> circles;
	HoughCircles(img, circles, HOUGH_GRADIENT, 1.5, 24, 300, 38, 0, 15);

	//дԲȦ
	for (size_t i = 0; i < circles.size(); i++)
	{

		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);

		circle(imgread, center, 3, Scalar(0, 255, 0), -1, 8, 0);

		circle(imgread, center, radius, Scalar(155, 50, 255), 3, 8, 0);
	}
	imshow("preFinal", imgread);

//	//��ʴ
////MORPH_RECTΪ���ǵ���״  CROSSԲ  kernelΪ��С ê��Ĭ������Point(-1,-1)
//	erode(img2, img2, element);
//	imshow("��ʴ1", img2);
//
//	//����
//
//	morphologyEx(img2, img2, MORPH_DILATE, element2dila);
//	imshow("����1", img2);
//
//	//cvAdaptiveThreshold(img, img,);
//
//	//����԰�任
//	GaussianBlur(img2, img2, Size(7, 7), 2, 2);
//	HoughCircles(img2, circles, HOUGH_GRADIENT, 1.5, 30, 30, 30, 0, 0);
//
//	//дԲȦ
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
