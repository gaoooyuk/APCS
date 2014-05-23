#ifndef VIOLAJONESDEFS_H
#define VIOLAJONESDEFS_H

#include <QColor>
#include <opencv2/core/core.hpp>

#define PI 3.14159265

// Penguin descriptors in different poses
// Frontal
#define penguin_cascade_frontal "cascade_frontal.xml"
//#define penguin_cascade_frontal "cascade_frontal_pruned.xml"

// 90 degree
#define penguin_cascade_90 "cascade_90.xml"
#define penguin_cascade_90_upper "cascade_90_upper.xml"
#define penguin_cascade_90_lower "cascade_90_lower.xml"

//#define penguin_cascade_90 "cascade_90_pruned.xml"
//#define penguin_cascade_90_upper "cascade_90_upper_pruned.xml"
//#define penguin_cascade_90_lower "cascade_90_lower_pruned.xml"

// 45 degree
#define penguin_cascade_45 "cascade_45.xml"
//#define penguin_cascade_45 "cascade_45_pruned.xml"

#define penguin_cascade_head "cascade_head.xml"

#define window_name "African Penguin Counter"

#define text_body_frontal "Frontal"

#define text_body_left90 "L90"
#define text_body_left90_upper "L90U"
#define text_body_left90_lower "L90L"

#define text_body_right90 "R90"
#define text_body_right90_upper "R90U"
#define text_body_right90_lower "R90L"

// Colors
#define COLOR_FRONTAL cv::Scalar(0, 255, 102)
#define COLOR_90 cv::Scalar(0, 255, 255)
#define COLOR_90_UPPER cv::Scalar(51, 153, 204)
#define COLOR_90_LOWER cv::Scalar(51, 102, 204)
#define COLOR_45 cv::Scalar(255, 153, 51)
#define COLOR cv::Scalar(255, 255, 255)

#define Q_COLOR_FRONTAL QColor(102, 255, 0)
#define Q_COLOR_90 QColor(255, 255, 0)
#define Q_COLOR_90_UPPER QColor(204, 153, 51)
#define Q_COLOR_90_LOWER QColor(204, 102, 51)
#define Q_COLOR_45 QColor(51, 153, 255)

#endif // VIOLAJONESDEFS_H
