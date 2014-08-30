#ifndef TRACKER_H
#define TRACKER_H

#include <QObject>
#include <QPoint>
#include <QList>
#include <QRect>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "violajonesclassifier.h"

class Tracker : public QObject
{
    Q_OBJECT

public:
    explicit Tracker(QObject *parent = 0);
    enum TrackingAlgorithm {
        Condensation    // default
    };

    enum ClusteringAlgorithm {
        SpectralClustering,    // default
        KMeansClustering,
        ARALClustering
    };

    struct Particle {
        QPointF pos;
        float weight;
    };

    struct Trajectory {
        QList<QPointF> points;
        QColor color;
        int label;
    };

public:
    void setFrameSize(int width, int height);
    void setInitialState(double x, double y, int width, int height);
    void propagate(ViolaJonesClassifier::VJDetection obs);

    QList<Particle> getParticles();
    QList<Particle> getDetections();


    QList<STCluster> getSTClusters();
    QList<STLine> getAllLineSegments();
    QList<STPoint> getAllSTPoints();
    QList<STPoint> getFilteredSTPoints();
    QList<STCone> getSTCones();

    QList<STPoint> getManualLabelings();

    QList< QList<QColor> > getColorLabels();
    QList<Trajectory> getTrajectories();
    cv::Mat getDetectionConfidenceMap();

    int getEstimatedTargetAmount();
    QList<int> getCountingResult();

    bool isLost();
    bool isInit();

public slots:
    void addManualLabeling(float x, float y, int frameNo);

private:
    bool initialise();
    QPointF generateRandomPosition();
    bool insideFrame(QPointF sample);
    void runClusteringModelSelection(int startFrame, int endFrame);
    QList<STPoint> runSpaceTimeConeFiltering(int startFrame, int endFrame);   // This one maybe replace runClusterOutlierRemoval() in the future
    void runClusterOutlierRemoval();
    void runClusterEigenLineCalculation();
    void runTrajectoryCounting();

    // spatio-temporal get data points between frame "startFrame" and "endFrame"
    std::vector<cv::Point3f> st_getPoints(int startFrame, int endFrame);
    int clusterDetections_HoughTransform(int startFrame, int endFrame);

    double calculateMDL(int k, cv::Mat points, cv::Mat labels);  // calculate Minimum Description Length
    int getTotalDetectionCount();

    // Condensation Algorithm
    bool condensation_init();
    void condensation_init_defaults();
    void condensation_set_up_prior_conditions();
    void condensation_obtain_observations(ViolaJonesClassifier::VJDetection obs);
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

    int m_frameWidth;
    int m_frameHeight;

    double m_initX;    // start x position
    double m_initY;    // start y position
    int m_initWidth;
    int m_initHeight;

    bool m_isLost;

    cv::Mat m_distanceMap;
    cv::Mat m_confidenceMap;
    cv::Mat m_labels;
    QHash<int, double> m_confidenceHash;

    QList<ViolaJonesClassifier::VJDetection> m_spatioTemporalDetections;
    QList<STCluster> m_clusters;
    QList<STLine> m_lineSegments;
    QList<Trajectory> m_trajectories;
    QList<STPoint> m_manualLabelings;
    QList<STPoint> m_filteredPoints;
    QList<STCone> m_cones;

    QList< QList<QColor> > m_colorLabels;
    QList<int> m_sectionClusteringResult;

    ClusteringAlgorithm m_clusteringAlgorithm;

    int m_estimatedTargetAmount;
};

#endif // TRACKER_H
