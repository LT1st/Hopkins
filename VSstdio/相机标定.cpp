#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <math.h> 

using namespace cv;
using namespace std;

Mat thresholding(Mat input);
Mat drawrect(Mat input, Mat output);
Mat FindoutLine(Mat input, Mat target);
Mat src; Mat src_gray;

//Scalar colorful = CV_RGB(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));

//获取轮廓的中心点
Point Center_cal(vector<vector<Point> > contours, int i);
int Start, Stop, Tim, a, b;

void codeRotateByZ(double x, double y, double thetaz, double& outx, double& outy)

{

	double x1 = x;//将变量拷贝一次，保证&x == &outx这种情况下也能计算正确

	double y1 = y;

	double rz = thetaz * CV_PI / 180;

	outx = cos(rz) * x1 - sin(rz) * y1;

	outy = sin(rz) * x1 + cos(rz) * y1;



}

//将空间点绕Y轴旋转

//输入参数 x z为空间点原始x z坐标

//thetay为空间点绕Y轴旋转多少度，角度制范围在-180到180

//outx outz为旋转后的结果坐标

void codeRotateByY(double x, double z, double thetay, double& outx, double& outz)

{

	double x1 = x;

	double z1 = z;

	double ry = thetay * CV_PI / 180;

	outx = cos(ry) * x1 + sin(ry) * z1;

	outz = cos(ry) * z1 - sin(ry) * x1;

}

//将空间点绕X轴旋转
//输入参数 y z为空间点原始y z坐标
//thetax为空间点绕X轴旋转多少度，角度制范围在-180到180
//outy outz为旋转后的结果坐标

void codeRotateByX(double y, double z, double thetax, double& outy, double& outz)
{
	double y1 = y;//将变量拷贝一次，保证&y == &y这种情况下也能计算正确
	double z1 = z;
	double rx = thetax * CV_PI / 180;
	outy = cos(rx) * y1 - sin(rx) * z1;
	outz = cos(rx) * z1 + sin(rx) * y1;

}


int main()
{

	Mat src_all = src.clone();
	Mat yuantu = src.clone();
	Scalar color = Scalar(1, 1, 255);
	Mat threshold_output;
	//cv::Mat QR_world;
	vector<cv::Point3d> QR_world;
	QR_world.push_back(cv::Point3f(-20.0f, +20.0f, 0));
	QR_world.push_back(cv::Point3f(+20.0f, +20.0f, 0));
	QR_world.push_back(cv::Point3f(+20.0f, -20.0f, 0));
	QR_world.push_back(cv::Point3f(-20.0f, -20.0f, 0));
	namedWindow("框选");
	VideoCapture capture(0);

	capture.open(0);                             //打开相机，电脑自带摄像头一般编号为0，外接摄像头编号为1，主要是在设备管理器中查看自己摄像头的编号。

	capture.set(CAP_PROP_FRAME_WIDTH, 640);  //设置捕获视频的宽度
	capture.set(CAP_PROP_FRAME_HEIGHT, 480);  //设置捕获视频的高度

	if (!capture.isOpened())                         //判断是否成功打开相机
	{
		cout << "摄像头打开失败!" << endl;
		return -1;
	}

	while (1)
	{
		capture >> src;

		Mat src_all = src.clone();
		//cvtColor(src_all, src_all, COLOR_BGR2GRAY);
		QRCodeDetector QRdetecter;
		vector<cv::Point2d> list;	//Point2f list[4];
		QRdetecter.detect(src_all, list);

		if ((list.size() > 3) && (list.size() < 5))
		{
			line(src_all, list[0], list[1], Scalar(0, 255, 0), 3);

			line(src_all, list[1], list[2], Scalar(0, 255, 0), 6);

			line(src_all, list[2], list[3], Scalar(0, 255, 0), 9);

			line(src_all, list[3], list[0], Scalar(0, 255, 0), 12);




			/*	Mat camMatrix;
				Mat distCoeff;*/
			Mat Rvec;
			Mat_<double> Tvec;
			Mat raux, taux;



			//初始化相机参数Opencv
			float camD[9] = {
				1038.2,	0,		342.2 ,
				0,		1036.3,	254.4 ,
				0,		0,		1 };
			cv::Mat camera_matrix = cv::Mat(3, 3, CV_32FC1, camD);

			//畸变参数
			float distCoeffD[5] = { -0.4710   , 0.4779, 0, 0, 0 };
			cv::Mat distortion_coefficients = cv::Mat(1, 5, CV_32FC1, distCoeffD);





			solvePnP(QR_world, list, camera_matrix, distortion_coefficients, raux, taux, false, SOLVEPNP_EPNP);

			Mat rotMat;

			Rodrigues(raux, rotMat);  //由于solvePnP返回的是旋转向量，故用罗德里格斯变换变成旋转矩阵



			double r11 = rotMat.ptr<double>(0)[0];
			double r12 = rotMat.ptr<double>(0)[1];
			double r13 = rotMat.ptr<double>(0)[2];
			double r21 = rotMat.ptr<double>(1)[0];
			double r22 = rotMat.ptr<double>(1)[1];
			double r23 = rotMat.ptr<double>(1)[2];
			double r31 = rotMat.ptr<double>(2)[0];
			double r32 = rotMat.ptr<double>(2)[1];
			double r33 = rotMat.ptr<double>(2)[2];
			/*************************************此处计算出相机的旋转角**********************************************/
				//计算出相机坐标系的三轴旋转欧拉角，旋转后可以转出世界坐标系。
				//旋转顺序为z、y、x
				//原理见帖子：
			double thetaz = atan2(r21, r11) / CV_PI * 180;
			double thetay = atan2(-1 * r31, sqrt(r32 * r32 + r33 * r33)) / CV_PI * 180;
			double thetax = atan2(r32, r33) / CV_PI * 180;




			//提出平移矩阵，表示从相机坐标系原点，跟着向量(x,y,z)走，就到了世界坐标系原点
			double tx = taux.ptr<double>(0)[0];
			double ty = taux.ptr<double>(0)[1];
			double tz = taux.ptr<double>(0)[2];

			//x y z 为唯一向量在相机原始坐标系下的向量值
			//也就是向量OcOw在相机坐标系下的值
			double x = tx, y = ty, z = tz;

			//进行三次反向旋转
			codeRotateByZ(x, y, -1 * thetaz, x, y);
			codeRotateByY(x, z, -1 * thetay, x, z);
			codeRotateByX(y, z, -1 * thetax, y, z);


			//获得相机在世界坐标系下的位置坐标
			//即向量OcOw在世界坐标系下的值
			double Cx = x * -1;
			double Cy = y * -1;
			double Cz = z * -1;

			if ((Cz > 0) && (Cz < 500))
			{
				cout << Cz << endl << endl;
			}



		}
		/*namedWindow("框选");*/
		imshow("框选", src_all);
		waitKey(30);

	}
	return(0);
}

