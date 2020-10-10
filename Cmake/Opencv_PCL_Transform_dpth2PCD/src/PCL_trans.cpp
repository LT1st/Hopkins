 
#include <iostream>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pcl/visualization/cloud_viewer.h>
using namespace std;
// 定义点云类型
typedef pcl::PointXYZRGBA PointT;
typedef pcl::PointCloud<PointT> PointCloud;
 
// 相机内参
const double camera_factor = 1;
const double camera_cx = 649.402466;
const double camera_cy = 649.402466;
const double camera_fx = 640.685730;
const double camera_fy = 359.206085;
// 主函数
 
int main(int argc, char** argv)
{
    /*https://blog.csdn.net/SFM2020/article/details/84591758?ops_request_m
    isc=%257B%2522request%255Fid%2522%253A%2522160139146919725271727772%2522%252C%2522scm%2522%253
    A%252220140713.130102334..%2522%257D&request_id=160139146919725271727772&biz_id=0&utm_medium=d
    istribute.pc_search_result.none-task-blog-2~all~first_rank_v2~rank_v28_p-1-84591758.pc_first_ran
    k_v2_rank_v28_p&utm_term=RGB%E5%9B%BE%E5%92%8CDEPTH%E5%9B%BE%E7%94%9F%E6%88%90%E7%82%B9%E4%BA%91%
    E5%9B%BE%E5%8E%9F%E7%90%86%E4%B8%8E%E4%BB%A3%E7%A0%81%E5%AE%9E%E7%8E%B0&spm=1018.2118.3001.4187*/
    // 读取./data/rgb.png和./data/depth.png，并转化为点云
    // 图像矩阵
    cv::Mat rgb, depth;
    // 使用cv::imread()来读取图像
    // API: http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html?highlight=imread#cv2.imread
 //   rgb = cv::imread("/Home/Picture/Selection_001.png");
 //   cout << "read rgb"<<endl;
    // rgb 图像是8UC3的彩色图像
    // depth 是16UC1的单通道图像，注意flags设置-1,表示读取原始数据不做任何修改
    depth = cv::imread("/home/steven/Pictures/Selection_0021_Depth.png");
    cout << "read depth" << endl;
    // 点云变量
    // 使用智能指针，创建一个空点云。这种指针用完会自动释放。
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
  //      p.b = rgb.ptr<uchar>(m)[n * 3];
  //      p.g = rgb.ptr<uchar>(m)[n * 3 + 1];
  //      p.r = rgb.ptr<uchar>(m)[n * 3 + 2];
 
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
        pcl::io::savePCDFile("/home/xiang/picture/pcdTest00001.pcd", *cloud);

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