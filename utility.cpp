#include "utility.h"
#include <QDebug>

// http://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/

Utility::Utility(QObject *parent) :
    QObject(parent)
{
}

QImage Utility::cvMatToQImage(const cv::Mat &inMat)
{
    switch (inMat.type())
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32);
        return image;
    }

        // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }

    case CV_8UC1:
    {
        static QVector<QRgb> ctable;
        if (ctable.isEmpty())
        {
            for (int i = 0; i < 256; ++i)
                ctable.push_back(qRgb(i, i, i));
        }

        QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8);
        image.setColorTable(ctable);

        return image;
    }

    default:
        qWarning() << "Utility::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
        break;
    }

    return QImage();
}

cv::Mat Utility::QImageToCvMat(const QImage &inImage, bool inCloneImageData = true)
{
    switch (inImage.format())
    {
    // 8-bit, 4 channel
    case QImage::Format_RGB32:
    {
        cv::Mat mat(inImage.height(), inImage.width(), CV_8UC4, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine());

        return (inCloneImageData ? mat.clone() : mat);
    }

        // 8-bit, 3 channel
    case QImage::Format_RGB888:
    {
        if (!inCloneImageData)
            qWarning() << "Utility::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";

        QImage swapped = inImage.rgbSwapped();

        return cv::Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine()).clone();
    }

        // 8-bit, 1 channel
    case QImage::Format_Indexed8:
    {
        cv::Mat mat(inImage.height(), inImage.width(), CV_8UC1, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine());
        return (inCloneImageData ? mat.clone() : mat);
    }

    default:
        qWarning() << "Utility::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
        break;
    }

    return cv::Mat();
}
