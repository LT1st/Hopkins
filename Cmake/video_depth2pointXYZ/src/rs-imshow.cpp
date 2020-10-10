// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <opencv2/opencv.hpp>   // Include OpenCV API
#include <iostream>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pcl/visualization/cloud_viewer.h>//调用过程更加简单直接
#include <pcl/visualization/pcl_visualizer.h>//visuailization methord 最强大的是PCLvisualizer
#include <pcl/visualization/range_image_visualizer.h> //深度图可视化
#include <pcl/console/parse.h> //命令行参数解析
#include <pcl/io/real_sense_2_grabber.h>

using namespace std;
using namespace cv;
// 定义点云类型
typedef pcl::PointXYZ PointT;
typedef pcl::PointCloud<PointT> PointCloud;
typedef pcl::PointCloud<PointT> putput;

//int user_data;
//void viewerOneOff(pcl::visuailization::pcl_visualizer& viewer)
//{
//  viewer.setB
//}

int main(int argc, char * argv[]) try
{
    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map;

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    // Start streaming with default recommended configuration
    pipe.start();

  //  class Ptrrrrr pcl::PointCloud<PointT>Ptrrrrrrrrrr;
 //   class Test567 pcl::RealSense2Grabber pcl::PointCloud<pcl::PointXYZ>::Ptr; pcl::RealSense2Grabber::convertDepthToPointXYZ();
 //   Test567::convertDepthToPointXYZ();
    const auto window_name = "Display Image";
    namedWindow(window_name, WINDOW_AUTOSIZE);


    while (waitKey(1) < 0 && getWindowProperty(window_name, WND_PROP_AUTOSIZE) >= 0)
    {
        rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
        rs2::frame depth = data.get_depth_frame().apply_filter(color_map);

        //putput = pcl::RealSense2Grabber::convertDepthToPointXYZ();
                 pcl::RealSense2Grabber::convertRGBDepthToPointXYZRGB();
     // Query frame size (width and height)
        const int w = depth.as<rs2::video_frame>().get_width();
        const int h = depth.as<rs2::video_frame>().get_height();

        // Create OpenCV matrix of size (w,h) from the colorized depth data
        Mat image(Size(w, h), CV_8UC3, (void*)depth.get_data(), Mat::AUTO_STEP);
        Mat depthe = image;

        PointCloud::Ptr cloud(new PointCloud);
         // 遍历深度图

                 // 把p加入到点云中
                cloud->points.push_back(putput);
                cout << cloud->points.size() << endl;

        cloud->height = 1;
        cloud->width = cloud->points.size();
        cout << "point cloud size = " << cloud->points.size() << endl;
        cloud->is_dense = false;

        //显示点云图
        pcl::visualization::CloudViewer viewer("Simple Cloud Viewer");//直接创造一个显示窗口
        viewer.showCloud(cloud);//再这个窗口显示点云
        while (!viewer.wasStopped())
        {
        }
        // 清除数据并退出,avoid to accumulate
        cloud->points.clear();
        // Update the window with new data
//           imshow(window_name, image) ;

        waitKey(1);
    }

    return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}



