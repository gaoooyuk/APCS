#include "laplacian.h"
#include "opencv2/highgui/highgui.hpp"

Laplacian::Laplacian(QObject *parent) :
    QObject(parent)
{
    m_kernel_size = 3;
    m_scale = 1;
    m_delta = 0;
    m_ddepth = CV_16S;
}

cv::Mat Laplacian::compute(cv::Mat &src)
{
    cv::Mat src_gray;
    // Convert the image to grayscale
    cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);

    // Apply Laplace function
    cv::Mat dst;
    cv::Mat abs_dst;

    cv::Laplacian(src_gray, dst, m_ddepth, m_kernel_size, m_scale, m_delta, cv::BORDER_DEFAULT);
    cv::convertScaleAbs(dst, abs_dst);

    return abs_dst;
}
