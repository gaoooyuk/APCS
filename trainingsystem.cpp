#include "trainingsystem.h"
#include "utility.h"
#include "colorfeatureextractor.h"
#include "lbpfeatureextractor.h"

#include <QProcess>

#include <QDebug>

#define NUM_OBJECT_1 1

TrainingSystem::TrainingSystem(QObject *parent) :
    QObject(parent)
{
    m_vjProcess = 0;
}

TrainingSystem::~TrainingSystem()
{
    qDebug() << "TrainingSystem::~TrainingSystem()";
}

QStringList TrainingSystem::getAllFilesOfDir(QString dirPath)
{
    QStringList absoluteFilePathList;
    QFileInfoList infos = Utility::getFileInfoList(dirPath);
    foreach (QFileInfo info, infos)
    {
        absoluteFilePathList.append(info.absoluteFilePath());
    }

    return absoluteFilePathList;
}

void TrainingSystem::trainVJ(int w,
                             int h,
                             double minHitRate,
                             double maxFalseAlarmRate,
                             int numStages,
                             bool overwrite)
{
    if (0 == m_vjProcess)
    {
        m_vjProcess = new QProcess(this);
        connect(m_vjProcess, SIGNAL(readyReadStandardError()), this, SLOT(handleTrainingProcessError()));
        connect(m_vjProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(handleTrainingProcessMsg()));
        connect(m_vjProcess, SIGNAL(finished(int)), this, SLOT(handleTrainingProcessFinished(int)));
    } else {
        QProcess::ProcessState p_state = m_vjProcess->state();
        if (QProcess::Starting == p_state || QProcess::Running == p_state)
        {
            qDebug() << "The training process is already running, wait for the result";
            return;
        }
    }

    QString folderName = QString("vj_classifier_%1x%2").arg(QString::number(w)).arg(QString::number(h));

    QString posFolder
            = QString("/Users/apple/Desktop/Courses/Penguin/training_data/positives/frontal_%1x%2")
            .arg(QString::number(w))
            .arg(QString::number(h));
    QStringList positiveSampleList =  getAllFilesOfDir(posFolder);
    int numPosPerStage = positiveSampleList.count();

    QStringList negativeSampleList =  getAllFilesOfDir("/Users/apple/Desktop/Courses/Penguin/training_data/negatives");

    if (overwrite)
    {
        generateImageDB("negatives.dat", negativeSampleList);
        int posSamplesNeeded = round( numPosPerStage + (numStages - 1)*(1.0 - minHitRate)*numPosPerStage );

        generateMoreSamples(posSamplesNeeded, w, h, positiveSampleList);

        // Create output dir
        QString dirPath = QDir::currentPath() + QDir::separator() + folderName;
        QDir dir(dirPath);
        if (dir.exists())
        {
            bool ret = dir.removeRecursively();
            if (!ret)
            {
                qDebug() << "Can't clean the old dir(vj_classifier)";
            }
        }
        bool res = dir.mkdir(dirPath);
        if (!res)
        {
            qDebug() << "Can't create the training dir(vj_classifier)";
            return;
        }
    }


    qDebug() << "Preparation done.";
    return;



    QStringList trainingAgms;
    trainingAgms << "-vec" << "samples.vec";
    trainingAgms << "-bg" << "negatives.dat";
    trainingAgms << "-numPos" << QString::number(numPosPerStage);
    trainingAgms << "-numNeg" << QString::number(negativeSampleList.length());
    trainingAgms << "-numStages" << QString::number(numStages);
    trainingAgms << "-data" << folderName;
    trainingAgms << "-mode" << "ALL";
    trainingAgms << "-w" << QString::number(w);
    trainingAgms << "-h" << QString::number(h);
    trainingAgms << "-precalcValBufSize" << "512";
    trainingAgms << "-precalcIdxBufSize" << "512";
    trainingAgms << "-minHitRate" << QString::number(minHitRate);
    trainingAgms << "-maxFalseAlarmRate" << QString::number(maxFalseAlarmRate);

    // TODO
    //      trainingAgms << "-bt" << "";
    //      trainingAgms << "-weightTrimRate" << "";
    //      trainingAgms << "-maxDepth" << "";
    //      trainingAgms << "-maxWeakCount" << "";

    m_vjProcess->start("./opencv_traincascade", trainingAgms);
}

