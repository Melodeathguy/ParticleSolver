#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <particle.h>

#define MAX_STEP .1
#define MAX_ANGLE 0.015
#define MAX_WAIT_TIME 1.0
#define PADDING 2.
#define POINT_SAMPLE_RADIUS 5.f

class KeyFrame{
public:
    virtual bool tick(double delta) = 0;
    KeyFrame(Body *body, QList<Particle *> *particles, int cutAfter) : m_body(body), m_particles(particles), m_cutAfter(cutAfter), m_stepCounter(0){};

    void increaseStepCounter(){
        m_stepCounter += 1;
    }

protected:
    Body *m_body;
    QList<Particle *> *m_particles;
    int m_cutAfter;
    int m_stepCounter;
};

class RotationKeyFrame : public KeyFrame{
public:
    RotationKeyFrame(Body *body, QList<Particle *> *particles, double angle, int cutAfter);
    bool tick(double delta) override;

private:
    double m_angle;
    bool m_done;
};

class PositionKeyFrame : public KeyFrame{
public:
    PositionKeyFrame(Body *body, QList<Particle *> *particles, glm::dvec2 pos, int cutAfter);
    bool tick(double delta) override;

private:
    glm::dvec2 m_pos;
};

class DelayKeyFrame : public KeyFrame{
public:
    DelayKeyFrame(double seconds, int cutAfter);
    bool tick(double delta) override;

private:
    double m_seconds;
    double m_targetSeconds;
};

#endif // KEYFRAME_H
