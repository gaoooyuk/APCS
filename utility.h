#ifndef UTILITY_H
#define UTILITY_H

#include <QObject>
#include <QImage>
#include <QDir>
#include <QColor>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

#include "spatiotemporaldef.h"

class Utility : public QObject
{
    Q_OBJECT
public:
    explicit Utility(QObject *parent = 0);

    static QImage cvMatToQImage(const cv::Mat inMat);
    static cv::Mat QImageToCvMat(const QImage inImage, bool inCloneImageData = true);

    static QFileInfoList getFileInfoList(QString &dirPath);

    static QColor getJetColor(double v, double vmin, double vmax);

    static double calClusterVolumn(std::vector<cv::Point3f> points);

    static STLine fit3DLineToPoints(std::vector<cv::Point3f> points);

    static void fastICA(const cv::Mat mixture);

    // Returns an adjacency matrix based on the gaussian distance between the points
    static cv::Mat gaussianDistance(std::vector<cv::Point3f> points, float sigma);

    static double distance(const cv::Point3f pt1, const cv::Point3f pt2);

    static std::vector<cv::Point3f> QListSTPointToStdVectorCvPoint3f(QList<STPoint> stPoints);
};

#endif // UTILITY_H