void TrainingSystem::testVJ(QString posImageFolder,
                            QString negImageFolder,
                            int w,
                            int h)
{
    QStringList posImageList =  getAllFilesOfDir(posImageFolder);
    QStringList negImageList =  getAllFilesOfDir(negImageFolder);

    generateImageDB("tests.dat", posImageList, false);

    m_vjClassifier.setParams(1.25, 1, QSize(w, h));

    int positiveCount = posImageList.count();
    int negativeCount = negImageList.count();

    emit testSampleCountChanged(positiveCount, negativeCount);

    int truePositive = 0;
    int falsePositive = 0;

    float tpr = 0.0;
    float fpr = 0.0;

    for (int i = 0; i < posImageList.count(); i++)
    {
        QString imgFile = posImageList.at(i);
        emit testImageChanged(imgFile);

        cv::waitKey(30);

        cv::Mat frame = cv::imread(imgFile.toUtf8().data(), cv::IMREAD_UNCHANGED);   // Read the file
        if (!frame.data)
        {
            qDebug() <<  "Could not open or find the image.";
        }
        frame.convertTo(frame, CV_8UC1);
        ViolaJonesClassifier::VJDetection detections = m_vjClassifier.detectPenguins(frame);

        if (detections.weights.count() > 0)
        {
            truePositive++;
            tpr = (float)truePositive / (float)positiveCount;
        }

        emit testTPRChanged(tpr);
    }

    for (int i = 0; i < negImageList.count(); i++)
    {
        QString imgFile = negImageList.at(i);
        emit testImageChanged(imgFile);

        cv::waitKey(30);

        cv::Mat frame = cv::imread(imgFile.toUtf8().data(), cv::IMREAD_UNCHANGED);   // Read the file
        ViolaJonesClassifier::VJDetection detections = m_vjClassifier.detectPenguins(frame);

        if (detections.weights.count() > 0)
        {
            falsePositive++;
            fpr = (float)falsePositive / (float)negativeCount;
        }

        emit testFPRChanged(fpr);
    }

    qDebug() << "TPR: " << tpr;
    qDebug() << "FPR: " << fpr;
    qDebug() << "TestVJ done.";
}

