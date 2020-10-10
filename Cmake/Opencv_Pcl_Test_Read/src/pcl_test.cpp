#include <iostream> //标准输入输出流
#include <pcl/io/ply_io.h> //PCL的PCD格式文件的输入输出头文件
#include <pcl/io/pcd_io.h> //PCL的PCD格式文件的输入输出头文件
#include <pcl/ModelCoefficients.h>
#include <pcl/point_types.h> //PCL对各种格式的点的支持头文件
#include <pcl/visualization/cloud_viewer.h>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API

 
int main(int argc, char** argv) try
{
		// Create a Pipeline - this serves as a top-level API for streaming and processing frames
	rs2::pipeline p;

	// Configure and start the pipeline
	p.start();

	 while (true)
    {
        // Block program until frames arrive
		//RealSense摄像机通常提供多个视频，运动或姿势流。
		//wait_for_frames该功能将阻塞，直到来自各种已配置流的下一组相干帧为止。
        rs2::frameset frames = p.wait_for_frames();

        // Try to get a frame of a depth image
        rs2::depth_frame depth = frames.get_depth_frame();

        // Get the depth frame's dimensions
        float width = depth.get_width();
        float height = depth.get_height();

        // Query the distance from the camera to the object in the center of the image
        float dist_to_center = depth.get_distance(width / 2, height / 2);

        // Print the distance
        std::cout << "The camera is facing an object " << dist_to_center << " meters away \r";
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