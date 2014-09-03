#include "censusfeatureextractor.h"

CensusFeatureExtractor::CensusFeatureExtractor(QObject *parent) :
    FeatureExtractor(parent)
{
}

cv::Mat CensusFeatureExtractor::compute(QString imgPath)
{
    cv::Mat frame_census;

    cv::Mat inputImage = cv::imread(imgPath.toUtf8().data(), cv::IMREAD_UNCHANGED);   // Read the file
    if (!inputImage.data)
    {
        qDebug() <<  "Could not open or find the image.";
        return frame_census;
    }

    frame_census = compute(inputImage);
    return frame_census;
}

cv::Mat CensusFeatureExtractor::compute(cv::Mat inputImage)
{
    cv::Mat frame_census;
    cv::Mat frame_gray;
    cv::cvtColor(inputImage, frame_gray, CV_BGR2GRAY);
    frame_gray.copyTo(frame_census);

    int w = frame_gray.cols;
    int h = frame_gray.rows;

    census3x3(frame_gray.data, frame_census.data, w, h);
    frame_census.convertTo(frame_census, CV_8UC1);

    m_featureVector.clear();

    return frame_census;
}

void CensusFeatureExtractor::census3x3(unsigned char* in, unsigned char* out, int w, int h)
{
    for (int y = 1; y < h-1; y++) {
        out += w;
        in += w;
        for (int x = 1; x < w-1; x++) {
            out[x] = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (i||j) {
                        out[x] <<= 1;
                        if (in[x+j+i*w]>in[x])
                            out[x] |= 1;
                    }
                }
            }
        }
    }
}