void TrainingSystem::trainRF(int w, int h)
{
    qDebug() << "TrainingSystem::trainRF started";
    QString folderName = QString("cb_classifier_%1x%2").arg(QString::number(w)).arg(QString::number(h));

    QString posFolder
            = QString("/Users/apple/Desktop/Courses/Penguin/training_data/positives/frontal_%1x%2")
            .arg(QString::number(w))
            .arg(QString::number(h));
    QStringList positiveSampleList =  getAllFilesOfDir(posFolder);
    QStringList negativeSampleList =  getAllFilesOfDir("/Users/apple/Desktop/Courses/Penguin/training_data/old_negatives");

    // Create output dir
    QString dirPath = QDir::currentPath() + QDir::separator() + folderName;
    QDir dir(dirPath);
    if (dir.exists())
    {
        bool ret = dir.removeRecursively();
        if (!ret)
        {
            qDebug() << "Can't clean the old dir(cb_classifier)";
        }
    }
    bool res = dir.mkdir(dirPath);
    if (!res)
    {
        qDebug() << "Can't create the training dir(cb_classifier)";
        return;
    }

    QFile fvFile(dirPath + QDir::separator() + QString("cb.classifier"));
    if (!fvFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        qDebug() << "Can't create the cb.classifier file.";
        return;
    }

    // Generate feature vectors
    int posFVLen = -1;
    int negFVLen = -1;

    int positiveCount = positiveSampleList.count();
    int negativeCount = negativeSampleList.count();

    for (int i = 0; i < positiveSampleList.length(); i++)
    {
        qDebug() << "Extracting feature vector from #" << i+1 << "positive sample";
        QString imgPath = positiveSampleList.at(i);
        cv::Mat fv = m_randomForest.computeFeatureVectors(imgPath, w, h);

        if (-1 == posFVLen)
        {
            posFVLen = fv.cols;
        } else {
            if (posFVLen != fv.cols)
                qFatal("Feature vector length within Positive Samples should be the same.");
        }

        QByteArray data;
        for (int j = 0; j < fv.cols; j++)
        {
            data.append(QString::number(fv.at<float>(j)));
            data.append(",");
        }
        data.append("1\n");   // positive label
        fvFile.write(data);
    }

    for (int i = 0; i < negativeSampleList.length(); i++)
    {
        qDebug() << "Extracting feature vector from #" << i+1 << "negative sample";
        QString imgPath = negativeSampleList.at(i);
        cv::Mat fv = m_randomForest.computeFeatureVectors(imgPath, w, h);
        if (-1 == negFVLen)
        {
            negFVLen = fv.cols;
        } else {
            if (negFVLen != fv.cols)
            {
                qDebug("Feature vector length within Negative Samples should be the same, sampe abandoned.");
                continue;
            }
        }

        QByteArray data;
        for (int j = 0; j < fv.cols; j++)
        {
            data.append(QString::number(fv.at<float>(j)));
            data.append(",");
        }
        data.append("0\n");   // negative label
        fvFile.write(data);
    }

    if (posFVLen != negFVLen)
    {
        qFatal("Feature vector length of Positive and Negative Samples should be the same.");
    }

    fvFile.close();

    qDebug() << "Start training Random Forest";
    // Load and train random forest classifier
    m_randomForest.setWorkSize(QSize(w, h));
    m_randomForest.setNumOfFeatures(posFVLen);
    m_randomForest.train(QString("cb_classifier_%1x%2/cb.classifier").arg(w).arg(h), positiveCount + negativeCount, m_randomForest.numOfFeatures());

    // training error
//    m_randomForest.test(QString("cb_classifier_%1x%2/cb.classifier").arg(w).arg(h), positiveCount + negativeCount, m_randomForest.numOfFeatures());

    qDebug() << "TrainingSystem::trainRF finished";
}

void TrainingSystem::testRF(QString posImageFolder,
                            QString negImageFolder)
{
    QStringList posImageList =  getAllFilesOfDir(posImageFolder);
    QStringList negImageList =  getAllFilesOfDir(negImageFolder);
    int positiveCount = posImageList.count();
    int negativeCount = negImageList.count();

    emit testSampleCountChanged(positiveCount, negativeCount);

    int truePositive = 0;
    int falsePositive = 0;

    float tpr = 0.0;
    float fpr = 0.0;

    int w = m_randomForest.workSize().width();
    int h = m_randomForest.workSize().height();

    for (int i = 0; i < posImageList.count(); i++)
    {
        QString imgFile = posImageList.at(i);
        emit testImageChanged(imgFile);

        cv::waitKey(10);

        cv::Mat fv = m_randomForest.computeFeatureVectors(imgFile, w, h);
        float ret = m_randomForest.predict(fv);

        if (fabs(ret - 1) < FLT_EPSILON)
        {
            truePositive++;
            tpr = (float)truePositive / (float)positiveCount;
        }

        emit testTPRChanged(tpr);
    }

    for (int i = 0; i < negImageList.count(); i++)
    {
        QString imgFile = negImageList.at(i);
        emit testImageChanged(imgFile);

        cv::waitKey(10);

        cv::Mat fv = m_randomForest.computeFeatureVectors(imgFile, w, h);
        float ret = m_randomForest.predict(fv);

        // if pridict incorrect
        if (fabs(ret - 0) >= FLT_EPSILON)
        {
            falsePositive++;
            fpr = (float)falsePositive / (float)negativeCount;
        }
        emit testFPRChanged(fpr);
    }

    qDebug() << "TPR: " << tpr;
    qDebug() << "FPR: " << fpr;
    qDebug() << "TestRF done.";
}

