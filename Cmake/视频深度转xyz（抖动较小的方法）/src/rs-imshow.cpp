#include <iostream>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
using namespace std;
using namespace std::chrono_literals;
// 定义点云类型
    
    typedef pcl::PointXYZ PointT;
    typedef pcl::PointCloud<PointT> PointCloud;
     
    // 相机内参
    const double camera_factor = 1;
    const double camera_cx = 649.402466;
    const double camera_cy = 649.402466;
    const double camera_fx = 640.685730;
    const double camera_fy = 359.206085;
    // 主函数
    // int user_data;
    /*

void viewerPsycho (pcl::visualization::PCLVisualizer& viewer)
{
    static unsigned count = 0;
    std::stringstream ss;
    ss << "Once per viewer loop: " << count++;
    viewer.removeShape ("text", 0);
    viewer.addText (ss.str(), 200, 300, "text", 0);
 
    //FIXME: possible race condition here:
    user_data++;
}*/

int user_data;
    
void 
viewerOneOff (pcl::visualization::PCLVisualizer& viewer)
{
    viewer.setBackgroundColor (1.0, 0.5, 1.0);
    pcl::PointXYZ o;
    o.x = 1.0;
    o.y = 0;
    o.z = 0;
    viewer.addSphere (o, 0.25, "sphere", 0);
    std::cout << "i only run once" << std::endl;
    
}
    
void 
viewerPsycho (pcl::visualization::PCLVisualizer& viewer)
{
    static unsigned count = 0;
    std::stringstream ss;
    ss << "Once per viewer loop: " << count++;
    viewer.removeShape ("text", 0);
    viewer.addText (ss.str(), 200, 300, "text", 0);
    
    //FIXME: possible race condition here:
    user_data++;
}


