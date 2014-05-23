#ifndef UTILITY_H
#define UTILITY_H

#include <QObject>
#include <QImage>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

class Utility : public QObject
{
    Q_OBJECT
public:
    explicit Utility(QObject *parent = 0);

    static QImage cvMatToQImage(const cv::Mat &inMat);
    static cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData);
};

#endif // UTILITY_H
