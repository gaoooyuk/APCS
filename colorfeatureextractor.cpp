#include "colorfeatureextractor.h"

ColorFeatureExtractor::ColorFeatureExtractor(QObject *parent) :
    FeatureExtractor(parent)
{
}

cv::Mat ColorFeatureExtractor::compute(QString imgPath)
{
    cv::Mat frame_colorBins;

    cv::Mat inputImage = cv::imread(imgPath.toUtf8().data(), CV_LOAD_IMAGE_UNCHANGED);   // Read the file
    if (!inputImage.data)
    {
        qDebug() <<  "Could not open or find the image.";
        return frame_colorBins;
    }

    frame_colorBins = compute(inputImage);
    return frame_colorBins;
}

cv::Mat ColorFeatureExtractor::compute(cv::Mat inputImage)
{
    // User-defined bins
    int horizontalBins = 8;
    int verticalBins = 24;

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

    cv::Scalar color;
    QVector<int> output_r;
    QVector<int> output_g;
    QVector<int> output_b;
    for (int i = 0; i < horizontalBins * verticalBins; i++)
    {
        int x = (i % horizontalBins) * m_bin_width;
        int y = (i / horizontalBins) * m_bin_height;

        int r = 0;
        int g = 0;
        int b = 0;
        for (int j = x; j < x + m_bin_width; j++)
        {
            for (int k = y; k < y + m_bin_height; k++)
            {
                cv::Vec3b intensity = inputImage.at<cv::Vec3b>(k, j);   // (y, x) in OpenCV
                uchar blue = intensity.val[0];
                uchar green = intensity.val[1];
                uchar red = intensity.val[2];

                r += red;
                g += green;
                b += blue;
            }
        }

        // calculate array color for each bin
        r = r / (m_bin_width * m_bin_height);
        g = g / (m_bin_width * m_bin_height);
        b = b / (m_bin_width * m_bin_height);

        output_r.append(r);
        output_g.append(g);
        output_b.append(b);

        color = cv::Scalar(b, g, r);

        // select a ROI
        cv::Mat roi(frame_colorBins, cv::Rect(x, y, m_bin_width, m_bin_height));
        // fill the ROI with the averaged color
        // the original frame_colorBins image will be modified
        roi = color;
    }

    m_featureVector += output_r;
    m_featureVector += output_g;
    m_featureVector += output_b;

    return frame_colorBins;
}

QVector<int> ColorFeatureExtractor::featureVector() const
{
    return m_featureVector;
}
