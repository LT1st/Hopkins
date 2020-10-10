
// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.
/* 
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <opencv2/opencv.hpp>   // Include OpenCV API
#include <iostream>
#include <stdio.h>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h> */
#include <opencv2/opencv.hpp>
/* #include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pcl/visualization/cloud_viewer.h>//调用过程更加简单直接
#include <pcl/visualization/pcl_visualizer.h>//visuailization methord 最强大的是PCLvisualizer
#include <pcl/visualization/range_image_visualizer.h> //深度图可视化
#include <pcl/console/parse.h> //命令行参数解析
#include <boost/thread/thread.hpp>
#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h> */

using namespace std;
using namespace cv;
 
int main()
{
    VideoCapture capture;
    capture.open(0);//利用初始化的capture，打开ID为0的摄像头

    namedWindow("sss");
    Mat frame;   //定义Mat变量

while (1)
{
    capture >> frame;/* code */
    imshow("sss",frame);
    waitKey(30);

}
    waitKey(0);
    return 0;
}
