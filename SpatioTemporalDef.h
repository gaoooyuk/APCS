#ifndef SPATIOTEMPORALDEF_H
#define SPATIOTEMPORALDEF_H

#include <QColor>
#include <QList>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

typedef struct STLine {
    cv::Point3f start;
    cv::Point3f end;
} STLine;
Q_DECLARE_METATYPE(STLine)

typedef struct STPoint
{
    bool operator==(const STPoint& other) const
    {
        return (x == other.x && y == other.y && t == other.t);
    }

    float x;
    float y;
    int t;
} STPoint;
Q_DECLARE_METATYPE(STPoint)

typedef struct STCone
{
    QList<STPoint> points;
} STCone;
Q_DECLARE_METATYPE(STCone)

typedef struct STCluster {
    int label;
    QColor color;
    QList<STPoint> points;
    STLine eigenLine;
} STCluster;
Q_DECLARE_METATYPE(STCluster)

#endif // SPATIOTEMPORALDEF_H
