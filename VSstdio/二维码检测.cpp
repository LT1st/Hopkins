#include <opencv2/opencv.hpp>
#include <iostream>    
#include <opencv2\core\core.hpp>
#include <iostream>  
#include <stdio.h>
#include <string>
#include <sstream>

using namespace cv;
using namespace std;


// 用于矫正
Mat transformCorner(Mat src, RotatedRect rect);
Mat transformQRcode(Mat src, RotatedRect rect, double angle);
// 用于判断角点
bool IsQrPoint(vector<Point>& contour, Mat& img);
bool isCorner(Mat& image);
double Rate(Mat& count);
int leftTopPoint(vector<Point> centerPoint);
vector<int> otherTwoPoint(vector<Point> centerPoint, int leftTopPointIndex);
double rotateAngle(Point leftTopPoint, Point rightTopPoint, Point leftBottomPoint);
Mat FindoutLine(Mat input, Mat target);
Mat drawrect(Mat input, Mat output);
Point Center_cal(vector<vector<Point> > contours, int i);

int  time2cal = 0, singleposses = 0;

int main()
{
	int tiaocanshubiankuang = 5;

	VideoCapture cap;
	Mat src;
	cap.open(0);                             //打开相机，电脑自带摄像头一般编号为0，外接摄像头编号为1，主要是在设备管理器中查看自己摄像头的编号。

	cap.set(CAP_PROP_FRAME_WIDTH, 720);  //设置捕获视频的宽度
	cap.set(CAP_PROP_FRAME_HEIGHT, 480);  //设置捕获视频的高度

	if (!cap.isOpened())                         //判断是否成功打开相机
	{
		cout << "摄像头打开失败!" << endl;
		return -1;
	}

	while (1) {

		int time = getTickCount() * 1000 / getTickFrequency();

		Mat src;
		//src = imread("QRcode.jpg");
		cap >> src;                                //从相机捕获一帧图像

		Mat srcCopy = src.clone();
		//canvas为画布 将找到的定位特征画出来
		Mat canvas = Mat::zeros(src.size(), CV_8UC3);
		Mat srcGray;

		//center_all获取特性中心
		vector<Point2d> center_all;
		// 转化为灰度图
		cvtColor(src, srcGray, COLOR_BGR2GRAY);

		// 3X3模糊
		//blur(srcGray, srcGray, Size(3, 3));

		// 计算直方图
		//equalizeHist(srcGray, srcGray);
		//int s = srcGray.at<Vec3b>(0, 0)[0];
		// 设置阈值根据实际情况 如视图中已找不到特征 可适量调整
		threshold(srcGray, srcGray, 70, 255, THRESH_BINARY | THRESH_OTSU);

		//Canny(srcGray, srcGray, 50, 150, 3);
		//腐蚀膨胀
		//Mat erodeStruct = getStructuringElement(MORPH_RECT, Size(3, 3));
		//morphologyEx(srcGray, srcGray, MORPH_CLOSE, erodeStruct);
		imshow("threshold", srcGray);
		/*contours是第一次寻找轮廓*/
		/*contours2是筛选出的轮廓*/



		vector<vector<Point>> contours, contours2;
		//	用于轮廓检测;
		vector<Vec4i> hierarchy;
		/*
		1、srcGray是需要寻找轮廓的图像；
		2、contours是寻找到的轮廓的存储变量，其变量类型为vector<vector>，
		向量内每个元素保存了一组由连续的Point点构成的点的集合的向量。
		3、hierarchy是轮廓之间的关系，其变量类型为vector，
		每一个hierarchy的元素包括了四个int的数据，分别表示第i个轮廓的后一个轮廓、
		前一个轮廓、子轮廓、父轮廓的索引编号。
		TIPS:
		输入图像image必须为一个2值单通道图像
		contours参数为检测的轮廓数组，每一个轮廓用一个point类型的vector表示
		hiararchy参数和轮廓个数相同，每个轮廓contours[ i ]对应4个hierarchy元素hierarchy[ i ][ 0 ] ~hierarchy[ i ][ 3 ]，
			分别表示后一个轮廓、前一个轮廓、父轮廓、内嵌轮廓的索引编号，如果没有对应项，该值设置为负数。
		mode表示轮廓的检索模式
			CV_RETR_EXTERNAL 表示只检测外轮廓
			CV_RETR_LIST 检测的轮廓不建立等级关系
			CV_RETR_CCOMP 建立两个等级的轮廓，上面的一层为外边界，里面的一层为内孔的边界信息。如果内孔内还有一个连通物体，这个物体的边界也在顶层。
			CV_RETR_TREE 建立一个等级树结构的轮廓。具体参考contours.c这个demo
		method为轮廓的近似办法
			CV_CHAIN_APPROX_NONE 存储所有的轮廓点，相邻的两个点的像素位置差不超过1，即max（abs（x1-x2），abs（y2-y1））==1
			CV_CHAIN_APPROX_SIMPLE 压缩水平方向，垂直方向，对角线方向的元素，只保留该方向的终点坐标，例如一个矩形轮廓只需4个点来保存轮廓信息
			CV_CHAIN_APPROX_TC89_L1，CV_CHAIN_APPROX_TC89_KCOS 使用teh-Chinl chain 近似算法
		offset表示代表轮廓点的偏移量，可以设置为任意值。对ROI图像中找出的轮廓，并要在整个图像中进行分析时，这个参数还是很有用的。

		*/
		findContours(srcGray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

		//	drawContours(srcGray, contours, -1, Scalar(0, 255, 0),2,8,hierarchy);
		Mat temp_srcGray = srcGray.clone();
		Mat temp_srcGray1 = srcGray.clone();
		Mat temp_srcGray2 = srcGray.clone();

		//显示检测到的轮廓
		/*drawContours函数参数详解：
		第一个参数image表示目标图像，
		第二个参数contours表示输入的轮廓组，每一组轮廓由点vector构成，
		第三个参数contourIdx指明画第   几个轮廓，如果该参数为负值，则画全部轮廓，
		第四个参数color为轮廓的颜色，
		第五个参数thickness为轮廓的线宽，如果为负值或CV_FILLED表示填充轮廓内部，
		第六个参数lineType为线型，
		第七个参数为轮廓结构信息，
		第八个参数为maxLevel*/
		//drawContours(temp_srcGray2, contours, -1, CV_RGB(0, 255, 255), 2, 8);
		//drawContours(temp_srcGray, contours, -1, Scalar(0, 255, 0), 2, LINE_8, hierarchy, 0);
		////drawContours(mask, contours, 0, Scalar(255, 255, 255), -1);
		///*最后的0@param maxLevel绘制轮廓的最大水平。如果为0，则仅绘制指定的轮廓。
		//如果为1，该函数将绘制轮廓和所有嵌套轮廓。如果为2，则函数
		//绘制轮廓，所有嵌套轮廓，所有从嵌套到嵌套的轮廓，等等。这个
		//仅当有可用的层次结构时，才考虑该参数。*/
		//imshow("全部显示1", temp_srcGray2);
		//imshow("全部显示", temp_srcGray);

		//Mat yuantu = FindoutLine(temp_srcGray1, src);

		Mat output;
		//vector<vector<Point> > contours, contours2;
		//vector<Vec4i> hierarchy;
		Mat input = temp_srcGray1;
		Mat target = src;
		Mat blackpicture = Mat::zeros(target.size(), CV_8UC3);
		Mat s1 = blackpicture.clone();
		Mat s2 = blackpicture.clone();
		Mat sss = blackpicture.clone();
		//
		//findContours(input, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));

		int c = 0, j = 0, k = 0, area = 0;

		//通过黑色定位角作为父轮廓，有两个子轮廓的特点，筛选出三个定位角
		int parentIdx = -1;
		for (int i = 0; i < contours.size(); i++)
		{
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
			if (j == 2)
			{
				//保存定位角
				contours2.push_back(contours[parentIdx]);
				//画出定轮廓
				drawContours(s1, contours, parentIdx, CV_RGB(0, 255, 0), 1, 8);
				j = 0;
				parentIdx = -1;
			}
		}
		if (contours2.size() == 3 || contours2.size() == 4)
		{
			//画出定位角轮廓
			for (int i = 0; i < contours2.size(); i++)
				drawContours(s2, contours2, i, CV_RGB(255, 255, 0), FILLED, 4, hierarchy[k][2], 0, Point());
			//三个定位角的中心坐标
			Point point[4];
			for (int i = 0; i < contours2.size(); i++)
			{
				point[i] = Center_cal(contours2, i);
			}
			//轮廓面积,定位角的面积，计算边长
			area = contourArea(contours2[1]);
			int area_side = cvRound(sqrt(double(area)));
			for (int i = 0; i < contours2.size(); i++)
			{
				//画出三个定位角的中心连线
				line(s2, point[i % contours2.size()], point[(i + 1) % contours2.size()], Scalar(105, 75, 155), area_side, 8);
			}

			output = drawrect(s2, target);
			contours2.clear();
			//return output;
		}
		else
		{
			output = target;
			//return output;
		}

		imshow("框选", output);

		waitKey(1);
	}
	return 0;
}

Mat transformCorner(Mat src, RotatedRect rect)
{
	// 获得旋转中心
	Point center = rect.center;
	// 获得左上角和右下角的角点，而且要保证不超出图片范围，用于抠图
	Point TopLeft = Point(cvRound(center.x), cvRound(center.y)) - Point(rect.size.height / 2, rect.size.width / 2);  //旋转后的目标位置
	TopLeft.x = TopLeft.x > src.cols ? src.cols : TopLeft.x;
	TopLeft.x = TopLeft.x < 0 ? 0 : TopLeft.x;
	TopLeft.y = TopLeft.y > src.rows ? src.rows : TopLeft.y;
	TopLeft.y = TopLeft.y < 0 ? 0 : TopLeft.y;

	int after_width, after_height;
	if (TopLeft.x + rect.size.width > src.cols) {
		after_width = src.cols - TopLeft.x - 1;
	}
	else {
		after_width = rect.size.width - 1;
	}
	if (TopLeft.y + rect.size.height > src.rows) {
		after_height = src.rows - TopLeft.y - 1;
	}
	else {
		after_height = rect.size.height - 1;
	}
	// 获得二维码的位置
	Rect RoiRect = Rect(TopLeft.x, TopLeft.y, after_width, after_height);

	//	dst是被旋转的图片 roi为输出图片 mask为掩模
	double angle = rect.angle;
	Mat mask, roi, dst;
	Mat image;
	// 建立中介图像辅助处理图像

	vector<Point> contour;
	// 获得矩形的四个点
	Point2f points[4];
	rect.points(points);
	for (int i = 0; i < 4; i++)
		contour.push_back(points[i]);

	vector<vector<Point>> contours;
	contours.push_back(contour);
	// 再中介图像中画出轮廓
	drawContours(mask, contours, 0, Scalar(255, 255, 255), -1);
	// 通过mask掩膜将src中特定位置的像素拷贝到dst中。
	src.copyTo(dst, mask);
	// 旋转
	Mat M = getRotationMatrix2D(center, angle, 1);
	warpAffine(dst, image, M, src.size());
	// 截图
	roi = image(RoiRect);

	return roi;
}

// 该部分用于检测是否是角点，与下面两个函数配合
bool IsQrPoint(vector<Point>& contour, Mat& img) {
	double area = contourArea(contour);
	// 角点不可以太小
	if (area < 30)
		return 0;
	RotatedRect rect = minAreaRect(Mat(contour));
	double w = rect.size.width;
	double h = rect.size.height;
	double rate = min(w, h) / max(w, h);
	if (rate > 0.7)
	{
		// 返回旋转后的图片，用于把“回”摆正，便于处理
		Mat image = transformCorner(img, rect);
		if (isCorner(image))
		{
			return 1;
		}
	}
	return 0;
}

// 计算内部所有白色部分占全部的比率
double Rate(Mat& count)
{
	int number = 0;
	int allpixel = 0;
	for (int row = 0; row < count.rows; row++)
	{
		for (int col = 0; col < count.cols; col++)
		{
			if (count.at<uchar>(row, col) == 255)
			{
				number++;
			}
			allpixel++;
		}
	}
	//cout << (double)number / allpixel << endl;
	return (double)number / allpixel;
}

// 用于判断是否属于角上的正方形
bool isCorner(Mat& image)
{
	// 定义mask
	Mat imgCopy, dstCopy;
	Mat dstGray;
	imgCopy = image.clone();

	// 转化为灰度图像
	cvtColor(image, dstGray, COLOR_BGR2GRAY);
	// 进行二值化

	threshold(dstGray, dstGray, 0, 255, THRESH_BINARY | THRESH_OTSU);
	dstCopy = dstGray.clone();  //备份

	Mat erodeStruct = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(dstCopy, dstCopy, MORPH_OPEN, erodeStruct);
	// 找到轮廓与传递关系
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(dstCopy, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);


	for (int i = 0; i < contours.size(); i++)
	{
		//cout << i << endl;
		if (hierarchy[i][2] == -1 && hierarchy[i][3] != -1)
		{

			Rect rect = boundingRect(Mat(contours[i]));
			rectangle(image, rect, Scalar(0, 0, 255), 2);

			// 最里面的矩形与最外面的矩形的对比
			if (rect.width < imgCopy.cols * 2 / 7)      //2/7是为了防止一些微小的仿射
				continue;
			if (rect.height < imgCopy.rows * 2 / 7)      //2/7是为了防止一些微小的仿射
				continue;

			// 判断其中黑色与白色的部分的比例
			if (Rate(dstGray) > 0.20)
			{
				return true;
			}
		}
	}
	return  false;
}

int leftTopPoint(vector<Point> centerPoint) {
	int minIndex = 0;
	int multiple = 0;
	int minMultiple = 10000;
	multiple = (centerPoint[1].x - centerPoint[0].x) * (centerPoint[2].x - centerPoint[0].x) + (centerPoint[1].y - centerPoint[0].y) * (centerPoint[2].y - centerPoint[0].y);
	if (minMultiple > multiple) {
		minIndex = 0;
		minMultiple = multiple;
	}
	multiple = (centerPoint[0].x - centerPoint[1].x) * (centerPoint[2].x - centerPoint[1].x) + (centerPoint[0].y - centerPoint[1].y) * (centerPoint[2].y - centerPoint[1].y);
	if (minMultiple > multiple) {
		minIndex = 1;
		minMultiple = multiple;
	}
	multiple = (centerPoint[0].x - centerPoint[2].x) * (centerPoint[1].x - centerPoint[2].x) + (centerPoint[0].y - centerPoint[2].y) * (centerPoint[1].y - centerPoint[2].y);
	if (minMultiple > multiple) {
		minIndex = 2;
		minMultiple = multiple;
	}
	return minIndex;
}

vector<int> otherTwoPoint(vector<Point> centerPoint, int leftTopPointIndex) {
	vector<int> otherIndex;
	double waiji = (centerPoint[(leftTopPointIndex + 1) % 3].x - centerPoint[(leftTopPointIndex) % 3].x) *
		(centerPoint[(leftTopPointIndex + 2) % 3].y - centerPoint[(leftTopPointIndex) % 3].y) -
		(centerPoint[(leftTopPointIndex + 2) % 3].x - centerPoint[(leftTopPointIndex) % 3].x) *
		(centerPoint[(leftTopPointIndex + 1) % 3].y - centerPoint[(leftTopPointIndex) % 3].y);
	if (waiji > 0) {
		otherIndex.push_back((leftTopPointIndex + 1) % 3);
		otherIndex.push_back((leftTopPointIndex + 2) % 3);
	}
	else {
		otherIndex.push_back((leftTopPointIndex + 2) % 3);
		otherIndex.push_back((leftTopPointIndex + 1) % 3);
	}
	return otherIndex;
}

double rotateAngle(Point leftTopPoint, Point rightTopPoint, Point leftBottomPoint) {
	double dy = rightTopPoint.y - leftTopPoint.y;
	double dx = rightTopPoint.x - leftTopPoint.x;
	double k = dy / dx;
	double angle = atan(k) * 180 / CV_PI;//转化角度
	if (leftBottomPoint.y < leftTopPoint.y)
		angle -= 180;
	return angle;
}

Mat transformQRcode(Mat src, RotatedRect rect, double angle)
{
	// 获得旋转中心
	Point center = rect.center;
	// 获得左上角和右下角的角点，而且要保证不超出图片范围，用于抠图
	Point TopLeft = Point(cvRound(center.x), cvRound(center.y)) - Point(rect.size.height / 2, rect.size.width / 2);  //旋转后的目标位置
	TopLeft.x = TopLeft.x > src.cols ? src.cols : TopLeft.x;
	TopLeft.x = TopLeft.x < 0 ? 0 : TopLeft.x;
	TopLeft.y = TopLeft.y > src.rows ? src.rows : TopLeft.y;
	TopLeft.y = TopLeft.y < 0 ? 0 : TopLeft.y;

	int after_width, after_height;
	if (TopLeft.x + rect.size.width > src.cols) {
		after_width = src.cols - TopLeft.x - 1;
	}
	else {
		after_width = rect.size.width - 1;
	}
	if (TopLeft.y + rect.size.height > src.rows) {
		after_height = src.rows - TopLeft.y - 1;
	}
	else {
		after_height = rect.size.height - 1;
	}
	// 获得二维码的位置
	Rect RoiRect = Rect(TopLeft.x, TopLeft.y, after_width, after_height);

	// dst是被旋转的图片，roi为输出图片，mask为掩模
	Mat mask, roi, dst;
	Mat image;
	// 建立中介图像辅助处理图像

	vector<Point> contour;
	// 获得矩形的四个点
	Point2f points[4];
	rect.points(points);
	for (int i = 0; i < 4; i++)
		contour.push_back(points[i]);

	vector<vector<Point>> contours;
	contours.push_back(contour);
	// 再中介图像中画出轮廓
	drawContours(mask, contours, 0, Scalar(255, 255, 255), -1);
	// 通过mask掩膜将src中特定位置的像素拷贝到dst中。
	src.copyTo(dst, mask);
	// 旋转
	Mat M = getRotationMatrix2D(center, angle, 1);
	warpAffine(dst, image, M, src.size());
	// 截图
	roi = image(RoiRect);

	return roi;
}


Mat FindoutLine(Mat input, Mat target)
{
	Mat output;
	vector<vector<Point> > contours, contours2;
	vector<Vec4i> hierarchy;
	Mat blackpicture = Mat::zeros(target.size(), CV_8UC3);
	Mat s1 = blackpicture.clone();
	Mat s2 = blackpicture.clone();
	Mat sss = blackpicture.clone();
	findContours(input, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));

	int c = 0, j = 0, k = 0, area = 0;

	//通过黑色定位角作为父轮廓，有两个子轮廓的特点，筛选出三个定位角
	int parentIdx = -1;
	for (int i = 0; i < contours.size(); i++)
	{
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
		if (j == 2)
		{
			//保存定位角
			contours2.push_back(contours[parentIdx]);
			//画出定轮廓
			drawContours(s1, contours, parentIdx, CV_RGB(0, 255, 0), 1, 8);
			j = 0;
			parentIdx = -1;
		}
	}
	if (contours2.size() == 3 || contours2.size() == 4)
	{
		//画出定位角轮廓
		for (int i = 0; i < contours2.size(); i++)
			drawContours(s2, contours2, i, CV_RGB(255, 255, 0), FILLED, 4, hierarchy[k][2], 0, Point());
		//三个定位角的中心坐标
		Point point[4];
		for (int i = 0; i < contours2.size(); i++)
		{
			point[i] = Center_cal(contours2, i);
		}
		//轮廓面积,定位角的面积，计算边长
		area = contourArea(contours2[1]);
		int area_side = cvRound(sqrt(double(area)));
		for (int i = 0; i < contours2.size(); i++)
		{
			//画出三个定位角的中心连线
			line(s2, point[i % contours2.size()], point[(i + 1) % contours2.size()], Scalar(105, 75, 155), area_side, 8);
		}

		output = drawrect(s2, target);
		contours2.clear();
		return output;
	}
	else
	{
		output = target;
		return output;
	}
}



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

Mat drawrect(Mat input, Mat output)
{
	//框出这整个二维码
	Mat gray_all, threshold_output_all;
	vector<vector<Point> > contours_all;
	vector<Vec4i> hierarchy_all;
	cvtColor(input, gray_all, COLOR_BGR2GRAY);

	threshold(gray_all, threshold_output_all, 45, 255, THRESH_BINARY);
	findContours(threshold_output_all, contours_all, hierarchy_all, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));//RETR_EXTERNAL表示只寻找最外层轮廓

	Point2f fourPoint2f[4];
	//求最小包围矩形
	RotatedRect rectPoint = minAreaRect(contours_all[0]);
	rectPoint.points(fourPoint2f);
	for (int i = 0; i < 4; i++)
	{
		line(output, fourPoint2f[i % 4], fourPoint2f[(i + 1) % 4], Scalar(0, 0, 255), 3);
	}
	return output;
}