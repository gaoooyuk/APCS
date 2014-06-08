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

    int horizontalBins = 8;
    int verticalBins = 24;

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

//    QHash<int, int> red_bins;
//    QHash<int, int> green_bins;
//    QHash<int, int> blue_bins;
//    QColor color = Qt::black;

    cv::Scalar color;

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

        r = r / (m_bin_width * m_bin_height);
        g = g / (m_bin_width * m_bin_height);
        b = b / (m_bin_width * m_bin_height);
        color = cv::Scalar(b, g, r);

//        intensityMapping(r, red_bins);
//        intensityMapping(g, green_bins);
//        intensityMapping(b, blue_bins);

        // select a ROI
        cv::Mat roi(frame_colorBins, cv::Rect(x, y, m_bin_width, m_bin_height));
        // fill the ROI with the averaged color
        // the original frame_colorBins image will be modified
        roi = color;
    }

    return frame_colorBins;
}