int main(int argc, char * argv[]) try
{
    void viewerPsycho (pcl::visualization::PCLVisualizer& viewer);
    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map;

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    // Start streaming with default recommended configuration
    pipe.start();

    using namespace cv;

    // PointCloud::Ptr cloud(new PointCloud)
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    // pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_p (new pcl::PointCloud<pcl::PointXYZ>);
    //pcl::visualization::CloudViewer viewer("pcd viewer");
    //viewer.showCloud(cloud);
    const auto window_name = "Display Image";
    namedWindow(window_name, WINDOW_AUTOSIZE);

    //while (waitKey(1) < 0 && getWindowProperty(window_name, WND_PROP_AUTOSIZE) >= 0)
    
    pcl::visualization::PCLVisualizer::Ptr viewer (new pcl::visualization::PCLVisualizer ("sample cloud"));
    viewer->setBackgroundColor (0, 0, 0);
    viewer->addPointCloud<pcl::PointXYZ> (cloud, "sample cloud");
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color(cloud, 0, 255, 0);
    viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud");
    viewer->addCoordinateSystem (200);
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
        Mat image(Size(w, h), CV_8UC3, (void*)depths.get_data(), Mat::AUTO_STEP);

        // Update the window with new data
        //imshow(window_name, image);
        cv::Mat depth = image; 
        // 遍历深度图
        for (int m = 0; m < depth.rows; m++)
            for (int n = 0; n < depth.cols; n++)
            {
            // 获取深度图中(m,n)处的值
            ushort d = depth.ptr<ushort>(m)[n];
            // d 可能没有值，若如此，跳过此点
            if (d == 0)
                continue;
            // d 存在值，则向点云增加一个点
            PointT p;
     
            // 计算这个点的空间坐标
            p.z = double(d) / camera_factor;
            p.x = (n - camera_cx) * p.z / camera_fx;
            p.y = (m - camera_cy) * p.z / camera_fy;
        
            // 从rgb图像中获取它的颜色
            // rgb是三通道的BGR格式图，所以按下面的顺序获取颜色
            // p.b = rgb.ptr<uchar>(m)[n * 3];
            // p.g = rgb.ptr<uchar>(m)[n * 3 + 1];
            // p.r = rgb.ptr<uchar>(m)[n * 3 + 2];     
            // 把p加入到点云中
            cloud->points.push_back(p);
            //cout << cloud->points.size() << endl;
            }
        
        // 设置并保存点云
        cloud->height = 1;
        cloud->width = cloud->points.size();
        cout << "point cloud size = " << cloud->points.size() << endl;
        // cloud->is_dense = false;
    // viewer.runOnVisualizationThread (viewerPsycho);
    //保存点云图
    // pcl::io::savePCDFile("/Home/code/cmake/depth/bin/pcd.pcd", *cloud);
    // pcl::visualization::CloudViewer viewer ("Simple Cloud Viewer");
    // viewer.showCloud (cloud);
    // pcl::visualization::CloudViewer viewer("Cloud Viewer");   
    // //blocks until the cloud is actually rendered
    // viewer.showCloud(cloud);   
    // //use the following functions to get access to the underlying more advanced/powerful
    // //PCLVisualizer
    // //This will only get called once
    // viewer.runOnVisualizationThreadOnce (viewerOneOff);
    // //This will get called once per visualization iteration
    // viewer.runOnVisualizationThread (viewerPsycho);
    // while (!viewer.wasStopped ())
    // {
    // //you can also do cool processing here
    // //FIXME: Note that this is running in a separate thread from viewerPsycho
    // //and you should guard against race conditions yourself...
    // user_data++;
    // }
    //  while(!viewer.wasStopped()){}
    // while(1);
    // system("pause");               
        
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_f(new pcl::PointCloud<pcl::PointXYZ>);

        pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
        pcl::PointIndices::Ptr inliers (new pcl::PointIndices);
        // Create the segmentation object
        pcl::SACSegmentation<pcl::PointXYZ> seg;
        // Optional 
        seg.setOptimizeCoefficients (true);
        // Mandatory
        seg.setModelType (pcl::SACMODEL_PLANE);
        seg.setMethodType (pcl::SAC_RANSAC);
        seg.setDistanceThreshold (10);

        seg.setInputCloud (cloud);
        seg.segment (*inliers, *coefficients);

        // if (inliers->indices.size () == 0)
        // {
        //     PCL_ERROR ("Could not estimate a planar model for the given dataset.");
        //     return (-1);
        // }

        // Extract the planar inliers from the input cloud
        
        pcl::ExtractIndices<pcl::PointXYZ> extract;
        extract.setInputCloud (cloud);
        extract.setIndices (inliers);
        extract.setNegative (false);
        extract.filter (*cloud_f);
        
        
//   pcl::visualization::PCLVisualizer::Ptr viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));
//   viewer->setBackgroundColor (0, 0, 0);
//   pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> rgb(cloud);
//   viewer->updatePointCloud<pcl::PointXYZRGB> (cloud_f, rgb, "sample cloud");
//   viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud");
//   viewer->addCoordinateSystem (1.0);

//   viewer->initCameraParameters ();

//         while (!viewer->wasStopped ())
//   {
//     viewer->spinOnce (100);
//     pcl::visualization::PCLVisualizer::Ptr viewer (new pcl::visualization::PCLVisualizer ("sample cloud"));
//     viewer->setBackgroundColor (0, 0, 0);
//     pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color(cloud, 0, 255, 0); 
//     viewer-> updatePointCloud<pcl::PointXYZ> (cloud_f, single_color, "sample cloud");
//     std::this_thread::sleep_for(100ms);
//   }
       
        viewer-> updatePointCloud<pcl::PointXYZ> (cloud_f, single_color, "sample cloud");


        // Kcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud");

        // pcl::visualization::CloudViewer viewer ("Simple Cloud Viewer");
        // viewer.showCloud (cloud_f);
        //*cloud_filtered = *cloud_f;
        std::cerr << "Model coefficients: " << coefficients->values[0] << " " 
                                            << coefficients->values[1] << " "
                                            << coefficients->values[2] << " " 
                                            << coefficients->values[3] << std::endl;

        std::cerr << "Model inliers: " << inliers->indices.size () << std::endl;
        
        // while(!viewer->wasStopped()){}

        //while(1);
        // system("pause");
               
 
    std::this_thread::sleep_for(100ms);
    cloud->points.clear();
    cloud_f->points.clear();
    }
       //  while(!viewer.wasStopped()){}
        //while(1);
       // system("pause");
    return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
    std::cerr << "nononono" << std::endl;
    return EXIT_FAILURE;
}


   /*
    int main(int argc, char** argv)
    {
        // 读取./data/rgb.png和./data/depth.png，并转化为点云
     
        // 图像矩阵
       // cv::Mat depth;
        // 使用cv::imread()来读取图像
        // API: http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html?highlight=imread#cv2.imread
        // depth 是16UC1的单通道图像，注意flags设置-1,表示读取原始数据不做任何修改
        //depth = cv::imread("/home/xiang/picture/D1_Depth.png");
        cout << "read depth" << endl;
        // 点云变量
        // 使用智能指针，创建一个空点云。这种指针用完会自动释放。
        
            // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map;

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    // Start streaming with default recommended configuration
    pipe.start();



        PointCloud::Ptr cloud(new PointCloud);
        // 遍历深度图
        for (int m = 0; m < depth.rows; m++)
            for (int n = 0; n < depth.cols; n++)
            {
            // 获取深度图中(m,n)处的值
            ushort d = depth.ptr<ushort>(m)[n];
            // d 可能没有值，若如此，跳过此点
            if (d == 0)
                continue;
            // d 存在值，则向点云增加一个点
            PointT p;
     
            // 计算这个点的空间坐标
            p.z = double(d) / camera_factor;
            p.x = (n - camera_cx) * p.z / camera_fx;
            p.y = (m - camera_cy) * p.z / camera_fy;
     
            // 从rgb图像中获取它的颜色
            // rgb是三通道的BGR格式图，所以按下面的顺序获取颜色
            p.b = rgb.ptr<uchar>(m)[n * 3];
            p.g = rgb.ptr<uchar>(m)[n * 3 + 1];
            p.r = rgb.ptr<uchar>(m)[n * 3 + 2];
     
            // 把p加入到点云中
            cloud->points.push_back(p);
            //cout << cloud->points.size() << endl;
            }
        // 设置并保存点云
        cloud->height = 1;
        cloud->width = cloud->points.size();
        cout << "point cloud size = " << cloud->points.size() << endl;
        cloud->is_dense = false;
        try{
            //保存点云图
            pcl::io::savePCDFile("/home/xiang/picture/pcd.pcd", *cloud);
     
     
        }
        catch (pcl::IOException &e){
            cout << e.what()<< endl;
        }
        //显示点云图
        pcl::visualization::CloudViewer viewer("Simple Cloud Viewer");//直接创造一个显示窗口
        viewer.showCloud(cloud);//再这个窗口显示点云
        while (!viewer.wasStopped())
        {
        }
     
        //pcl::io::savePCDFileASCII("E:\\Visual Studio2013\\projectpointcloud.pcd", *cloud);
        // 清除数据并退出
        cloud->points.clear();
        cout << "Point cloud saved." << endl;
        return 0;
    }
    &*/
   /*
  pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
  pcl::PointIndices::Ptr inliers (new pcl::PointIndices);
  // Create the segmentation object
  pcl::SACSegmentation<pcl::PointXYZ> seg;
  // Optional
  seg.setOptimizeCoefficients (true);
  // Mandatory
  seg.setModelType (pcl::SACMODEL_PLANE);
  seg.setMethodType (pcl::SAC_RANSAC);
  seg.setDistanceThreshold (0.01);

  seg.setInputCloud (cloud_o);
  seg.segment (*inliers, *coefficients);


  pcl::ExtractIndices<pcl::PointXYZ> extract;
  if (inliers->indices.size () == 0)
  {
    PCL_ERROR ("Could not estimate a planar model for the given dataset.");
    return (-1);
  }
    extract.setInputCloud (cloud_o);
    extract.setIndices (inliers);
    extract.setNegative (false);
    extract.filter (*cloud_p);
    pcl::visualization::CloudViewer viewer("pcd viewer");
    viewer.showCloud(cloud_p);
    std::cout << "Model coefficients: " << coefficients->values[0] << " " 
                                      << coefficients->values[1] << " "
                                      << coefficients->values[2] << " " 
                                      << coefficients->values[3] << std::endl;

  std::cerr << "Model inliers: " << inliers->indices.size () << std::endl;


*/