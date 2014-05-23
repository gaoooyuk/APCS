#ifndef TRACKER_H
#define TRACKER_H

#include <QObject>
#include <QPoint>
#include <QList>

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include "violajonesclassifier.h"
#include "randomforest.h"

class Tracker : public QObject
{
    Q_OBJECT

public:
    explicit Tracker(QObject *parent = 0);
    enum TrackingAlgorithm {
        Condensation    // default
    };

public:
    void setInitialState(double x, double y, int width, int height);
    void propagate(cv::Mat frame);
    QList<QPointF> getCandidates();
    bool isLost();
    QPointF getTrackedPos();
    QRect getTrackedRectangle();
    QList<QPointF> getTrajectory();
    QList<QRect> getTrajectoryRects();

private:
    bool initialise();

    // Condensation Algorithm
    bool condensation_init();
    void condensation_init_defaults();
    void condensation_set_up_prior_conditions();
    void condensation_obtain_observations();
    void condensation_predict_new_bases();
    int condensation_pick_base_sample();
    void condensation_predict_sample_position(int new_sample, int old_sample);
    void condensation_calculate_base_weights();
    void condensation_update_after_iterating();
    double condensation_evaluate_observation_density(QPointF samplePoint);
    QPointF condensation_cal_estimated_pos();
    void condensation_shut_down();

private:
    bool m_initialized;
    double m_initX;    // start x position
    double m_initY;    // start y position
    int m_initWidth;
    int m_initHeight;

    cv::Mat m_currentFrame;
    QList<QPointF> m_trajectory;
    bool m_isLost;

    ViolaJonesClassifier m_vjClassifier;

};

#endif // TRACKER_H
