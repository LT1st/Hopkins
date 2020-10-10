#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <math.h> 

using namespace cv;
using namespace std;


Mat src; Mat src_gray;

//Scalar colorful = CV_RGB(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));

//获取轮廓的中心点
Point Center_cal(vector<vector<Point> > contours, int i);



int main()
{



	src = imread("3.jpg", 1);
	Mat src_all = src.clone();


	//彩色图转灰度图
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	//对图像进行平滑处理
	blur(src_gray, src_gray, Size(3, 3));
	//使灰度图象直方图均衡化
	equalizeHist(src_gray, src_gray);
	namedWindow("灰度图");
	imshow("灰度图", src_gray);


	Scalar color = Scalar(1, 1, 255);
	Mat threshold_output;
	vector<vector<Point> > contours, contours2;
	vector<Vec4i> hierarchy;
	Mat s1 = Mat::zeros(src.size(), CV_8UC3);
	Mat s2 = Mat::zeros(src.size(), CV_8UC3);
	Mat sss = Mat::zeros(src.size(), CV_8UC3);

	//指定阀值进行二值化
	threshold(src_gray, threshold_output, 70, 255, THRESH_BINARY);
	namedWindow("阈值图");
	imshow("阈值图", threshold_output);


	//查找轮廓
	//   参数说明
	//	输入图像image必须为一个2值单通道图像
	//	contours参数为检测的轮廓数组，每一个轮廓用一个point类型的vector表示
	//	hiararchy参数和轮廓个数相同，每个轮廓contours[ i ]对应4个hierarchy元素hierarchy[ i ][ 0 ] ~hierarchy[ i ][ 3 ]，
	//		分别表示后一个轮廓、前一个轮廓、父轮廓、内嵌轮廓的索引编号，如果没有对应项，该值设置为负数。
	//	mode表示轮廓的检索模式
	//		CV_RETR_EXTERNAL 表示只检测外轮廓
	//		CV_RETR_LIST 检测的轮廓不建立等级关系
	//		CV_RETR_CCOMP 建立两个等级的轮廓，上面的一层为外边界，里面的一层为内孔的边界信息。如果内孔内还有一个连通物体，这个物体的边界也在顶层。
	//		CV_RETR_TREE 建立一个等级树结构的轮廓。具体参考contours.c这个demo
	//	method为轮廓的近似办法
	//		CV_CHAIN_APPROX_NONE 存储所有的轮廓点，相邻的两个点的像素位置差不超过1，即max（abs（x1-x2），abs（y2-y1））==1
	//		CV_CHAIN_APPROX_SIMPLE 压缩水平方向，垂直方向，对角线方向的元素，只保留该方向的终点坐标，例如一个矩形轮廓只需4个点来保存轮廓信息
	//		CV_CHAIN_APPROX_TC89_L1，CV_CHAIN_APPROX_TC89_KCOS 使用teh-Chinl chain 近似算法
	//	offset表示代表轮廓点的偏移量，可以设置为任意值。对ROI图像中找出的轮廓，并要在整个图像中进行分析时，这个参数还是很有用的。
	findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));

	int c = 0, j = 0, k = 0, area = 0;

	//通过黑色定位角作为父轮廓，有两个子轮廓的特点，筛选出三个定位角
	int parentIdx = -1;
	for (int i = 0; i < contours.size(); i++)
	{
		//画出所以轮廓图

		//void drawContours(InputOutputArray image, InputArrayOfArrays contours, int contourIdx, const Scalar & color, int thickness = 1, int lineType = 8, InputArray hierarchy = noArray(), int maxLevel = INT_MAX, Point offset = Point())

		//	函数参数详解：
		//	其中第一个参数image表示目标图像，
		//	第二个参数contours表示输入的轮廓组，每一组轮廓由点vector构成，
		//	第三个参数contourIdx指明画第   几个轮廓，如果该参数为负值，则画全部轮廓，
		//	第四个参数color为轮廓的颜色，
		//	第五个参数thickness为轮廓的线宽，如果为负值或CV_FILLED表示填充轮廓内部，
		//	第六个参数lineType为线型，
		//	第七个参数为轮廓结构信息，
		//	第八个参数为maxLevel		
		drawContours(sss, contours, parentIdx, CV_RGB(0, 255, 255), 1, 8);
		if (hierarchy[i][2] != -1 && j == 0)
		{
			parentIdx = i;
			j++;
		}
		else if (hierarchy[i][2] != -1)
		{
			j++;
		}
		else if (hierarchy[i][2] == -1)
		{
			j = 0;
			parentIdx = -1;
		}

		//有两个子轮廓
		if (j >= 2)
		{
			//保存定位角
			contours2.push_back(contours[parentIdx]);
			//画出定轮廓
			drawContours(s1, contours, parentIdx, CV_RGB(0, 255, 0), 1, 8);
			j = 0;
			parentIdx = -1;
		}
	}

	//画出定位角轮廓
	for (int i = 0; i < contours2.size(); i++)
		drawContours(s2, contours2, i, CV_RGB(255, 255, 0), FILLED, 4, hierarchy[k][2], 0, Point());

	//三个定位角的中心坐标
	Point point[3];
	for (int i = 0; i < contours2.size(); i++)
	{
		point[i] = Center_cal(contours2, i);
		cout << "\t" << point[i] << endl;
	}

	//轮廓面积,定位角的面积，计算边长
	area = contourArea(contours2[1]);
	cout << "\t" << area << endl;
	int area_side = cvRound(sqrt(double(area)));
	cout << "\t" << area_side << endl;
	for (int i = 0; i < contours2.size(); i++)
	{
		//画出三个定位角的中心连线
		line(s2, point[i % contours2.size()], point[(i + 1) % contours2.size()], Scalar(255, 255, 255), area_side / 2, 8);
		/*void line(Mat & img, Point pt1, Point pt2, const Scalar & color, int thickness = 1, int lineType = 8, int shift = 0)

			参数：

			img : 要绘制线段的图像。
			pt1 : 线段的起点。
			pt2 : 线段的终点。
			color : 线段的颜色，通过一个Scalar对象定义。
			thickness : 线条的宽度。
			lineType : 线段的类型。可以取值8， 4， 和CV_AA， 分别代表8邻接连接线，4邻接连接线和反锯齿连接线。默认值为8邻接。为了获得更好地效果可以选用CV_AA(采用了高斯滤波)。
			shift : 坐标点小数点位数。*/
	}

	namedWindow("s1");
	imshow("s1", s1);

	namedWindow("s2");
	imshow("s2", s2);


	//Mat s2;
	//s2 = imread("3.jpg", 1);
	//Mat src_all = s2.clone();
	//框出这整个二维码
	Mat gray_all, threshold_output_all;
	vector<vector<Point> > contours_all;
	vector<Vec4i> hierarchy_all;
	cvtColor(s2, gray_all, COLOR_BGR2GRAY);


	threshold(gray_all, threshold_output_all, 45, 255, THRESH_BINARY);
	findContours(threshold_output_all, contours_all, hierarchy_all, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));//RETR_EXTERNAL表示只寻找最外层轮廓


	Point2f fourPoint2f[4];
	//求最小包围矩形
	RotatedRect rectPoint = minAreaRect(contours_all[0]);
	//1、minAreaRect函数

	//	函数作用：

	//	主要求得包含点集最小面积的矩形，，这个矩形是可以有偏转角度的，可以与图像的边界不平行

	//	2、minAreaRect函数调用形式

	//	C++: RotatedRect minAreaRect(InputArray points)

	//	InputArray points：表示输入的点集

	//	输出是矩形的四个点坐标

	//将rectPoint变量中存储的坐标值放到 fourPoint的数组中
	rectPoint.points(fourPoint2f);


	for (int i = 0; i < 4; i++)
	{
		line(src_all, fourPoint2f[i % 4], fourPoint2f[(i + 1) % 4]
			, Scalar(0, 0, 255), 3);
	}

	namedWindow("框选");
	imshow("框选", src_all);


	waitKey(0);
	return(0);
}


//获取轮廓的中心点
Point Center_cal(vector<vector<Point> > contours, int i)
{
	int centerx = 0, centery = 0, n = contours[i].size();
	//在提取的小正方形的边界上每隔周长个像素提取一个点的坐标，
	//求所提取四个点的平均坐标（即为小正方形的大致中心）
	centerx = (contours[i][n / 4].x + contours[i][n * 2 / 4].x + contours[i][3 * n / 4].x + contours[i][n - 1].x) / 4;
	centery = (contours[i][n / 4].y + contours[i][n * 2 / 4].y + contours[i][3 * n / 4].y + contours[i][n - 1].y) / 4;
	Point point1 = Point(centerx, centery);
	return point1;
}
