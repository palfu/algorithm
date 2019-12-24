#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#include <sys/time.h>

using namespace cv;
using namespace std;

#define COLOR_NONE         "\033[0m"
#define COLOR_RED          "\033[0;32;31m"
#define COLOR_GREEN        "\033[0;32;32m"
#define PRINTF_INFO(...) 	\
{								\
	printf(COLOR_GREEN "[INFO]: "  __VA_ARGS__);	\
	CLEAR_COLOR;				\
}
#define PRINTF_ERROR(...) \
{								\
	printf(COLOR_RED "[ERROR]: " __VA_ARGS__);	\
	CLEAR_COLOR;				\
}

#define CLEAR_COLOR printf(COLOR_NONE)

static timeval g_start_time_, g_end_time_;

void start_tick()
{
    gettimeofday(&g_start_time_, NULL);
}


double end_tick()
{
    gettimeofday(&g_end_time_, NULL);
    double time_used = (1000000.0*(g_end_time_.tv_sec - g_start_time_.tv_sec) +g_end_time_.tv_usec -g_start_time_.tv_usec)/1000.0;
    return time_used;
}

void saveDebugImage(string filename, Mat& dst)
{
    Mat save = dst.clone();
    if(save.type()==CV_32F)
    {
        save = save*100;
        save.convertTo(save, CV_8U);
    }
    imwrite(filename, save);
}


void test_roi(cv::Mat& src, int dst_size, int times)
{
    PRINTF_INFO("src image %dx%d, dst image size %dx%d\n", src.cols, src.rows, dst_size, dst_size);
    Mat dst = Mat::zeros(dst_size, dst_size, CV_32F);
    int min_width = min(src.cols, dst.cols);
	int min_height = min(src.rows, dst.rows);

	cv::Mat src_roi = src(cv::Rect((src.cols-min_width)/2, (src.rows-min_height)/2, min_width, min_height));
	cv::Mat dst_roi = dst(cv::Rect((dst.cols-min_width)/2, (dst.rows-min_height)/2, min_width, min_height));

    start_tick();

    uchar* src_ptr = (uchar*)src_roi.data;    
    float* dst_ptr = (float*)dst_roi.data;
    for(int ti=0; ti<times; ti++)
    {
        for(int r=0; r<min_height; r++)
        {
            for(int c=0; c<min_width; c++)
                dst_ptr[r*min_width+c] = src_ptr[r*min_width+c]>30?1.0f:0.0f;
        }
    }
	
    double ms = end_tick();
    PRINTF_INFO("ptr times %d, average time %0.3fms\n", times, ms/times);
    saveDebugImage("ptr.jpg", dst);

    dst = Mat::zeros(dst_size, dst_size, CV_32F);

    start_tick();
    for(int ti=0; ti<times; ti++)
    {
        src_roi.convertTo(dst_roi, CV_32F);
        threshold(dst_roi, dst_roi, 30, 1.0, THRESH_BINARY);
    }
    ms = end_tick();
    PRINTF_INFO("threshold times %d, average time %0.3fms\n", times, ms/times);
    saveDebugImage("threshold.jpg", dst);


    dst = Mat::zeros(dst_size, dst_size, CV_32F);
    start_tick();

    for(int ti=0; ti<times; ti++)
    {
        for(int r=0; r<min_height; r++)
        {
            for(int c=0; c<min_width; c++)
                dst_roi.ptr(r)[c] = src_roi.ptr(r)[c]>30?1.0f:0.0f;
        }
    }
	
    ms = end_tick();
    PRINTF_INFO("ptr1 times %d, average time %0.3fms\n", times, ms/times);
    saveDebugImage("ptr1.jpg", dst);

    dst = Mat::zeros(dst_size, dst_size, CV_32F);
    start_tick();

    for(int ti=0; ti<times; ti++)
    {
        for(int r=0; r<min_height; r++)
        {
            for(int c=0; c<min_width; c++)
                dst_roi.at<float>(r, c) = src_roi.at<uchar>(r, c)>30?1.0f:0.0f;
        }
    }
	
    ms = end_tick();
    PRINTF_INFO("ptr2 times %d, average time %0.3fms\n", times, ms/times);
    saveDebugImage("ptr2.jpg", dst);

}


int main(int argc, char** argv)
{
    int dst_size = 600;
    int times = 1000;

    if(argc>1)
        dst_size = atoi(argv[1]);
    
    if(argc>2)
        times = atoi(argv[2]);

    Mat src = imread("test.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    test_roi(src, dst_size, times);
   
    return 0;
}
