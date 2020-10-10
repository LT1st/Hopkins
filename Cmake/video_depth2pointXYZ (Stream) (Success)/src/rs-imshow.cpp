
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <opencv2/opencv.hpp>   // Include OpenCV API
#include <iostream>
#include <stdio.h>
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
#include <boost/thread/thread.hpp>
#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>

using namespace std;
using namespace cv;
// 定义点云类型
typedef pcl::PointXYZ PointT;
typedef pcl::PointCloud<PointT> PointCloud;


int main(int argc, char * argv[]) try
{
    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map;
    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    // Start streaming with default recommended configuration
    pipe.start();

    int hasbeenadded = 0;//已经首次写入标志位
  
  // 用mat先施图片
    const auto window_name = "Display Image";
    namedWindow(window_name, WINDOW_AUTOSIZE);

    // 相机内参

    const double camera_factor = 1;
    const double camera_cx = 422.886;
    const double camera_cy = 241.742;
    const double camera_fx = 422.886;
    const double camera_fy = 423.801;
   
   

    PointCloud::Ptr cloud(new PointCloud);

    //创建视窗对象并给标题栏设置一个名称“3D Viewer”并将它设置为boost::shared_ptr智能共享指针，
    //这样可以保证指针在程序中全局使用，而不引起内存错误
    boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer2 (new pcl::visualization::PCLVisualizer ("sample cloud"));
    //设置视窗的背景色，可以任意设置RGB的颜色，这里是设置为黑色
    viewer2->setBackgroundColor (0, 0, 0);

       //  viewer2->addPointCloud<pcl::PointXYZ> (cloud, "sample cloud"); 
        // while()
    /*这是最重要的一行，我们将点云添加到视窗对象中，并定一个唯一的字符串作为ID 号，利用此字符串保证在其他成员中也能
    标志引用该点云，多次调用addPointCloud可以实现多个点云的添加，，每调用一次就会创建一个新的ID号，如果想更新一个
    已经显示的点云，必须先调用removePointCloud（），并提供需要更新的点云ID 号*/
    if(hasbeenadded == 0)
    {
        hasbeenadded = 1;
        viewer2->addPointCloud<pcl::PointXYZ> (cloud, "sample cloud"); 
    }
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color(cloud, 255, 255, 0);
    //set point propoties
    viewer2->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud");
    //圆柱体规模
    viewer2->addCoordinateSystem (100);
    //通过设置照相机参数使得从默认的角度和方向观察点云
    viewer2->initCameraParameters();
    //viewer2.showCloud(cloud);//再这个窗口显示点云

   
    while (!viewer2->wasStopped())
    {

        viewer2->spinOnce (100);

        //INIT
        rs2::frameset data = pipe.wait_for_frames(15000); // Wait for next set of frames from the camera
        rs2::frame depth = data.get_depth_frame().apply_filter(color_map);

        // Query frame size (width and height)
        const int w = depth.as<rs2::video_frame>().get_width();
        const int h = depth.as<rs2::video_frame>().get_height();

        // Create OpenCV matrix of size (w,h) from the colorized depth data
        Mat image(Size(w, h), CV_8UC3, (void*)depth.get_data(), Mat::AUTO_STEP);
        Mat depthe = image;

        
         // 遍历深度图
        for (int m = 0; m < depthe.rows; m++)
        {
            for (int n = 0; n < depthe.cols; n++)
                {
                // 获取深度图中(m,n)处的值
                ushort d = depthe.ptr<ushort>(m)[n];
                //d可能没有值，若如此，跳过此点
                //在深度图像的极限位置，因缺失数据，在生成的点云中最远处会有一个大平面
                //会导致使别最大平面出错
                if ((d == 0)||(d>500) )//|| (d <500)
                    continue;
                // d 存在值，则向点云增加一个点
                PointT p;

                // 计算这个点的空间坐标
                p.z = double(d) / camera_factor;
                p.x = (n - camera_cx) * p.z / camera_fx;
                p.y = (m - camera_cy) * p.z / camera_fy;
        
        /*       // 从rgb图像中获取它的颜色
                // rgb是三通道的BGR格式图，所以按下面的顺序获取颜色
                p.b = rgb.ptr<uchar>(m)[n * 3];
                p.g = rgb.ptr<uchar>(m)[n * 3 + 1];
                p.r = rgb.ptr<uchar>(m)[n * 3 + 2];
        
               
        */
                 // 把p加入到点云中
                cloud->points.push_back(p);
                //cout << cloud->points.size() << endl;
                
               }
        }

        cloud->height = 1;
        cloud->width = cloud->points.size();
        cout << "point cloud size = " << cloud->points.size() << endl;
        cloud->is_dense = false;

        //显示点云图X（红色）Y（绿色 ）Z （蓝色）
  //      pcl::visualization::CloudViewer viewer("Simple Cloud Viewer");//直接创造一个显示窗口
 //       viewer.showCloud(cloud);//再这个窗口显示点云

       

        //pcl::visualization::PCLVisualizer::addPointCloud 
        //updatePointCloud
        
        /*
        pcl::visualization::PCLVisualizer::Ptr viewer (new pcl::visualization::PCLVisualizer ("sample cloud"));
        viewer->setBackgroundColor (0, 0, 0);
        viewer->addPointCloud<pcl::PointXYZ> (cloud, "sample cloud");
        pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color(cloud, 255, 255, 0);
        viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud");
        viewer->addCoordinateSystem (200);
        viewer->initCameraParameters ();
        */

        
        // viewer2->removePointCloud<pcl::PointXYZ>("sample cloud",0);//只有声明，在hpp中没有写出
        if(hasbeenadded == 1)
        {
            viewer2->updatePointCloud<pcl::PointXYZ>(cloud, "sample cloud");
        //   while (!viewer.wasStopped()){}
        }

   
        // 清除数据并退出
        cloud->points.clear();
        // Update the window with new data
//           imshow(window_name, image) ;
        
       // std::this_thread::sleep_for(100ms);
        waitKey(100);
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



