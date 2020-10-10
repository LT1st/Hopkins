/* #include <cv.h> */
/* #include <highgui.h> */
#include <vector>

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
 
 using namespace std;
#define MINAREA 600

int main()
{
    IplImage* srcRGB =  cvLoadImage("33333.bmp");
	IplImage* src= cvCreateImage(cvGetSize(srcRGB), 8, 1);	
 
	cvCvtColor(srcRGB, src,CV_RGB2GRAY);
	cvNamedWindow("srcRGB", 0);
	if(srcRGB!=NULL)
	{
		cvShowImage("srcRGB",srcRGB);
	}
	
	/**********************************转HSV分离通道**********************************/
	IplImage* hsv = cvCreateImage(cvGetSize(srcRGB), 8, 3);
	cvCvtColor( srcRGB, hsv, CV_BGR2HSV );
 
	IplImage* img_h = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* img_s = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* img_v = cvCreateImage(cvGetSize(src), 8, 1);
 
	cvSplit(hsv, img_h, img_s, img_v, NULL);
	
	cvNamedWindow("img_h", 0);
	cvShowImage("img_h", img_h);
	cvNamedWindow("img_s", 0);
	cvShowImage("img_s", img_s);
	cvNamedWindow("img_v", 0);
	cvShowImage("img_v", img_v);
 
	cvNamedWindow("Thresh", 0);
	
	IplImage* Thresh_img2 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* Thresh_img1 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* Thresh_img = cvCreateImage(cvGetSize(src), 8, 1);
 
	cvThreshold(img_s, Thresh_img1, 90, 255, CV_THRESH_BINARY);
	cvErode( Thresh_img1, Thresh_img1, NULL, 2 );
	cvDilate(Thresh_img1, Thresh_img1, NULL, 2);
 
	cvNamedWindow("img_ED", 0);
	cvShowImage("img_ED", Thresh_img1);
 
	cvShowImage("Thresh", Thresh_img1);
	cvWaitKey(0);
	cvThreshold(img_v, Thresh_img2, 248, 255, CV_THRESH_BINARY);
	cvAdd(Thresh_img1,Thresh_img2, Thresh_img);
	
	cvNamedWindow("img_ADD", 0);
	cvShowImage("img_ADD", Thresh_img);
 
	/***********************************提取目标*************************************/
	IplImage* temp = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* dst = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* dstRGB = cvCreateImage(cvGetSize(src), 8, 3);
	cvZero(dstRGB);
	cvZero(temp);
	cvZero(dst);
 
	CvMemStorage* storage = cvCreateMemStorage(0);  
    CvSeq* contour = 0;
	
	CvPoint center[20] = {cvPoint(0,0)};
	
    int contour_num = cvFindContours(Thresh_img1, storage, &contour, sizeof(CvContour), 
	   CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); 
	
	for(int i = 0; contour != 0; contour = contour->h_next, i++ )    
    {  
        double length = cvArcLength(contour);  
        if(length < MINAREA)     
        {
           cvSeqRemove(contour, 0);           // 删除面积小于设定值的轮廓  
           continue;
        }  
		int count = 0;
		if(length >= MINAREA)
		{
		 count++;
         cvZero(dst);
         cvZero(temp);
	     cvDrawContours(temp, contour, cvScalar(255,255,255), cvScalar(255,255,255), -1,CV_FILLED); //作为掩码		                     
		 cvCopy(img_h, dst, temp);
		// cvSeqRemove(contour->h_prev, 0); 
		
		 ///目标区域hue通道的平均灰度值
		 int pix_count=0;
		 int  gray_value = 0;
		 for(int x=0; x<dst->height; x++)
		 {
			 uchar* ptr = (uchar*) dst->imageData+x*dst->widthStep;
			 for(int y=0; y<dst->width; y++)
			 {				
				if(ptr[y])
					{
						gray_value += ptr[y];
						pix_count++;
				    }
			 }
		 }
		 gray_value/=pix_count;
 
         /找目标中心//
		
		 CvSeqReader reader;
		 CvPoint pt = cvPoint(0,0);
		 cvStartReadSeq(contour, &reader);
		 
		 int point_count = 0;
 
		 for(int i=0; i<contour->total; i++)
			{				
				CV_READ_SEQ_ELEM(pt, reader);
				center[count].x+=pt.x;
				center[count].y+=pt.y;
				point_count++;
		    }
		
		 center[count].x=center[count].x/point_count;     //中心坐标
		 center[count].y=center[count].y/point_count;
		 cout<<"X:"<<center[count].x << " Y:" << center[count].y<<endl;
	    		 
		 /
		 cvCopy(srcRGB, dstRGB, temp);		
		// char* Gray_Value = "hello";
		// itoa(gray_value, Gray_Value , 20);
		// sprintf(Gray_Value,"%d",gray_value);
		// cout<<Gray_Value<<endl;
		// CvFont font;
	    // cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.8f, 0.8f, 1, 1);
		// cvPutText(dstRGB,  Gray_Value, center,	&font, CV_RGB(255,255,255) );
		 cvCircle(dstRGB, center[count], 6, CV_RGB(0,255,0), 2);
		 cout<<gray_value<<endl;
		 cvNamedWindow("dstRGB", 0);
		 cvShowImage("dstRGB", dstRGB);	    
		}
		//cvWaitKey(0);
	} 
	cvWaitKey(0);   
			
	cvReleaseImage(&img_h);
	cvReleaseImage(&img_s);
	cvReleaseImage(&img_v);	
	cvReleaseImage(&hsv);
	cvReleaseImage(&src);
	cvDestroyAllWindows();
} /* 
//----------------------------------------------
            Camera.Size size = mCamera.getParameters().getPreviewSize();
            final int w = size.width;
            final int h = size.height;
            final YuvImage image = new YuvImage(mData, ImageFormat.NV21, w, h, null);
            ByteArrayOutputStream os = new ByteArrayOutputStream(mData.length);
            if (!image.compressToJpeg(new Rect(0, 0, w, h), 100, os)) {
                return;
            }
            byte[] tmp = os.toByteArray();
            Bitmap bitmap = BitmapFactory.decodeByteArray(tmp, 0, tmp.length);
            Bitmap input = rotateBitmap(bitmap, 90);
            Bitmap bmp = input.copy(Bitmap.Config.ARGB_8888, true);
 
            int nw = bmp.getWidth();
            int nh = bmp.getHeight();
            int[] pix = new int[nw * nh];
            bmp.getPixels(pix, 0, nw, 0, 0, nw, nh);
            int[] resultPixels = OpenCVHelper.colorDetection(pix, nw, nh);
            Bitmap result = Bitmap.createBitmap(nw, nh, Bitmap.Config.RGB_565);
            result.setPixels(resultPixels,0,nw,0,0,nw,h);
            image_show.setImageBitmap(result);

            JNIEXPORT jintArray JNICALL Java_com_example_targetlocationapp_OpenCVHelper_colorDetection
            (JNIEnv *env, jclass obj, jintArray  buf, jint w, jint h){
                jint *cbuf;
                cbuf = env->GetIntArrayElements(buf,JNI_FALSE);
                if (NULL == cbuf)
                {
                    return 0;
                }
            
                Mat srcImage(h,w,CV_8UC4,(unsigned char*) cbuf);
                Mat rgbImage;
                cvtColor(srcImage, rgbImage, COLOR_BGRA2RGB);
            
                for (size_t i = 0; i < 5; i++)
                {
                    Point center(100 + 50*i, 150 + 30*i);
                    ellipse(rgbImage, center, Size(100, 150), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
                }
            
                int size = w * h;
                jintArray result = env->NewIntArray(size);
                //jint *pArrayBuf = env->GetIntArrayElements(result, nullptr);
                u_char *ptr = rgbImage.ptr(0);
                u_char *ptrRst = srcImage.ptr(0);
                for(int i=0;i < size; i++){
                    // int grayScale = (int)(ptr[3*i+2]*0.299 + ptr[3*i+1]*0.587 + ptr[3*i+0]*0.144 );
                    // pArrayBuf[i] = (int)ptr[3*i+2];
                    ptrRst[4*i+0] = (int)ptr[3*i+2];
                    ptrRst[4*i+1] = (int)ptr[3*i+1];
                    ptrRst[4*i+2] = (int)ptr[3*i+0];
                }
                env->SetIntArrayRegion(result,0,size,cbuf);
                env->ReleaseIntArrayElements(buf,cbuf,0);
                return result;
            } */