#include "particleapp.h"
#include <cuda_runtime.h>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include "particlesystem.h"
#include "renderer.h"
#include "helper_math.h"

#define MAX_PARTICLES 20000 // (vbo size)
//#define NUM_PARTICLES 5000
//#define NUM_PARTICLES 4000
//#define NUM_PARTICLES 2025
//#define NUM_PARTICLES 1024
//#define NUM_PARTICLES 529
//#define NUM_PARTICLES 506
//#define NUM_PARTICLES 225
//#define NUM_PARTICLES 25
//#define NUM_PARTICLES 9
#define NUM_PARTICLES 0
#define PARTICLE_RADIUS 0.25f
//#define GRID_SIZE make_uint3(64, 64, 2) // 2D
#define GRID_SIZE make_uint3(64, 64, 64) // 3D
//#define GRID_SIZE make_uint3(4, 4, 4) // 3D

#include <random>

//#include "debugprinting.h"

ParticleApp::ParticleApp()
    : m_particleSystem(NULL),
      m_renderer(NULL),
      m_mouseDownL(false),
      m_mouseDownR(false),
      m_fluidEmmiterOn(false),
      m_timer(-1.f)
{
    m_particleSystem = new ParticleSystem(NUM_PARTICLES, PARTICLE_RADIUS, GRID_SIZE, MAX_PARTICLES, make_int3(-50, 0, -50), make_int3(50, 200, 50), 5);
    makeInitScene();
    m_renderer = new Renderer();
    m_renderer->createVAO(m_particleSystem->getCurrentReadBuffer(),
                          m_particleSystem->getParticleRadius());
}


ParticleApp::~ParticleApp()
{
    if (m_particleSystem)
        delete m_particleSystem;
    if (m_renderer)
        delete m_renderer;

    // cudaDeviceReset causes the driver to clean up all state. While
    // not mandatory in normal operation, it is good practice.  It is also
    // needed to ensure correct operation when the application is being
    // profiled. Calling cudaDeviceReset causes all profile data to be
    // flushed before the application exits
    cudaDeviceReset();
}

inline float frand()
{
    return rand() / (float) RAND_MAX;
}

void ParticleApp::makeInitScene()
{
#ifdef TWOD
    m_particleSystem->addParticleGrid(make_int3(-3, 0, 0), make_int3(3, 20, 0), 1.f, false);
#else
    m_particleSystem->addHorizCloth(make_int2(0, -3), make_int2(6,3), make_float3(.5f,5.f,.5f), make_float2(.3f, .3f), 3.f, false);
#endif
}


void ParticleApp::tick(float secs)
{
    if (m_fluidEmmiterOn && m_timer <= 0.f)
    {
//        m_particleSystem->setFluidToAdd(make_float3(frand() * 5.f, 2.f, frand() * 5.f), make_float3(1,0,0), 1.f, 1.5f);
        m_particleSystem->addFluid(make_int3(-1,0,-1), make_int3(1,1,1), 1.f, 1.f, make_float3(1,0,0));
        m_timer = 0.1f;
    }
    m_timer -= secs;

    m_particleSystem->update(secs);
    m_renderer->update(secs);
}


void ParticleApp::render()
{
    m_renderer->render(m_particleSystem->getColorIndex(), m_particleSystem->getColors());
}


void ParticleApp::mousePressed(QMouseEvent *e, float x, float y)
{
    if (e->button() == Qt::LeftButton)
    {
#ifdef TWOD
        float4 pos = m_renderer->raycast2XYPlane(x, y);
        m_particleSystem->mousePos = pos;
        m_particleSystem->setParticleToAdd(make_float3(pos), make_float3(10, 0, 0), 100.f);
#else
        m_particleSystem->setParticleToAdd(m_renderer->getEye(), m_renderer->getDir(x, y) * 30.f, 2.f);
#endif
        m_mouseDownL = true;
    }
    else if (e->button() == Qt::RightButton)
    {
//        m_renderer->mousePressed(e);
        m_mouseDownR = true;
    }

}

void ParticleApp::mouseReleased(QMouseEvent *e, float, float)
{
    if (e->button() == Qt::LeftButton)
    {
        m_particleSystem->mousePos = make_float4(-1.f);
        m_mouseDownL = false;
    }
    if (e->button() == Qt::RightButton)
    {
        m_mouseDownR = false;
    }
}


void ParticleApp::mouseMoved(QMouseEvent *e, float deltaX, float deltaY)
{
    m_renderer->mouseMoved(e, deltaX, deltaY);
}

void ParticleApp::mouseScrolled(QWheelEvent *)
{
//    m_renderer->mouseScrolled(e);
}


void ParticleApp::keyPressed(QKeyEvent *e)
{
    m_renderer->keyPressed(e);
}

