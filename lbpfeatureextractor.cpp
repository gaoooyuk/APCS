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

    cv::Mat inputImage = cv::imread(imgPath.toUtf8().data(), CV_LOAD_IMAGE_UNCHANGED);   // Read the file
    if (!inputImage.data)
    {
        qDebug() <<  "Could not open or find the image.";
        return frame_lbp;
    }

    cv::Mat frame_gray;
    cv::cvtColor(inputImage, frame_gray, CV_BGR2GRAY);

//    libfacerec::olbp(frame_gray, frame_lbp);
    libfacerec::elbp(frame_gray, frame_lbp, 1, 8);

    frame_lbp.convertTo(frame_lbp, CV_8UC1);

    return frame_lbp;
}
