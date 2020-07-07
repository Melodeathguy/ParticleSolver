#ifndef ANIMATION_H
#define ANIMATION_H

#include <animation/keyframe.h>
#include <particle.h>
#include <glm.hpp>
#include <queue>
#include <memory>
#include <cassert>


class Animation
{
public:
    Animation(Body *body, QList<Particle *> *particles) : m_body(body), m_particles(particles){};
    virtual bool tick(double delta) = 0;

    void addPositionKeyFrame(glm::dvec2 pos, int cutAfter = -1);
    void addRotationKeyframe(double angle, int cutAfter = -1);
    void addDelay(double seconds, int cutAfter = -1);

protected:
    Body *m_body;
    QList<Particle *> *m_particles;
    std::queue<shared_ptr<KeyFrame>> m_keyFrames;

};

class StaticAnimation : public Animation
{
public:

    StaticAnimation(Body *body, QList<Particle *> *particles) : Animation(body, particles) {};
    bool tick(double delta) override;

private:
    std::queue<shared_ptr<KeyFrame>> m_keyFrames;
};

class DynamicAnimation : public Animation
{
public:

    DynamicAnimation(Body *body, QList<Particle *> *particles, glm::dvec2 xBoundaries, glm::dvec2 yBoundaries, float radius, int cutAfter = 1, float posProb = 0.5, float rotProb = 0.4, float delayProb = 0.1) :
        Animation(body, particles),
        m_xBoundaries(xBoundaries),
        m_yBoundaries(yBoundaries),
        m_radius(radius),
        m_cutAfter(cutAfter),
        m_posProb(posProb),
        m_rotProb(rotProb),
        m_delayProb(delayProb) {
        assert(fabs((posProb + rotProb + delayProb) - 1.0) < EPSILON);
    };
    bool tick(double delta) override;


private:
    void generateRandomKeyframe();

    float m_posProb;
    float m_rotProb;
    float m_delayProb;
    float m_cutAfter;
    float m_radius;
    glm::dvec2 m_xBoundaries;
    glm::dvec2 m_yBoundaries;

};

#endif // ANIMATION_H
