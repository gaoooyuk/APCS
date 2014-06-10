#ifndef TRAININGSYSTEM_H
#define TRAININGSYSTEM_H

#include <QObject>
#include <QStringList>
#include <QProcess>

class TrainingSystem : public QObject
{
    Q_OBJECT
public:
    explicit TrainingSystem(QObject *parent = 0);
    ~TrainingSystem();

public slots:
    static QStringList getAllFilesOfDir(QString dirPath);
    void train(int w,
               int h,
               double minHitRate,
               double maxFalseAlarmRate,
               int numStages,
               bool overwrite = true);

    void trainColorBins(int w,
                        int h);

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

private:
    QProcess *m_vjProcess;
};

#endif // TRAININGSYSTEM_H
