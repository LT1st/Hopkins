//#include "stdafx.h"
#include <iostream>
#include <pcl/point_types.h>
#include <pcl/filters/passthrough.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/io/pcd_io.h> 
#include <pcl/io/ply_io.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/filters/bilateral.h>
#include <pcl/filters/fast_bilateral.h>
#include <pcl/filters/fast_bilateral_omp.h>
#include <pcl/filters/radius_outlier_removal.h>

using namespace std;
int main()
{
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);

    if (pcl::io::loadPLYFile<pcl::PointXYZ>("realsensPic2.ply", *cloud) == -1) //* load the file 
    {
        PCL_ERROR("Couldn't read file realsensPic2 \n");
        system("PAUSE");
        return (-1);
    }

    pcl::PassThrough<pcl::PointXYZ> pass;
    pass.setInputCloud(cloud);
    pass.setFilterFieldName("z");
    pass.setFilterLimits(0.0, 2.4); //设置Z轴范围为0.0到2.4m
    pass.filter(*cloud_filtered);
    //显示滤波前的点云 
    pcl::visualization::PCLVisualizer viewer("passthrough viewer");
    pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZ> fildColor(cloud_filtered, "z");//按照z字段进行渲染
    viewer.addPointCloud(cloud_filtered, fildColor, "origin cloud");
    viewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "origin clouds");
    
    while (!viewer.wasStopped()) {
        viewer.spinOnce();
    }
    return (0);
}