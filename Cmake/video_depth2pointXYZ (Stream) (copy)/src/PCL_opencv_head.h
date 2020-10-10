
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <opencv2/opencv.hpp>   // Include OpenCV API
#include <iostream>
#include <stdio.h>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pcl/visualization/cloud_viewer.h>//调用过程更加简单直接
#include <pcl/visualization/pcl_visualizer.h>//visuailization methord 最强大的是PCLvisualizer
#include <pcl/visualization/range_image_visualizer.h> //深度图可视化
#include <pcl/console/parse.h> //命令行参数解析
#include <boost/thread/thread.hpp>
#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <pcl/common/centroid.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <iostream>
#include <pcl/PolygonMesh.h>
#include <pcl/point_types.h>                     //点云（类型）
#include <pcl/visualization/cloud_viewer.h>      //点云查看窗口头文件
#include <pcl/ModelCoefficients.h>               //模型系数 Used when add or delet or device the PC
#include <pcl/filters/extract_indices.h>
#include <pcl/sample_consensus/method_types.h>   //随机参数估计方法  随机样本一致性算法	方法类型
#include <pcl/sample_consensus/model_types.h>    //模型定义头文件     随机样本一致性算法	模型类型
#include <pcl/segmentation/sac_segmentation.h>   //基于采样一致性分割的类的头文件
#include <pcl/PCLPointCloud2.h>
#include <pcl/conversions.h>
#include <pcl/common/transforms.h>               //Updated after the artical written

using namespace std;
using namespace cv;