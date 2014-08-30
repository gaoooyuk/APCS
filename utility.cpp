#include "utility.h"
#include <iostream>
#include <QDebug>

#include <itpp/itsignal.h>
// http://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/

Utility::Utility(QObject *parent) :
    QObject(parent)
{
}

QImage Utility::cvMatToQImage(const cv::Mat inMat)
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

cv::Mat Utility::QImageToCvMat(const QImage inImage, bool inCloneImageData)
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

QFileInfoList Utility::getFileInfoList(QString &dirPath)
{
    QDir dir(dirPath);
    QStringList filters;
    filters << "*.jpg" << "*.png" << "*.jpeg";
    return dir.entryInfoList(filters, QDir::Files);
}

QColor Utility::getJetColor(double v, double vmin, double vmax)
{
    QColor color(255, 255, 255); // white

    if (v < vmin)
        v = vmin;

    if (v > vmax)
        v = vmax;

    double dv = vmax - vmin;

    if (v < (vmin + 0.25 * dv))
    {
        double g = 4 * (v - vmin) / dv;
        color.setRed(0);
        color.setGreen(255 * g);
    } else if (v < (vmin + 0.5 * dv)) {
        double b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
        color.setRed(0);
        color.setBlue(255 * b);
    } else if (v < (vmin + 0.75 * dv)) {
        double r = 4 * (v - vmin - 0.5 * dv) / dv;
        color.setRed(255 * r);
        color.setBlue(0);
    } else {
        double g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
        color.setGreen(255 * g);
        color.setBlue(0);
    }

    return color;
}

double Utility::calClusterVolumn(std::vector<cv::Point3f> points)
{
    int m = static_cast<int>(points.size());
    cv::Mat_<float> pointCloud(m, 3, CV_32FC1);
    for (int i = 0; i < m; i++)
    {
        pointCloud.row(i)(0) = points[i].x;
        pointCloud.row(i)(1) = points[i].y;
        pointCloud.row(i)(2) = points[i].z;
    }

    cv::Mat covarMatrix = cv::Mat(3, 3, CV_32FC1);
    cv::Mat meanMatrix = cv::Mat(1, 3, CV_32FC1);
    cv::calcCovarMatrix(pointCloud, covarMatrix, meanMatrix, CV_COVAR_NORMAL|CV_COVAR_ROWS, CV_32FC1);

    /* Perform eigen decompositions */
    cv::Mat eigenvalues, eigenvectors;
    cv::eigen(covarMatrix, eigenvalues, eigenvectors);

    cv::Point3f meanPoint;
    cv::Point3f start;
    cv::Point3f end;
    cv::Point3f direction;

    meanPoint.x = meanMatrix.at<float>(0);
    meanPoint.y = meanMatrix.at<float>(1);
    meanPoint.z = meanMatrix.at<float>(2);

    cv::Mat eigenvector = eigenvectors.row(0).t();
    float largestEigenVal = eigenvalues.at<float>(0);
    float eigenVecX = eigenvector.at<float>(0);
    float eigenVecY = eigenvector.at<float>(1);
    float eigenVecZ = eigenvector.at<float>(2);

    start.x = meanPoint.x - sqrt(largestEigenVal) * eigenVecX;
    start.y = meanPoint.y - sqrt(largestEigenVal) * eigenVecY;
    start.z = meanPoint.z - sqrt(largestEigenVal) * eigenVecZ;

    end.x = meanPoint.x + sqrt(largestEigenVal) * eigenVecX;
    end.y = meanPoint.y + sqrt(largestEigenVal) * eigenVecY;
    end.z = meanPoint.z + sqrt(largestEigenVal) * eigenVecZ;

    direction = end - start;
    cv::Mat normalizedDirection;
    cv::normalize(cv::Mat(direction), normalizedDirection);

    //    std::cout << "meanPoint:" << meanPoint << std::endl;
    //    std::cout << "direction:" << direction << std::endl;
    //    std::cout << "normalizedDirection:" << normalizedDirection << std::endl;

    cv::Point3f p1 = start;
    cv::Point3f p2 = end;

    double p1p2 = sqrt( pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2) + pow((p1.z - p2.z), 2) );

    double volume = 0.0;
    for (int i = 0; i < m; i++)
    {
        cv::Point3f p(points[i].x, points[i].y, points[i].z);
        double pp1 = sqrt( pow((p.x - p1.x), 2) + pow((p.y - p1.y), 2) + pow((p.z - p1.z), 2) );
        double pp2 = sqrt( pow((p.x - p2.x), 2) + pow((p.y - p2.y), 2) + pow((p.z - p2.z), 2) );

        double s = (pp1 + pp2 + p1p2) / 2;   // semiperimeter
        double tmp = s*(s-pp1)*(s-pp2)*(s-p1p2);
        tmp = tmp < 0 ? 0 : tmp;
        double area = sqrt(tmp);
        double d = area * 2 / p1p2;  // perpendicular distance

        //        qDebug() << "pp1:" << pp1;
        //        qDebug() << "pp2:" << pp2;
        //        qDebug() << "p1p2:" << p1p2;
        //        qDebug() << "s:" << s;
        //        qDebug() << "s*(s-pp1)*(s-pp2)*(s-p1p2):" << s*(s-pp1)*(s-pp2)*(s-p1p2);
        //        qDebug() << "d:" << d;

        volume += d;
    }

    //    qDebug() << "Cluster volume:" << volume;
    return volume;
}

