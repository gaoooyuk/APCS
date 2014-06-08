#include "edgefeatureextractor.h"
#include "hog/laplacian.h"

EdgeFeatureExtractor::EdgeFeatureExtractor(QObject *parent) :
    FeatureExtractor(parent)
{
}

cv::Mat EdgeFeatureExtractor::compute(QString imgPath)
{
    cv::Mat frame_edge;

    cv::Mat inputImage = cv::imread(imgPath.toUtf8().data(), CV_LOAD_IMAGE_UNCHANGED);   // Read the file
    if (!inputImage.data)
    {
        qDebug() <<  "Could not open or find the image.";
        return frame_edge;
    }

    Laplacian laplace;
    frame_edge = laplace.compute(inputImage);

    return frame_edge;
}
