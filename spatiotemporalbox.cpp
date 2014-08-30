#include "spatiotemporalbox.h"

#include <QGLWidget>
#include <QVector3D>
#include <QStringBuilder>

#define M_PI_DIV180 0.01745329251994329576

SpatioTemporalBox::SpatioTemporalBox(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setRenderTarget(QQuickPaintedItem::FramebufferObject);

    m_rotationX = 0;
    m_rotationY = 0;
    m_zoomValue = -8.0f;
    m_timeScale = 10;

    m_isDrawLBPoints = false;
    m_isDrawSTPoints = true;
    m_isHighlightClusters = false;
    m_isDrawFilteringSTPoints = false;
    m_isDrawClusterEigenLine = false;
}

QVector3D perp(const QVector3D &v)
{
    qreal min = fabs(v.x());
    QVector3D cardinalAxis(1, 0, 0);

    if (fabs(v.y()) < min) {
        min = fabs(v.y());
        cardinalAxis = QVector3D(0, 1, 0);
    }

    if (fabs(v.z()) < min) {
        cardinalAxis = QVector3D(0, 0, 1);
    }

    return QVector3D::crossProduct(v, cardinalAxis);
}

void drawCone(const QVector3D &d,
              const QVector3D &a,
              const qreal h,
              const qreal rd,
              const int n)
{
    QVector3D c = a + (-d * h);
    QVector3D e0 = perp(d);
    QVector3D e1 = QVector3D::crossProduct(e0, d);
    qreal angInc = 360.0 / n * M_PI_DIV180;

    // calculate points around directrix
    QVector<QVector3D> pts;
    for (int i = 0; i < n; ++i)
    {
        qreal rad = angInc * i;
        QVector3D p = c + (((e0 * cos(rad)) + (e1 * sin(rad))) * rd);
        pts.push_back(p);
    }

    // draw cone circle
    for (int i = 0; i < n; ++i)
    {
        if ((n-1)/4 == i || 3*(n-1)/4 == i)
        {
            glBegin(GL_LINE_STRIP);
            glVertex3f(a.x(), a.y(), a.z());
            glVertex3f(pts[i].x(), pts[i].y(), pts[i].z());
            glEnd();
        }
    }

    // draw cone bottom
    glBegin(GL_LINE_STRIP);
    glVertex3f(c.x(), c.y(), c.z());
    for (int i = n-1; i >= 0; --i)
    {
        glVertex3f(pts[i].x(), pts[i].y(), pts[i].z());
    }
    glEnd();
}

void SpatioTemporalBox::paint(QPainter *painter)
{
    painter->setRenderHints(QPainter::Antialiasing, true);

    painter->beginNativePainting();

    init();
    drawSTBox();
    drawSTPoints();
    drawSTClusters();
//    drawSTCones();
    drawClusterEigenLine();
//    drawSTLineSegments();

    painter->endNativePainting();
}

float SpatioTemporalBox::rotationX()
{
    return m_rotationX;
}

void SpatioTemporalBox::setRotationX(const float angel)
{
    if (m_rotationX != angel)
    {
        m_rotationX = angel;
        emit rotationXChanged();

        update();
    }
}

float SpatioTemporalBox::rotationY()
{
    return m_rotationY;
}

void SpatioTemporalBox::setRotationY(const float angel)
{
    if (m_rotationY != angel)
    {
        m_rotationY = angel;
        emit rotationYChanged();

        update();
    }
}

void SpatioTemporalBox::zoomIn()
{
    m_zoomValue += 0.1f;
    update();
}

void SpatioTemporalBox::zoomOut()
{
    m_zoomValue -= 0.1f;
    update();
}

void SpatioTemporalBox::timeScaleUp()
{
    m_timeScale += 2;
    update();
}

void SpatioTemporalBox::timeScaleDown()
{
    m_timeScale -= 2;
    m_timeScale = m_timeScale < 0 ? 0 : m_timeScale;
    update();
}

bool SpatioTemporalBox::isDrawLBPoints() const
{
    return m_isDrawLBPoints;
}

void SpatioTemporalBox::setIsDrawLBPoints(const bool is)
{
    if (m_isDrawLBPoints != is)
    {
        m_isDrawLBPoints = is;
        emit isDrawLBPointsChanged();

        update();
    }
}

bool SpatioTemporalBox::isDrawSTPoints() const
{
    return m_isDrawSTPoints;
}

void SpatioTemporalBox::setIsDrawSTPoints(const bool is)
{
    if (m_isDrawSTPoints != is)
    {
        m_isDrawSTPoints = is;
        emit isDrawSTPointsChanged();

        update();
    }
}

