#include "colorfeatureextractor.h"

ColorFeatureExtractor::ColorFeatureExtractor(QObject *parent) :
    FeatureExtractor(parent)
{
}

cv::Mat ColorFeatureExtractor::compute(QString imgPath, int horizontalBins, int verticalBins)
{
    cv::Mat frame_colorBins;

    cv::Mat inputImage = cv::imread(imgPath.toUtf8().data(), cv::IMREAD_UNCHANGED);   // Read the file
    if (!inputImage.data)
    {
        qDebug() <<  "Could not open or find the image.";
        return frame_colorBins;
    }

    frame_colorBins = compute(inputImage, horizontalBins, verticalBins);
    return frame_colorBins;
}

cv::Mat ColorFeatureExtractor::compute(cv::Mat inputImage, int horizontalBins, int verticalBins)
{
    cv::Mat frame_colorBins;
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
    inputImage.copyTo(frame_colorBins);

    int m_bin_width = inputImage.cols / horizontalBins;
    int m_bin_height = inputImage.rows / verticalBins;

//    cv::Scalar color;
    QVector<int> output_r;
    QVector<int> output_g;
    QVector<int> output_b;
    for (int i = 0; i < horizontalBins * verticalBins; i++)
    {
        int x = (i % horizontalBins) * m_bin_width;
        int y = (i / horizontalBins) * m_bin_height;

        QVector<int> r;
        r.resize(256);
        r.fill(0);
        QVector<int> g;
        g.resize(256);
        g.fill(0);
        QVector<int> b;
        b.resize(256);
        b.fill(0);

        for (int j = y; j < y + m_bin_height; j++)
        {
            for (int k = x; k < x + m_bin_width; k++)
            {
                cv::Vec3b intensity = inputImage.at<cv::Vec3b>(j, k);   // (y, x) in OpenCV
                uchar blue = intensity.val[0];
                uchar green = intensity.val[1];
                uchar red = intensity.val[2];

                int colorCount;

                colorCount = r.at(red);
                colorCount++;
                r.replace(red, colorCount);

                colorCount = r.at(green);
                colorCount++;
                g.replace(red, colorCount);

                colorCount = r.at(blue);
                colorCount++;
                b.replace(red, colorCount);
            }
        }

//        // calculate average color for each bin
//        r = r / (m_bin_width * m_bin_height);
//        g = g / (m_bin_width * m_bin_height);
//        b = b / (m_bin_width * m_bin_height);

        output_r += r;
        output_g += g;
        output_b += b;

//        color = cv::Scalar(b, g, r);

//        // select a ROI
//        cv::Mat roi(frame_colorBins, cv::Rect(x, y, m_bin_width, m_bin_height));
//        // fill the ROI with the averaged color
//        // the original frame_colorBins image will be modified
//        roi = color;
    }

    m_featureVector += output_r;
    m_featureVector += output_g;
    m_featureVector += output_b;

    return frame_colorBins;
}
