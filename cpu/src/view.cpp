#include "view.h"
#include <QApplication>
#include <QKeyEvent>

View::View(QWidget *parent) : QGLWidget(parent)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor since this is a fullscreen app
    // setCursor(Qt::BlankCursor);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // The game loop is implemented using a timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));

    fps = 120;
    scale = 50;
    m_frameNo = 0;
    m_busy = false;
    tickStep = 0.0111111111; // for 90 FPS video
    currentTick = 0.0;

    m_trajectory = 0;
    m_trajectorySteps = 600;
    m_maxTrajectories = 1000 + 100 + 100 + 10; // 1000 Tr, 100 Val, 100 Test, 100 reserve if contains NaN Particle values

    // TODO: Make automatic!
    double screenHeight = 768;//1050.;
    double screenWidth = 1366;//1680.;
    aspect = screenWidth / screenHeight;
    tickTime = 0.0;
    timestepMode = false;
    velocityRenderMode = false;
    current = SAND_DIGGER_TEST;
    allowInteraction = false;
    exportSimulationData = true;
    exporter = new PlyExporter("./export/");

}

View::~View()
{
    delete exporter;
}

void View::initializeGL()
{
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.

    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
    //time.start();
    timer.start(1000. / 120);


    // Center the mouse, which is explained more in mouseMoveEvent() below.
    // This needs to be done here because the mouse may be initially outside
    // the fullscreen window and will not automatically receive mouse move
    // events. This occurs if there are two monitors and the mouse is on the
    // secondary monitor.
//    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void View::paintGL()
{
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-scale * aspect, scale * aspect, -scale, scale, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (velocityRenderMode){
        sim.drawVelocityField();
    }
    else{
       sim.draw();
    }

    glColor3f(1,1,1);
    renderText(10, 20, "FPS: " + QString::number((double) (fps)), this->font());
    renderText(10, 40, "# Particles: " + QString::number(sim.getNumParticles()), this->font());
    renderText(10, 60, "Kinetic Energy: " + QString::number(sim.getKineticEnergy()), this->font());
    renderText(10, 80, "Trajectory #: " + QString::number(m_trajectory), this->font());
    renderText(10, 100, "Frame #: " + QString::number(m_frameNo), this->font());
}

void View::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    dimensions.x = w;
    dimensions.y = h;

    sim.resize(glm::ivec2(scale, scale));
}



void View::mousePressEvent(QMouseEvent *event)
{
    if (!allowInteraction) return;

    glm::dvec2 screen(event->x(), height() - event->y());
    glm::dvec2 aspectScale = glm::dvec2((double)(scale * aspect), (double) scale);
    glm::dvec2 world = (aspectScale * 2.) * (screen / glm::dvec2(width(), height())) - aspectScale;
    sim.mousePressed(world);
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    if (!allowInteraction) return;

    glm::dvec2 screen(event->x(), height() - event->y());
    glm::dvec2 aspectScale = glm::dvec2((double)(scale * aspect), (double) scale);
    glm::dvec2 world = (aspectScale * 2.) * (screen / glm::dvec2(width(), height())) - aspectScale;
    this->sim.mouseMoved(world);
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    if (!allowInteraction) return;

    glm::dvec2 screen(event->x(), height() - event->y());
    glm::dvec2 aspectScale = glm::dvec2((double)(scale * aspect), (double) scale);
    glm::dvec2 world = (aspectScale * 2.) * (screen / glm::dvec2(width(), height())) - aspectScale;
    sim.mouseReleased(world);
}

void View::wheelEvent(QWheelEvent *event)
{
    if (!allowInteraction) {
        if (event->delta() > 0) {
            scale /= 1.2;
        } else {
            scale *= 1.2;
        }
        sim.resize(glm::ivec2(scale, scale));
        return;
    }
    sim.mouseWheelMoved(event->delta() / 2000.);
}

void View::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QApplication::quit();
    if (event->key() == Qt::Key_R) sim.init(current);
    if (event->key() == Qt::Key_T) timestepMode = !timestepMode;
    if (event->key() == Qt::Key_Space) tickTime = .01;
    if (event->key() == Qt::Key_Backspace) tickTime = -.01;
    if (event->key() == Qt::Key_C) sim.debug = !sim.debug;
    if (event->key() == Qt::Key_Tab) velocityRenderMode = !velocityRenderMode;

    if (event->key() == Qt::Key_1) {
        current = GRANULAR_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_2) {
        current = STACKS_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_3) {
        current = WALL_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_4) {
        current = PENDULUM_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_5) {
        current = ROPE_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_6) {
        current = FLUID_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_7) {
        current = FLUID_SOLID_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_8) {
        current = GAS_ROPE_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_9) {
        current = FRICTION_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_0) {
        current = WATER_BALLOON_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_N) {
        current = CRADLE_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_S) {
        current = SMOKE_OPEN_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_D) {
        current = SMOKE_CLOSED_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_Period) {
        current = SDF_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_V) {
        current = VOLCANO_TEST;
        sim.init(current);
    } else if (event->key() == Qt::Key_W) {
        current = WRECKING_BALL;
        sim.init(current);
    } else if (event->key() == Qt::Key_Q) {
        current = SAND_DIGGER_TEST;
        sim.init(current);
    }
}

void View::keyReleaseEvent(QKeyEvent *event)
{
}

void View::tick()
{
    bool stopping = false;
    tickLock->lock();
    if(m_busy){
        stopping = true;
    } else {
        m_busy = true;
    }
    //tickLock->unlock();
    if(stopping){return;}


    // Get the number of seconds since the last tick (variable update rate)
    double seconds = time.elapsed();

    // Trigger animations, hint methods in the animated body is called. Hints are reset at the simulation loop end.
    for (int i = 0; i < sim.m_animations.length(); i++){
        sim.m_animations[i]->tick(tickStep);
    }

    if (exportSimulationData){

        // This exporter uses hints, set by the animations to write the commands
        //exporter->writePoints(m_frameNo, sim.m_particles, 0, sim.m_bodies.at(0));

        exporter->writeAllPoints(this->currentTick, m_frameNo, sim.m_particles, 0, m_trajectory);
    }

    time.start();

    fps = 1000. / seconds;

    //std::cout << "FPS: " << fps << ", " << m_frameNo << ", "<<"\n";

    if (timestepMode) {
        if (tickTime != 0.0) {
            sim.tick(tickTime);
            tickTime = 0.0;
        }
    } else {
        this->currentTick += tickStep;
        sim.tick(tickStep); // 11.11MS -> 90 FPS
        m_frameNo += 1;
    }

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();

    // TODO: make more beautiful
    QString number = QString("%1").arg(m_frameNo + m_trajectory * m_trajectorySteps, 20, 10, QChar('0'));
    //renderImage("/home/stahl/tmp/frame"+ number +".jpg");

    if (m_frameNo >= m_trajectorySteps){
        m_frameNo = 0;
        currentTick = 0.0;
        m_trajectory += 1;
        if (m_trajectory == m_maxTrajectories){
            // TODO FIX!
            delete this;
        }
        sim.init(current);
    }

    //tickLock->lock();
    m_busy = false;
    tickLock->unlock();
}

void View::renderImage(QString fileName){
    //QPixmap image = QPixmap::grabWidget(this);
    QPixmap image = renderPixmap();
    if( !image.save( fileName, "JPG" ) )
    {
       std::cerr << "Error saving image." << std::endl;
    }
}
