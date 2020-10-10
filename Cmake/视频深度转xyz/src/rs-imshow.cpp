#include <iostream>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>

#include <iostream>
#include <thread>

#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/console/parse.h>

using namespace std::chrono_literals;

// // #include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
// // #include <pcl/io/pcd_io.h>
// // #include <pcl/common/centroid.h>
// // #include <pcl/segmentation/sac_segmentation.h>
// // #include <pcl/visualization/pcl_visualizer.h>
// //     #include <iostream>
// //     #include <pcl/PolygonMesh.h>
// //     #include <pcl/point_types.h>
// //     #include <pcl/io/ply_io.h>
// //     #include <pcl/visualization/pcl_visualizer.h>
// //     #include <pcl/visualization/cloud_viewer.h>//点云查看窗口头文件
// //     #include <pcl/segmentation/sac_segmentation.h>
// //     #include <pcl/ModelCoefficients.h>
// //     #include <pcl/filters/extract_indices.h>
// //     #include <pcl/sample_consensus/method_types.h>   //随机参数估计方法头文件
// //     #include <pcl/sample_consensus/model_types.h>   //模型定义头文件
// //     #include <pcl/segmentation/sac_segmentation.h>   //基于采样一致性分割的类的头文件
// //     #include <pcl/sample_consensus/method_types.h>
// //     #include <pcl/sample_consensus/model_types.h>
// //     #include <pcl/segmentation/sac_segmentation.h>
// //     #include <pcl/visualization/cloud_viewer.h>
// //     #include <pcl/filters/extract_indices.h>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
using namespace std;
// 定义点云类型
    
    typedef pcl::PointXYZ PointT;
    typedef pcl::PointCloud<PointT> PointCloud;
     
    // 相机内参
    const double camera_factor = 100;
    const double camera_cx = 431.304;
    const double camera_cy = 229.998;
    const double camera_fx = 431.182;
    const double camera_fy = 431.182;
    // 主函数 
    // int user_data;



int main() 
{
    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map;

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    // Start streaming with default recommended configuration
    pipe.start();

    using namespace cv;

   // PointCloud::Ptr cloud(new PointCloud)
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_p (new pcl::PointCloud<pcl::PointXYZ>);

        rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
        rs2::frame depths = data.get_depth_frame().apply_filter(color_map);

        // Query frame size (width and height)
        const int w = depths.as<rs2::video_frame>().get_width();
        const int h = depths.as<rs2::video_frame>().get_height();

        // Create OpenCV matrix of size (w,h) from the colorized depth data
        Mat image(Size(w, h), CV_8UC3, (void*)depths.get_data(), Mat::AUTO_STEP);
        cout<<w<<" "<<h<<endl;
        // Update the window with new data

        cv::Mat depth = image.clone();
     
        // 遍历深度图
        for (int m = 0; m < depth.rows; m++)
        {
            for (int n = 0; n < depth.cols; n++)
            {
            // 获取深度图中(m,n)处的值
            ushort d = depth.ptr<ushort>(m)[n];
            // d 可能没有值，若如此，跳过此点
            if (d == 0)
            {
                continue;
            }
            // d 存在值，则向点云增加一个点
            PointT p;
     
            // 计算这个点的空间坐标
            p.z = double(d) / camera_factor;
            p.x = (n - camera_cx) * p.z / camera_fx;
            p.y = (m - camera_cy) * p.z / camera_fy;
     
     
            // 把p加入到点云中
            cloud->points.push_back(p);
            //cout << cloud->points.size() << endl;
            }
        }

        cloud->height = 1;
        cloud->width = cloud->points.size();
        cloud->is_dense = false;
       

    
pcl::visualization::PCLVisualizer::Ptr viewer (new pcl::visualization::PCLVisualizer ("sample cloud"));
viewer->setBackgroundColor (0, 0, 0);
viewer->addPointCloud<pcl::PointXYZ> (cloud, "sample cloud");
pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color(cloud, 0, 255, 0);
viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud");
viewer->addCoordinateSystem(2);
viewer->initCameraParameters ();


  while (!viewer->wasStopped ())
  {
    viewer->spinOnce (100);

        rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
        rs2::frame depths = data.get_depth_frame().apply_filter(color_map);

        // Query frame size (width and height)
        const int w = depths.as<rs2::video_frame>().get_width();
        const int h = depths.as<rs2::video_frame>().get_height();

        // Create OpenCV matrix of size (w,h) from the colorized depth data
        Mat depth(Size(w, h), CV_8UC3, (void*)depths.get_data(), Mat::AUTO_STEP);
        // Update the window with new data

   
        // 遍历深度图
        for (int m = 0; m < depth.rows; m++)
        {
            for (int n = 0; n < depth.cols; n++)
            {
            // 获取深度图中(m,n)处的值
            ushort d = depth.ptr<ushort>(m)[n];
            // d 可能没有值，若如此，跳过此点
            if (d == 0||d>500)
            {
                continue;
            }
            // d 存在值，则向点云增加一个点
            PointT p;
     
            // 计算这个点的空间坐标
            p.z = double(d) / camera_factor;
            p.x = (n - camera_cx) * p.z / camera_fx;
            p.y = (m - camera_cy) * p.z / camera_fy;    
     
            // 把p加入到点云中
            cloud->points.push_back(p);
            //cout << cloud->points.size() << endl;
            }
        }



    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);

    pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
    pcl::PointIndices::Ptr inliers (new pcl::PointIndices);
    // Create the segmentation object
    pcl::SACSegmentation<pcl::PointXYZ> seg;
    // Optional
    seg.setOptimizeCoefficients (true);
    // Mandatory
    seg.setModelType (pcl::SACMODEL_PLANE);
    seg.setMethodType (pcl::SAC_RANSAC);
    seg.setDistanceThreshold (thedistanceThre);

    seg.setInputCloud (cloud);
    seg.segment (*inliers, *coefficients); 

    if (inliers->indices.size() == 0)
    {
        PCL_ERROR("Could not estimate a planar model for the given dataset.");
        return (-1);
    }

     std::cerr << "Model coefficients: " << coefficients->values[0] << " "
                                         << coefficients->values[1] << " "
                                         << coefficients->values[2] << " "
                                         << coefficients->values[3] << std::endl;

    // 提取地面
    pcl::ExtractIndices<pcl::PointXYZ> extract;
    extract.setInputCloud (cloud);
    extract.setIndices (inliers);
    extract.filter (*cloud_filtered);
    // 提取除地面外的物体
    extract.setNegative (false);
    extract.filter (*cloud_filtered);


    viewer->updatePointCloud<pcl::PointXYZ> (cloud_filtered, "sample cloud");
    cloud->points.clear();


    std::this_thread::sleep_for(100ms);
  }

    return EXIT_SUCCESS;
}


