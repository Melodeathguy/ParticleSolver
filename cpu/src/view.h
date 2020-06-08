#ifndef VIEW_H
#define VIEW_H

#include "simulation.h"

#include <QTime>
#include <QTimer>
#include <QElapsedTimer>
#include <QMutex>

class View : public QGLWidget
{
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();


private:
    QElapsedTimer time;
    QTimer timer;
    double fps, tickTime;
    double scale;
    double aspect;
    bool timestepMode;
    bool velocityRenderMode;
    bool allowInteraction;

    QMutex *tickLock = new QMutex(QMutex::NonRecursive);
    bool m_busy;

    glm::ivec2 dimensions;
    Simulation sim;
    SimulationType current;

    int m_frameNo;

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void renderImage(QString fileName);

private slots:
    void tick();
};

#endif // VIEW_H

