#ifndef VIOLAJONESCLASSIFIER_H
#define VIOLAJONESCLASSIFIER_H

#include <QObject>
#include <QVector>
#include <QHash>
#include <QPointF>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ocl/ocl.hpp>

#include <stdio.h>
#include <math.h>
#include <time.h>

#include "violajonesdefs.h"
#include "utility.h"

class ViolaJonesClassifier : public QObject
{
    Q_OBJECT
public:
    explicit ViolaJonesClassifier(QObject *parent = 0);

    struct VJDetection {
        QList<QPolygonF> polygons;
        QList<double> weights;
    };

    struct VJDetectionPrivate {
        std::vector<cv::Rect> rects;
        std::vector<double> weights;
    };

    void setParams(double scaleFactor, int minNeighbours, QSize minSize);
    VJDetection detectPenguins(cv::Mat videoFrame);
    VJDetectionPrivate detect(cv::Mat frame);
    void rotate(cv::Mat& src, double angle, cv::Mat& dst);

private:
    cv::ocl::OclCascadeClassifier  m_oclClassifier;
    cv::CascadeClassifier m_classifier;
    std::vector<std::string> m_cascade_xmls;
    QHash< QString, QList<QPolygonF> > m_colorRects;

    double m_scaleFactor;
    int m_minNeighbours;
    QSize m_minSize;

    bool m_usingOpenCL;
};

#endif // VIOLAJONESCLASSIFIER_H