void TrainingSystem::handleTrainingProcessError()
{
    qDebug() << m_vjProcess->readAllStandardError();
}

void TrainingSystem::handleTrainingProcessMsg()
{
    qDebug() << m_vjProcess->readAllStandardOutput();

    // TODO: parse

    //    m_vjProcess->deleteLater();
}

void TrainingSystem::handleTrainingProcessFinished(int exitCode)
{
    qDebug() << "Process exit: " << exitCode;
}

void TrainingSystem::generateImageDB(QString dbName, QStringList fileList, bool pathOnly)
{
    qDebug() << "TrainingSystem::generateImageDB";
    QFile dbFile(dbName);
    if (!dbFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return;

    for (int i = 0; i < fileList.length(); i++)
    {
        QString filePath = fileList.at(i);
        QImage img(filePath);
        if (img.isNull())
            continue;

        QByteArray data;
        data.append(filePath);
        if (!pathOnly)
        {
            int w = img.size().width();
            int h = img.size().height();

            data.append("  ");
            data.append(QString::number(NUM_OBJECT_1));
            data.append("  ");
            data.append("0");
            data.append(" ");
            data.append("0");
            data.append(" ");
            data.append(QString::number(w));
            data.append(" ");
            data.append(QString::number(h));
        }

        if (i != (fileList.length() - 1))
            data.append("\n");
        dbFile.write(data);
    }

    dbFile.close();
}

void TrainingSystem::generateMoreSamples(int targetNum,
                                         int targetWidth,
                                         int targetHeight,
                                         QStringList fileList)
{
    qDebug() << "TrainingSystem::generateMoreSamples";
    int num = fileList.length();
    if (0 == num)
    {
        return;
    }

    QString program = "./opencv_createsamples";
    QStringList arguments;
    arguments << "-w" << QString::number(targetWidth);
    arguments << "-h" << QString::number(targetHeight);
    arguments << "-bg" << "negatives.dat";
    arguments << "-baseFormatSave";

    QProcess *myProcess = new QProcess(this);

    if (num >= targetNum)
    {
        qDebug() << "Target number <= Current number, use original images";

        generateImageDB("positives.dat", fileList, false);

        arguments << "-vec" << "samples.vec";
        arguments << "-info" << "positives.dat";
        arguments << "-num" << QString::number(targetNum);
        myProcess->start(program, arguments);

    } else {
        arguments << "-maxxangle" << "0.1";
        arguments << "-maxyangle" << "0.1";
        arguments << "-maxzangle" << "0.1";
        arguments << "-maxidev" << "80";
        arguments << "-bgthresh" << "10";
        arguments << "-maxzangle" << "0.1";

        int loop = targetNum / num;
        int remain = targetNum % num;

        QString dirPath = QDir::currentPath() + "/tempvectors";
        QDir dir(dirPath);
        if (dir.exists())
        {
            bool ret = dir.removeRecursively();
            if (!ret)
            {
                qDebug() << "Can't clean the old dir(tempvectors)";
            }
        }
        bool res = dir.mkdir(dirPath);
        if (!res)
        {
            qDebug() << "Can't create the training dir(tempvectors)";
            return;
        }

        for (int i = 0; i < num; i++)
        {
            QString imgPath = fileList.at(i);
            int generateNum = (i < remain) ? loop + 1: loop;

            QString tmpName = QString("./tempvectors/%1.vec").arg(i);

            arguments << "-vec" << tmpName;
            arguments << "-img" << imgPath;
            arguments << "-num" << QString::number(generateNum);
            myProcess->start(program, arguments);
            myProcess->waitForFinished();
        }

        QString cmd = "find tempvectors/ -name '*.vec' > tempvectors.dat";
        myProcess->start("/bin/bash", QStringList() << "-c" << cmd);
        myProcess->waitForFinished();

        myProcess->start("./mergevec", QStringList() << "tempvectors.dat" << "samples.vec");
        myProcess->waitForFinished();

        myProcess->deleteLater();
    }
}
