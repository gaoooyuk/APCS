#include "edgefeatureextractor.h"
#include "hog/laplacian.h"

EdgeFeatureExtractor::EdgeFeatureExtractor(QObject *parent) :
    FeatureExtractor(parent)
{
}

cv::Mat EdgeFeatureExtractor::compute(QString imgPath, int horizontalBins, int verticalBins)
{
    cv::Mat frame_edge;

    cv::Mat inputImage = cv::imread(imgPath.toUtf8().data(), cv::IMREAD_UNCHANGED);   // Read the file
    if (!inputImage.data)
    {
        qDebug() <<  "Could not open or find the image.";
        return frame_edge;
    }

    frame_edge = compute(inputImage, horizontalBins, verticalBins);
    return frame_edge;
}

cv::Mat EdgeFeatureExtractor::compute(cv::Mat inputImage, int horizontalBins, int verticalBins)
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

    cv::Mat frame_edge;
    Laplacian laplace;
    frame_edge = laplace.compute(inputImage);

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
                uchar edgeCode = frame_edge.at<uchar>(j, k);   // (y, x) in OpenCV

                int codeCount;
                codeCount = codes.at(edgeCode);
                codeCount++;
                codes.replace(edgeCode, codeCount);
            }
        }

        m_featureVector += codes;
    }

    return frame_edge;
}