void ParticleApp::keyReleased(QKeyEvent *e)
{
    bool resetVbo = true;
    float3 h, vec;
    float angle;


    switch (e->key())
    {
    case Qt::Key_1:
        delete m_particleSystem;
        m_particleSystem = new ParticleSystem(NUM_PARTICLES, PARTICLE_RADIUS, GRID_SIZE, MAX_PARTICLES, make_int3(-50, 0, -50), make_int3(50, 200, 50), 5);
        makeInitScene();
        break;
    case Qt::Key_2:
        delete m_particleSystem;
        m_particleSystem = new ParticleSystem(NUM_PARTICLES, PARTICLE_RADIUS, GRID_SIZE, MAX_PARTICLES, make_int3(-7, 0, -5), make_int3(7, 20, 5), 5);
        m_particleSystem->addFluid(make_int3(-7, 0, -5), make_int3(7, 5, 5), 1.f, 2.f, make_float3(frand(),frand(),frand()));
        m_particleSystem->addFluid(make_int3(-7, 5, -5), make_int3(7, 10, 5), 1.f, 3.f, make_float3(frand(),frand(),frand()));
//        m_particleSystem->addFluid(make_int3(-10, 23,-1), make_int3(0, 33, 1), 1.f, 2.f);
//        m_particleSystem->addFluid(make_int3(-0, 0, -5), make_int3(7, 10, 5), 1.f, 3.f);
        break;
    case Qt::Key_3:
        delete m_particleSystem;
        m_particleSystem = new ParticleSystem(NUM_PARTICLES, PARTICLE_RADIUS, GRID_SIZE, MAX_PARTICLES, make_int3(-50, 0, -50), make_int3(50, 200, 50), 5);
        m_particleSystem->addParticleGrid(make_int3(-10, 0, -3), make_int3(-7, 10, 3), 1.f, false);
        m_particleSystem->addParticleGrid(make_int3(-3, 0, -3), make_int3(3, 10, 3), 1.f, false);
        m_particleSystem->addParticleGrid(make_int3(7, 0, -3), make_int3(10, 10, 3), 1.f, false);
        break;
    case Qt::Key_4:
        delete m_particleSystem;
        m_particleSystem = new ParticleSystem(NUM_PARTICLES, PARTICLE_RADIUS, GRID_SIZE, MAX_PARTICLES, make_int3(-50, 0, -50), make_int3(50, 200, 50), 5);
#ifdef TWOD
        m_particleSystem->addParticleGrid(make_int3(-3, 0, 0), make_int3(3, 20, 0), 1.f, false);
#else
        m_particleSystem->addParticleGrid(make_int3(-5, 0, -5), make_int3(5, 20, 5), 1.f, false);
#endif
        break;
    case Qt::Key_5:
        delete m_particleSystem;
        m_particleSystem = new ParticleSystem(NUM_PARTICLES, PARTICLE_RADIUS, GRID_SIZE, MAX_PARTICLES, make_int3(-50, 0, -50), make_int3(50, 200, 50), 5);
        m_particleSystem->addHorizCloth(make_int2(-10, -10), make_int2(10, 10), make_float3(.3f, 5.5f, .3f), make_float2(.1f, .1f), 10.f, true);
        m_particleSystem->addParticleGrid(make_int3(-3, 6, -3), make_int3(3, 15, 3), 1.f, false);
        break;
    case Qt::Key_6:
        delete m_particleSystem;
        m_particleSystem = new ParticleSystem(NUM_PARTICLES, PARTICLE_RADIUS, GRID_SIZE, MAX_PARTICLES, make_int3(-50, 0, -50), make_int3(50, 200, 50), 5);
        m_particleSystem->addParticleGrid(make_int3(-5, 0, -5), make_int3(5, 10, 5), 1.f, false);
        m_particleSystem->addFluid(make_int3(-5, 20, -5), make_int3(5, 30, 5), .5f, 1.5f, make_float3(frand(),frand(),frand()));
        break;
    case Qt::Key_7:
        delete m_particleSystem;
        m_particleSystem = new ParticleSystem(NUM_PARTICLES, PARTICLE_RADIUS, GRID_SIZE, MAX_PARTICLES, make_int3(-50, 0, -50), make_int3(50, 200, 50), 5);
        break;
    case Qt::Key_8:
        delete m_particleSystem;
        m_particleSystem = new ParticleSystem(NUM_PARTICLES, PARTICLE_RADIUS, GRID_SIZE, MAX_PARTICLES, make_int3(-50, 0, -50), make_int3(50, 200, 50), 5);
        break;
    case Qt::Key_9:
        delete m_particleSystem;
        m_particleSystem = new ParticleSystem(NUM_PARTICLES, PARTICLE_RADIUS, GRID_SIZE, MAX_PARTICLES, make_int3(-50, 0, -50), make_int3(50, 200, 50), 5);

        h = make_float3(0, 10, 0);
        for(int i = 0; i < 50; i++)
        {
            angle = M_PI * i * 0.02f;
            vec = make_float3(cos(angle), sin(angle), 0.f);
            m_particleSystem->addRope(vec*5.f + h, vec*.5f, .35f, 30, 1.f, true);
        }
        m_particleSystem->addStaticSphere(make_int3(-4, 7, -4), make_int3(4, 16, 4), .5f);

        break;
    case Qt::Key_Space:
        m_fluidEmmiterOn = !m_fluidEmmiterOn;
        break;
    default:
        resetVbo = false;
        m_renderer->keyReleased(e);
        break;
    }
    if (resetVbo)
        m_renderer->createVAO(m_particleSystem->getCurrentReadBuffer(),
                              m_particleSystem->getParticleRadius());

}


void ParticleApp::resize(int w, int h)
{
    m_renderer->resize(w, h);
}

