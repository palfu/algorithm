#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>

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


void saveDebugImage(string filename,  Mat& mat)
{
    Mat save = mat.clone();
    if(mat.type()==CV_32F)
    {
        save.convertTo(save, CV_8U);
        save = save*100;
    }
    imwrite(filename, save);
}

int overThresholdNbr(cv::Mat& mat, float thresh)
{
    Mat over;
    threshold(mat, over, thresh, 2.0, THRESH_BINARY);
    over.convertTo(over, CV_8U);
    return countNonZero(over);
}

double filterRes(cv::Mat& src, cv::Mat& dst, float ratio, float thresh)
{

    double res = 0.0;
    for(int r=0; r<dst.rows; r++)
    {
        for(int c=0; c<dst.cols; c++)
        {
            int rr = int(r*ratio+0.5f);
            int cc = int(c*ratio+0.5f);

            float v1 = src.at<float>(rr, cc);
            float v0 = dst.at<float>(r, c);

            if(v0<thresh && v1<thresh)
            res += (v1-v0)*(v1-v0);
        }
    }

    return res;
}

float findBestSigma(cv::Mat& src, int src_size, float src_sigma, float focal, int test_size, float thresh=0.1)
{
    Mat basic_kernel = getGaussianKernel(src_size, src_sigma, CV_32F);
    // cout<<"basic kernel: \n"<<basic_kernel<<endl;

    Mat src_cut, src_float, src_filter;
    threshold(src, src_cut, 30, 1, THRESH_BINARY);
    src_cut.convertTo(src_float, CV_32F);
    filter2D(src_float, src_filter, -1, basic_kernel);

    saveDebugImage("src_filter.jpg", src_filter);
    PRINTF_INFO("src image valid nbr %d\n", overThresholdNbr(src_filter, thresh));

    Mat src_resize, dst_float, dst_filter;
    Size new_size = Size(int(src.cols/focal), int(src.rows/focal));
    PRINTF_INFO("resize image size %dx%d\n", new_size.width, new_size.height);

    resize(src, src_resize, new_size);
    threshold(src_resize, dst_float, 30, 1, THRESH_BINARY);
    dst_float.convertTo(dst_float, CV_32F);
    
    Mat best_filter;
    float new_sigma=0.1;
    double min_res=1e8;
    float best_sigma = new_sigma;
    while(new_sigma<2*src_sigma)
    {
        Mat dst_filter;
        GaussianBlur(dst_float, dst_filter, Size(test_size, test_size), new_sigma);
        double res = filterRes(src_filter, dst_filter, focal, thresh);

        if(min_res>res)
        {
            min_res = res;
            best_filter = dst_filter.clone();
            best_sigma = new_sigma;
        }

        new_sigma+=0.1;
    }

    int best_nbr = overThresholdNbr(best_filter, thresh);
    PRINTF_INFO("for dst kernel size %d, find best sigma %0.2f, valid nbr %d, filter res %0.6f, average res %0.6f\n", test_size, best_sigma, best_nbr, min_res, min_res/best_nbr);
    saveDebugImage("best_filter.jpg", best_filter);

    return best_sigma;
}



int main(int argc, char** argv)
{

    Mat basic_image = imread("test.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    int src_size = 25;
    float src_sigma = 12.0f;
    float focal = 2.0f;
    int test_size = 5;

    PRINTF_INFO("input: src_filter_size, src_filter_sigma, resize_ratio, resize_filter_size\n");
    if(argc>1)
        src_size = atoi(argv[1]);
    if(argc>2)
        src_sigma = atof(argv[2]);
    if(argc>3)
        focal  = atof(argv[3]);
    if(argc>4)
        test_size = atoi(argv[4]);
    
    PRINTF_INFO("src_filter_size %d, src_filter_sigma %0.2f, resize_ratio %0.2f, resize_filter_size %d\n", src_size, src_sigma, focal, test_size);
    findBestSigma(basic_image, src_size, src_sigma, focal, test_size, 0.1);

    return 0;
}