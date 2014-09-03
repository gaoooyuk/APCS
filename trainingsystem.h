#ifndef TRAININGSYSTEM_H
#define TRAININGSYSTEM_H

#include <QObject>
#include <QStringList>
#include <QProcess>
#include "violajonesclassifier.h"
#include "randomforest.h"

class TrainingSystem : public QObject
{
    Q_OBJECT
public:
    explicit TrainingSystem(QObject *parent = 0);
    ~TrainingSystem();

public slots:
    static QStringList getAllFilesOfDir(QString dirPath);

    void trainVJ(int w,
                 int h,
                 double minHitRate,
                 double maxFalseAlarmRate,
                 int numStages,
                 bool overwrite = true);

    void testVJ(QString posImageFolder,
                QString negImageFolder,
                int w,
                int h);

    void trainRF(int w, int h);

    void testRF(QString posImageFolder,
                QString negImageFolder);

signals:
    void testImageChanged(QString imgFile);
    void testSampleCountChanged(int posCount, int negCount);
    void testTPRChanged(float tpr);
    void testFPRChanged(float fpr);

private slots:
    void handleTrainingProcessError();
    void handleTrainingProcessMsg();
    void handleTrainingProcessFinished(int exitCode);

private:
    void generateImageDB(QString dbName, QStringList fileList, bool pathOnly = true);
    void generateMoreSamples(int targetNum,
                             int targetWidth,
                             int targetHeight,
                             QStringList fileList);
    QVector<int> computeFeatureVectors(QString imgPath, int w, int h);

private:
    QProcess *m_vjProcess;
    ViolaJonesClassifier m_vjClassifier;
    RandomForest m_randomForest;
};

#endif // TRAININGSYSTEM_H
