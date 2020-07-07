#ifndef ANIMATION_H
#define ANIMATION_H

#include <particle.h>
#include <glm.hpp>
#include <queue>
#include <memory>
#include <cassert>

#define MAX_STEP .2
#define MAX_ANGLE 0.015


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


class Animation
{
public:
    Animation(Body *, QList<Particle *> *particles);

    void addKeyFrame(glm::dvec2 pos, int cutAfter = -1);
    void addRotationKeyframe(double angle, int cutAfter = -1);
    void addDelay(double seconds, int cutAfter = -1);

    bool tick(double delta);


private:

    Body *m_body;
    std::queue<shared_ptr<KeyFrame>> m_keyFrames;
    QList<Particle *> *m_particles;

};

#endif // ANIMATION_H