bool SpatioTemporalBox::isHighlightClusters() const
{
    return m_isHighlightClusters;
}

void SpatioTemporalBox::setIsHighlightClusters(const bool is)
{
    if (m_isHighlightClusters != is)
    {
        m_isHighlightClusters = is;
        emit isHighlightClustersChanged();

        update();
    }
}

bool SpatioTemporalBox::isDrawFilteringSTPoints() const
{
    return m_isDrawFilteringSTPoints;
}

void SpatioTemporalBox::setIsDrawFilteringSTPoints(const bool is)
{
    if (m_isDrawFilteringSTPoints != is)
    {
        m_isDrawFilteringSTPoints = is;
        emit isDrawFilteringSTPointsChanged();

        update();
    }
}

bool SpatioTemporalBox::isDrawClusterEigenLine() const
{
    return m_isDrawClusterEigenLine;
}

void SpatioTemporalBox::setIsDrawClusterEigenLine(const bool is)
{
    if (m_isDrawClusterEigenLine != is)
    {
        m_isDrawClusterEigenLine = is;
        emit isDrawClusterEigenLineChanged();

        update();
    }
}

void SpatioTemporalBox::updateSpatioTemporalPoints(QObject *stPoints)
{
    m_stPoints.clear();
    QVariantList vl = stPoints->property("stPoints").toList();

    foreach (QVariant var, vl)
    {
        STPoint stPoint = var.value<STPoint>();
        m_stPoints.append(stPoint);
    }

    update();
}

void SpatioTemporalBox::updateSpatioTemporalFilteredPoints(QObject *stFilteredPoints)
{
    m_stFilteredPoints.clear();
    QVariantList vl = stFilteredPoints->property("stFilteredPoints").toList();

    foreach (QVariant var, vl)
    {
        STPoint stPoint = var.value<STPoint>();
        m_stFilteredPoints.append(stPoint);
    }

    update();
}

void SpatioTemporalBox::updateSpatioTemporalCones(QObject *stCones)
{
    m_stCones.clear();
    QVariantList vl = stCones->property("stCones").toList();

    foreach (QVariant var, vl)
    {
        STCone stCone = var.value<STCone>();
        m_stCones.append(stCone);
    }

    update();
}

void SpatioTemporalBox::updateSpatioTemporalLabelingPoints(QObject *lbPoints)
{
    m_lbPoints.clear();
    QVariantList vl = lbPoints->property("lbPoints").toList();

    foreach (QVariant var, vl)
    {
        STPoint stPoint = var.value<STPoint>();
        m_lbPoints.append(stPoint);
    }

    update();
}

void SpatioTemporalBox::updateSpatioTemporalCluster(QObject *clusterInfo)
{
    m_clusters.clear();
    QVariantList vl = clusterInfo->property("clusterInfo").toList();

    foreach (QVariant var, vl)
    {
        STCluster cluster = var.value<STCluster>();
        m_clusters.append(cluster);
    }

    update();
}

void SpatioTemporalBox::updateSpatioTemporalLineSegments(QObject *lineSegments)
{
    m_lineSegments.clear();
    QVariantList vl = lineSegments->property("lineSegments").toList();

    foreach (QVariant var, vl)
    {
        STLine line = var.value<STLine>();
        m_lineSegments.append(line);
    }

    update();
}

void SpatioTemporalBox::loadSpatioTemporalLabelingPoints(const QString fileName)
{
    m_lbPoints.clear();
    int dim = 3;
    QString filePath = fileName;
    QFile file(filePath.replace("file:///", "/"));
    if (file.open(QFile::ReadOnly | QIODevice::Text))
    {
        while (!file.atEnd())
        {
            QByteArray lineData = file.readLine();
            lineData.replace("\n", "");
            char ch = ' ';
            QList<QByteArray> list = lineData.split(ch);

            if (dim == list.length())
            {
                STPoint stPoint;
                stPoint.x = list.at(0).toFloat();
                stPoint.y = list.at(1).toFloat();
                stPoint.t = list.at(2).toInt();

                m_lbPoints.append(stPoint);
            }
        }
    }

    file.close();

    update();
}

void SpatioTemporalBox::init()
{
    glViewport(0, 0, (GLint)width(), (GLint)height());

    /* create viewing cone with near and far clipping planes */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -1.0, 1.0, 5.0, 50.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, m_zoomValue); //move along z-axis
    glRotatef(m_rotationX, 1.0, 0.0, 0.0); // around x-axis
    glRotatef(m_rotationY, 0.0, 1.0, 0.0); // around y-axis

    // Enable smooth drawing
    glShadeModel(GL_SMOOTH);   // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLineWidth(3.0f);
}

