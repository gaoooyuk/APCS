#include "trainingsystem.h"
#include "utility.h"

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

void TrainingSystem::train(int w,
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

    QString folderName = QString("classifier_%1x%2").arg(QString::number(w)).arg(QString::number(h));
    if (overwrite)
    {
        QStringList positiveSampleList =  getAllFilesOfDir("/Users/apple/Desktop/Courses/Penguin/training_images/positives/frontal_12x36");
        QStringList negativeSampleList =  getAllFilesOfDir("/Users/apple/Desktop/Courses/Penguin/prototype/training/training/negatives");
        generateImageDB("negatives.dat", negativeSampleList);
        generateMoreSamples(5000, w, h, positiveSampleList);

        // Create output dir
        QString dirPath = QDir::currentPath() + QDir::separator() + folderName;
        QDir dir(dirPath);
        if (dir.exists())
        {
            bool ret = dir.removeRecursively();
            if (!ret)
            {
                qDebug() << "Can't clean the old dir(classifier)";
            }
        }
        bool res = dir.mkdir(dirPath);
        if (!res)
        {
            qDebug() << "Can't create the training dir(classifier)";
            return;
        }
    }

    QStringList trainingAgms;
    trainingAgms << "-vec" << "samples.vec";
    trainingAgms << "-bg" << "negatives.dat";
    trainingAgms << "-numPos" << "4000";
    trainingAgms << "-numNeg" << "5000";
    trainingAgms << "-numStages" << QString::number(numStages);
    trainingAgms << "-data" << folderName;
    trainingAgms << "-mode" << "ALL";
    trainingAgms << "-w" << QString::number(w);
    trainingAgms << "-h" << QString::number(h);
    trainingAgms << "-precalcValBufSize" << "512";
    trainingAgms << "-precalcIdxBufSize" << "512";

    // TODO
    //      trainingAgms << "-bt" << "";
    trainingAgms << "-minHitRate" << QString::number(minHitRate);
    trainingAgms << "-maxFalseAlarmRate" << QString::number(maxFalseAlarmRate);
    //      trainingAgms << "-weightTrimRate" << "";
    //      trainingAgms << "-maxDepth" << "";
    //      trainingAgms << "-maxWeakCount" << "";

    m_vjProcess->start("./opencv_traincascade", trainingAgms);
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
        int w = img.size().width();
        int h = img.size().height();
        data.append(filePath);
        if (!pathOnly)
        {
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
    int num = fileList.length();
    if (0 == num)
    {
        return;
    }

    if (num >= targetNum)
    {
        qDebug() << "Target number <= Current number";
    }

    QString program = "./opencv_createsamples";
    QStringList arguments;
    arguments << "-maxxangle" << "0.1";
    arguments << "-maxyangle" << "0.1";
    arguments << "-maxzangle" << "0.1";
    arguments << "-maxidev" << "80";
    arguments << "-bgthresh" << "10";
    arguments << "-maxzangle" << "0.1";
    arguments << "-w" << QString::number(targetWidth);
    arguments << "-h" << QString::number(targetHeight);
    arguments << "-bg" << "negatives.dat";

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

    QProcess *myProcess = new QProcess(this);

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

    QString cmd = "find tempvectors/ -name '*.vec' > samples.dat";
    myProcess->start("/bin/bash", QStringList() << "-c" << cmd);
    myProcess->waitForFinished();

    myProcess->start("./mergevec", QStringList() << "samples.dat" << "samples.vec");
    myProcess->waitForFinished();

    myProcess->deleteLater();
}