STLine Utility::fit3DLineToPoints(std::vector<cv::Point3f> points)
{
    int m = static_cast<int>(points.size());
    cv::Mat_<float> pointCloud(m, 3, CV_32FC1);
    for (int i = 0; i < m; i++)
    {
        pointCloud.row(i)(0) = points[i].x;
        pointCloud.row(i)(1) = points[i].y;
        pointCloud.row(i)(2) = points[i].z;
    }

    cv::Mat covarMatrix = cv::Mat(3, 3, CV_32FC1);
    cv::Mat meanMatrix = cv::Mat(1, 3, CV_32FC1);
    cv::calcCovarMatrix(pointCloud, covarMatrix, meanMatrix, CV_COVAR_NORMAL|CV_COVAR_ROWS, CV_32FC1);

    /* Perform eigen decompositions */
    cv::Mat eigenvalues, eigenvectors;
    cv::eigen(covarMatrix, eigenvalues, eigenvectors);

    cv::Point3f meanPoint;
    cv::Point3f start;
    cv::Point3f end;
    cv::Point3f direction;

    meanPoint.x = meanMatrix.at<float>(0);
    meanPoint.y = meanMatrix.at<float>(1);
    meanPoint.z = meanMatrix.at<float>(2);

    cv::Mat eigenvector = eigenvectors.row(0).t();
    float largestEigenVal = eigenvalues.at<float>(0);
    float eigenVecX = eigenvector.at<float>(0);
    float eigenVecY = eigenvector.at<float>(1);
    float eigenVecZ = eigenvector.at<float>(2);

    start.x = meanPoint.x - sqrt(largestEigenVal) * eigenVecX;
    start.y = meanPoint.y - sqrt(largestEigenVal) * eigenVecY;
    start.z = meanPoint.z - sqrt(largestEigenVal) * eigenVecZ;

    end.x = meanPoint.x + sqrt(largestEigenVal) * eigenVecX;
    end.y = meanPoint.y + sqrt(largestEigenVal) * eigenVecY;
    end.z = meanPoint.z + sqrt(largestEigenVal) * eigenVecZ;

    direction = end - start;

    int sTime = points[0].z;
    int eTime = points[m-1].z;

    float alpha0 = (sTime - start.z) / (end.z - start.z);
    float alpha1 = (eTime - start.z) / (end.z - start.z);

    STLine line;
    line.start = start + alpha0 * direction;
    line.end = start + alpha1 * direction;

    return line;
}

void Utility::fastICA(const cv::Mat mixture)
{
    // Separate nrIC independent components in nrSamples samples
    int nrSamples = mixture.rows;
    int nrIC = 3;
    itpp::mat X = itpp::zeros(nrIC, nrSamples);

    //    qDebug() << "FastICA Num of samples:" << nrSamples;
    //    qDebug() << "FastICA Dimension of sample:" << nrIC;

    for (int i = 0; i < nrSamples; i++)
    {
        for (int j = 0; j < nrIC; j++)
        {
            X(j, i) = mixture.at<float>(i, j);
        }
    }

    itpp::RNG_reset(0);//separate uses randu

    itpp::Fast_ICA myFastICA(X);

    myFastICA.set_nrof_independent_components(nrSamples);

    // Perform ICA
    bool result = myFastICA.separate();
    if (false == result)
    {
        //        qDebug() << "FastICA failed";
    } else {
        // Get results
        itpp::mat mixing_matrix = myFastICA.get_mixing_matrix();
        itpp::mat separating_matrix = myFastICA.get_separating_matrix();
        itpp::mat independent_components = myFastICA.get_independent_components();

        //        std::cout << "FastICA result:" << independent_components << std::endl;

        //        qDebug() << "FastICA Num of IC:" << myFastICA.get_nrof_independent_components();
        //        qDebug() << "FastICA result rows:" << independent_components.rows();
        //        qDebug() << "FastICA result cols:" << independent_components.cols();
    }
}

cv::Mat Utility::gaussianDistance(std::vector<cv::Point3f> points, float sigma)
{
    int m = static_cast<int>(points.size());
    cv::Mat adjacency(m, m, CV_32FC1, cv::Scalar(0.0));

    for (int i = 0; i < m; i++)
    {
        float *ptr = adjacency.ptr<float>(i);
        for (int j = 0; j < m; j++)
        {
            if (i >= j)
                continue;

            cv::Point3f p1(points[i].x, points[i].y, points[i].z);
            cv::Point3f p2(points[j].x, points[j].y, points[j].z);
            ptr[j] = (p1 - p2).dot(p1 - p2);
        }
    }

    cv::completeSymm(adjacency);

    adjacency = -adjacency / (2.0 * sigma * sigma);
    cv::exp(adjacency, adjacency);
    adjacency = adjacency - cv::Mat::eye(m, m, CV_32FC1);

    return adjacency;
}

double Utility::distance(const cv::Point3f pt1, const cv::Point3f pt2)
{
    return sqrt( pow(pt1.x - pt2.x, 2) + pow(pt1.y - pt2.y, 2) + pow(pt1.z - pt2.z, 2) );
}

std::vector<cv::Point3f> Utility::QListSTPointToStdVectorCvPoint3f(QList<STPoint> stPoints)
{
    std::vector<cv::Point3f> points;
    for (int i = 0; i < stPoints.length(); i++)
    {
        STPoint stPoint = stPoints.at(i);
        cv::Point3f point(stPoint.x, stPoint.y, stPoint.t);

        points.push_back(point);
    }

    return points;
}
