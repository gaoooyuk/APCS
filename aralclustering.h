#ifndef ARALCLUSTERING_H
#define ARALCLUSTERING_H

#include "utility.h"

class ARALClustering
{
public:
    explicit ARALClustering();
    std::vector<int> calculateLocalDensity(std::vector<cv::Point3f> points);
    std::vector<double> calculateDistanceToHigherDensity(std::vector<cv::Point3f> points,
                                                         std::vector<int> rhos);

    int getMaxRho(std::vector<int> rhos);
    double getMaxDelta(std::vector<double> deltas);

private:
    double distance(const cv::Point3f pt1, const cv::Point3f pt2);
};

#endif // ARALCLUSTERING_H
