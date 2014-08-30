#include "aralclustering.h"

ARALClustering::ARALClustering()
{
}

std::vector<int> ARALClustering::calculateLocalDensity(std::vector<cv::Point3f> points)
{
    int nSamples = (int)points.size();
    std::vector<int> rhos;
    for (int i = 0; i < nSamples; i++)
    {
        int rho = 0;
        double dc = 100;
        for (int j = 0; j < nSamples; j++)
        {
            if (i == j)
                continue;

            int res = distance(points.at(i), points.at(j)) < dc ? 1 : 0;
            rho += res;
        }

        rhos.push_back(rho);
    }

    return rhos;
}

std::vector<double> ARALClustering::calculateDistanceToHigherDensity(std::vector<cv::Point3f> points,
                                                                     std::vector<int> rhos)
{
    int nSamples = (int)points.size();
    std::vector<double> deltas;

    // 1. Find the index(point) with the highest rho
    std::vector<int>::iterator res;
    res = std::max_element(rhos.begin(), rhos.end());
    int maxVal = *res;
    std::vector<int> maxIdxs;

    for (size_t n = 0; n < rhos.size(); n++)
    {
        if (rhos.at(n) == maxVal)
            maxIdxs.push_back((int)n);
    }

    // 2.
    for (int i = 0; i < nSamples; i++)
    {
        bool flag = false;
        if (std::find(maxIdxs.begin(), maxIdxs.end(), i) != maxIdxs.end())
            flag = true;

        double delta;
        if (flag)
            delta = 0.0;
        else
            delta = 100000.0;

        for (int j = 0; j < nSamples; j++)
        {
            if (i == j)
                continue;

            double tmp = distance(points.at(i), points.at(j));

            if (flag)
            {
                delta = tmp > delta ? tmp : delta;
            } else {
                if (rhos[j] > rhos[i])
                {
                    delta = delta < tmp ? delta : tmp;
                }
            }
        }

        deltas.push_back(delta);
    }

    return deltas;
}

int ARALClustering::getMaxRho(std::vector<int> rhos)
{
    std::vector<int>::iterator res;
    res = std::max_element(rhos.begin(), rhos.end());
    return *res;
}

double ARALClustering::getMaxDelta(std::vector<double> deltas)
{
    std::vector<double>::iterator res;
    res = std::max_element(deltas.begin(), deltas.end());
    return *res;
}

double ARALClustering::distance(const cv::Point3f pt1, const cv::Point3f pt2)
{
    return sqrt( pow(pt1.x - pt2.x, 2) + pow(pt1.y - pt2.y, 2) + pow(pt1.z - pt2.z, 2) );
}
