#ifndef LAPLACIAN_H
#define LAPLACIAN_H

#include <QObject>
#include <opencv2/imgproc/imgproc.hpp>

class Laplacian : public QObject
{
    Q_OBJECT

public:
    explicit Laplacian(QObject *parent = 0);
    cv::Mat compute(cv::Mat &src);

private:
    int m_kernel_size;
    int m_scale;
    int m_delta;
    int m_ddepth;
};

#endif // LAPLACIAN_H
