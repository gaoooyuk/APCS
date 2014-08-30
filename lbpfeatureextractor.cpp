#include "lbpfeatureextractor.h"
#include "lbp/lbp.hpp"

#include <QDebug>

LbpFeatureExtractor::LbpFeatureExtractor(QObject *parent) :
    FeatureExtractor(parent)
{
}

cv::Mat LbpFeatureExtractor::compute(QString imgPath)
{
    cv::Mat frame_lbp;

    cv::Mat inputImage = cv::imread(imgPath.toUtf8().data(), cv::IMREAD_UNCHANGED);   // Read the file
    if (!inputImage.data)
    {
        qDebug() <<  "Could not open or find the image.";
        return frame_lbp;
    }

    frame_lbp = compute(inputImage);
    return frame_lbp;
}

cv::Mat LbpFeatureExtractor::compute(cv::Mat inputImage)
{
    m_featureVector.clear();

    cv::Mat frame_lbp;
    cv::Mat frame_gray;
    cv::cvtColor(inputImage, frame_gray, CV_BGR2GRAY);

    int radius = 1;
    int neighbours = 8;
//    libfacerec::olbp(frame_gray, frame_lbp);
    libfacerec::elbp(frame_gray, frame_lbp, radius, neighbours);

    int numPatterns = pow(2, neighbours);
    m_featureVector.resize(numPatterns);
    m_featureVector.fill(0);

    frame_lbp.convertTo(frame_lbp, CV_8UC1);
    for (int i = 0; i < frame_lbp.rows; i++)
    {
        for (int j = 0; j < frame_lbp.cols; j++)
        {
            int val = frame_lbp.at<uchar>(i, j);
            int num = m_featureVector.at(val);
            num++;
            m_featureVector.replace(val, num);
        }
    }

    return frame_lbp;
}
