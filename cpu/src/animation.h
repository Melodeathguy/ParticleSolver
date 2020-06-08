#ifndef ANIMATION_H
#define ANIMATION_H

#import <particle.h>
#import <glm.hpp>
#import <queue>


#define MAX_STEP .1
#define MAX_ANGLE 0.01


class KeyFrame{
public:
    virtual bool tick(double delta) = 0;
    KeyFrame(Body *body, QList<Particle *> *particles) : m_body(body), m_particles(particles){};

protected:
    Body *m_body;
    QList<Particle *> *m_particles;
};

class RotationKeyFrame : public KeyFrame{
public:
    RotationKeyFrame(Body *body, QList<Particle *> *particles, double angle);
    bool tick(double delta) override;

private:
    double m_angle;
};

class PositionKeyFrame : public KeyFrame{
public:
    PositionKeyFrame(Body *body, QList<Particle *> *particles, glm::dvec2 pos);
    bool tick(double delta) override;

private:
    glm::dvec2 m_pos;
};

class DelayKeyFrame : public KeyFrame{
public:
    DelayKeyFrame(double seconds);
    bool tick(double delta) override;

private:
    double m_seconds;
    double m_targetSeconds;
};


class Animation
{
public:
    Animation(Body *, QList<Particle *> *particles);

    void addKeyFrame(glm::dvec2 pos);
    void addRotationKeyframe(double angle);
    void addDelay(double seconds);

    bool tick(double delta);


private:

    Body *m_body;
    std::queue<shared_ptr<KeyFrame>> m_keyFrames;
    QList<Particle *> *m_particles;

};

#endif // ANIMATION_H