void SpatioTemporalBox::drawSTBox()
{
    // Draw 3D cube
    glBegin(GL_LINES);
    glColor3f(1.0,1.0,1.0);
//    glColor3f(0.0, 0.0, 0.0);

    // front
    glVertex3f(1.0,1.0,1.0);
    glVertex3f(-1.0,1.0,1.0);

    glVertex3f(-1.0,-1.0,1.0);
    glVertex3f(1.0,-1.0,1.0);

    glVertex3f(-1.0,1.0,1.0);
    glVertex3f(-1.0,-1.0,1.0);

    glVertex3f(1.0,1.0,1.0);
    glVertex3f(1.0,-1.0,1.0);

    // back
    glVertex3f(-1.0,1.0,-1.0);
    glVertex3f(1.0,1.0,-1.0);

    glVertex3f(-1.0,-1.0,-1.0);
    glVertex3f(1.0,-1.0,-1.0);

    glVertex3f(-1.0,1.0,-1.0);
    glVertex3f(-1.0,-1.0,-1.0);

    glVertex3f(1.0,1.0,-1.0);
    glVertex3f(1.0,-1.0,-1.0);

    // other edges
    glVertex3f(-1.0,1.0,-1.0);
    glVertex3f(-1.0,1.0,1.0);

    glVertex3f(1.0,1.0,-1.0);
    glVertex3f(1.0,1.0,1.0);

    glVertex3f(-1.0,-1.0,-1.0);
    glVertex3f(-1.0,-1.0,1.0);

    glVertex3f(1.0,-1.0,-1.0);
    glVertex3f(1.0,-1.0,1.0);

    glEnd();
}

void SpatioTemporalBox::drawSTPoints()
{
    // Draw points
    float quadratic[] =  {0.0f, 0.0f, 0.01f};
    glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic);
    float maxSize = 0.0f;
    glGetFloatv(GL_POINT_SIZE_MAX_ARB, &maxSize);
    glPointSize(maxSize);
    glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, 2.0f);
    glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, 1.0f);
    glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

    glEnable(GL_POINT_SPRITE_ARB);
    glBegin(GL_POINTS);

    if (m_isDrawSTPoints)
    {
        glColor3f(1.0, 1.0, 1.0);
        //    glColor3f(0.0, 0.0, 0.0);

        QList<STPoint> drawingList;
        if (m_isDrawFilteringSTPoints)
        {
            drawingList = m_stFilteredPoints;
        } else {
            drawingList = m_stPoints;
        }

        for (int i = 0; i < drawingList.length(); i++)
        {
            STPoint p = drawingList.at(i);
            float x = 2.0f * p.x/1280.0f - 1.0f;
            float y = 2.0f * p.y/600.0f - 1.0f;
            float t = 1.0 - p.t/12800.0f * (float)m_timeScale;
            glVertex3f(x, y, t);
        }
    }

    if (m_isDrawLBPoints)
    {
        // draw manual labeling points
        glColor3f(1.0, 0.0, 0.0);
        for (int j = 0; j < m_lbPoints.length(); j++)
        {
            STPoint p = m_lbPoints.at(j);
            float x = 2.0f * p.x/1280.0f - 1.0f;
            float y = 2.0f * p.y/600.0f - 1.0f;
            float t = 1.0 - p.t/1280.0f * (float)m_timeScale;
            glVertex3f(x, y, t);
        }
    }

    glEnd();
    glDisable(GL_POINT_SPRITE_ARB);
}

void SpatioTemporalBox::drawSTClusters()
{
    if (!m_isHighlightClusters)
        return;

    // Draw points in clusters
    float quadratic[] =  {0.0f, 0.0f, 0.01f};
    glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic);
    float maxSize = 0.0f;
    glGetFloatv(GL_POINT_SIZE_MAX_ARB, &maxSize);
    glPointSize(maxSize);
    glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, 2.0f);
    glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, 1.0f);
    glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

    glEnable(GL_POINT_SPRITE_ARB);
    glBegin(GL_POINTS);
    // draw points for each cluster
    for (int i = 0; i < m_clusters.length(); i++)
    {
        STCluster cluster = m_clusters.at(i);
        QColor color = cluster.color;

        int clusterSize = cluster.points.length();
        for (int j = 0; j < clusterSize; j++)
        {
            STPoint p = cluster.points.at(j);
            glColor3f(color.redF(), color.greenF(), color.blueF());
            float x = 2.0f * p.x/1280.0f - 1.0f;
            float y = 2.0f * p.y/600.0f - 1.0f;
            float t = 1.0 - p.t/12800.0f * (float)m_timeScale;
            glVertex3f(x, y, t);
        }
    }
    glEnd();
    glDisable(GL_POINT_SPRITE_ARB);
}

