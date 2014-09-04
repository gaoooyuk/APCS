#include "censusfeatureextractor.h"

CensusFeatureExtractor::CensusFeatureExtractor(QObject *parent) :
    FeatureExtractor(parent)
{
}

cv::Mat CensusFeatureExtractor::compute(QString imgPath, int horizontalBins, int verticalBins)
{
    cv::Mat frame_census;

    cv::Mat inputImage = cv::imread(imgPath.toUtf8().data(), cv::IMREAD_UNCHANGED);   // Read the file
    if (!inputImage.data)
    {
        qDebug() <<  "Could not open or find the image.";
        return frame_census;
    }

    frame_census = compute(inputImage, horizontalBins, verticalBins);
    return frame_census;
}

cv::Mat CensusFeatureExtractor::compute(cv::Mat inputImage, int horizontalBins, int verticalBins)
{
    m_featureVector.clear();

    // slightly scale the input image to fit the aspect ratio
    int w = inputImage.cols;
    while (0 != (w % horizontalBins))
    {
        w--;
    }

    int h = inputImage.rows;
    while (0 != (h % verticalBins))
    {
        h--;
    }

    cv::resize(inputImage, inputImage, cv::Size(w, h));

    int m_bin_width = inputImage.cols / horizontalBins;
    int m_bin_height = inputImage.rows / verticalBins;

    cv::Mat frame_census;
    cv::Mat frame_gray;
    cv::cvtColor(inputImage, frame_gray, CV_BGR2GRAY);
    frame_gray.copyTo(frame_census);

    census3x3(frame_gray.data, frame_census.data, w, h);
    frame_census.convertTo(frame_census, CV_8UC1);

    int numPatterns = pow(2, 8);

    for (int i = 0; i < horizontalBins * verticalBins; i++)
    {
        int x = (i % horizontalBins) * m_bin_width;
        int y = (i / horizontalBins) * m_bin_height;

        QVector<int> codes;
        codes.resize(numPatterns);
        codes.fill(0);

        // for each bin of size (m_bin_width, m_bin_height)
        for (int j = y; j < y + m_bin_height; j++)
        {
            for (int k = x; k < x + m_bin_width; k++)
            {
                uchar censusCode = frame_census.at<uchar>(j, k);   // (y, x) in OpenCV

                int codeCount;
                codeCount = codes.at(censusCode);
                codeCount++;
                codes.replace(censusCode, codeCount);
            }
        }

        m_featureVector += codes;
    }

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