void SpatioTemporalBox::drawSTLineSegments()
{
    for (int i = 0; i < m_lineSegments.length(); i++)
    {
        STLine line = m_lineSegments.at(i);
        cv::Point3f start = line.start;
        cv::Point3f end = line.end;
        glBegin(GL_LINES);
        float x1 = 2.0f * start.x/1280.0f - 1.0f;
        float y1 = 2.0f * start.y/600.0f - 1.0f;
        float z1 = 1.0f - start.z/12800.0f * (float)m_timeScale;
        float x2 = 2.0f * end.x/1280.0f - 1.0f;
        float y2 = 2.0f * end.y/600.0f - 1.0f;
        float z2 = 1.0f - end.z/12800.0f * (float)m_timeScale;
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
        glEnd();
    }
}

void SpatioTemporalBox::drawSTCones()
{  
    // draw cones
    //    for (int i = 0; i < m_clusters.length(); i++)
    //    {
    //        STCluster cluster = m_clusters.at(i);
    //        int clusterSize = cluster.points.length();
    //        for (int j = 0; j < clusterSize; j++)
    //        {
    //            STPoint p = cluster.points.at(j);
    //            float x = 2.0f * p.x/1280.0f - 1.0f;
    //            float y = 2.0f * p.y/600.0f - 1.0f;
    //            float t = 1.0 - p.t/12800.0f * (float)m_timeScale;

    //            float t0 = t;
    //            float t1 = 1.0 - (p.t+1)/12800.0f * (float)m_timeScale;

    //            // draw cone
    //            glEnable(GL_BLEND);
    //            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //            glColor4f(1.0f, 1.0f, 1.0f, 0.5f);

    //            QVector3D d(0, 0, t1);
    //            QVector3D a(x, y, t0);
    //            qreal h = 0.1;
    //            qreal rd = 0.05;
    //            int n = 360;
    //            drawCone(d, a, h, rd, n);
    //        }
    //    }

    // draw cones
//    for (int i = 0; i < m_stPoints.length(); i++)
//    {
//        STPoint p = m_stPoints.at(i);
//        float x = 2.0f * p.x/1280.0f - 1.0f;
//        float y = 2.0f * p.y/600.0f - 1.0f;
//        float t = 1.0 - p.t*10/12800.0f * (float)m_timeScale;

//        float t0 = t;
//        float t1 = 1.0 - (p.t*10+1)/12800.0f * (float)m_timeScale;

//        // draw cone
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);

//        QVector3D d(0, 0, t1);
//        QVector3D a(x, y, t0);
//        qreal h = 0.1;
//        qreal rd = 0.05;
//        int n = 360;
//        drawCone(d, a, h, rd, n);
//    }

    for (int i = 0; i < m_stCones.length(); i++)
    {
        STCone cone = m_stCones.at(i);
        QList<STPoint> stPoints = cone.points;
        for (int j = 0; j < stPoints.length(); j++)
        {
            if (j < (stPoints.length() - 1))
            {
                STPoint start = stPoints.at(j);
                STPoint end = stPoints.at(j+1);
                glBegin(GL_LINES);

                float x1 = 2.0f * start.x/1280.0f - 1.0f;
                float y1 = 2.0f * start.y/600.0f - 1.0f;
                float z1 = 1.0f - start.t/12800.0f * (float)m_timeScale;
                float x2 = 2.0f * end.x/1280.0f - 1.0f;
                float y2 = 2.0f * end.y/600.0f - 1.0f;
                float z2 = 1.0f - end.t/12800.0f * (float)m_timeScale;

                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);
                glEnd();
            }
        }
    }
}

void SpatioTemporalBox::drawClusterEigenLine()
{
    if (!isDrawClusterEigenLine())
        return;

    for (int i = 0; i < m_clusters.length(); i++)
    {
        STCluster cluster = m_clusters.at(i);
        STLine line = cluster.eigenLine;
        cv::Point3f start = line.start;
        cv::Point3f end = line.end;
        glBegin(GL_LINES);
        QColor color = cluster.color;
        glColor3f(color.redF(), color.greenF(), color.blueF());
        float x1 = 2.0f * start.x/1280.0f - 1.0f;
        float y1 = 2.0f * start.y/600.0f - 1.0f;
        float z1 = 1.0f - start.z/12800.0f * (float)m_timeScale;
        float x2 = 2.0f * end.x/1280.0f - 1.0f;
        float y2 = 2.0f * end.y/600.0f - 1.0f;
        float z2 = 1.0f - end.z/12800.0f * (float)m_timeScale;
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
        glEnd();
    }
}
