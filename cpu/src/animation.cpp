#include "animation.h"


DelayKeyFrame::DelayKeyFrame(double seconds) : KeyFrame(0, 0), m_seconds(0.), m_targetSeconds(seconds) {};

bool DelayKeyFrame::tick(double delta) {
    return (m_seconds += delta) >= m_targetSeconds;
}


RotationKeyFrame::RotationKeyFrame(Body *body, QList<Particle *> *particles, double angle) : KeyFrame(body, particles), m_angle(angle) {};

bool RotationKeyFrame::tick(double delta) {

    double pi = 3.14159265359;

    // Update circumcenters
    //m_body->updateCC(m_particles);

    double current_angle = m_body->getAngle(m_particles);
    double target_angle = m_angle;

    double absAngle = current_angle - target_angle;

    // Clockwise Direction [-PI, PI]
    if (absAngle > PI) absAngle -= 2*PI;
    else if (absAngle < -PI) absAngle += 2*PI;

    int sign = absAngle > 0 ? 1 : -1;

    bool done = false;

    if(abs(absAngle) > MAX_ANGLE){ // Angle step too big: limit to MAX_ANGLE
        absAngle = -sign * MAX_ANGLE;
    } else {
        done = true;
        absAngle *= -1;
    } // [-MAX_ANGLE, MAX_ANGLE]


    for(int i = 0; i < m_body->particles.size(); i++){
        int particleId = m_body->particles.at(i);
        Particle * part = m_particles->at(particleId);
        glm::dvec2 rotated = glm::rotate(m_body->ccenter - part->p, absAngle);
        part->v = ((m_body->ccenter - rotated)-part->p) / delta;
    }

    return done;
}


PositionKeyFrame::PositionKeyFrame(Body *body, QList<Particle *> *particles, glm::dvec2 pos) : KeyFrame(body, particles), m_pos(pos) {};

bool PositionKeyFrame::tick(double delta) {

    bool reachedKeyframe = true;

    // Update circumcenters
    m_body->updateCC(m_particles);

    glm::dvec2 aimVector = m_pos - m_body->ccenter;
    double vectorLength = (double) glm::length(aimVector);
    if (vectorLength > MAX_STEP){
        aimVector /= vectorLength;
        aimVector *= MAX_STEP;
        reachedKeyframe = false;
    }
    aimVector += m_body->ccenter;

    for(int i = 0; i < m_body->particles.size(); i++){
        int particleId = m_body->particles.at(i);
        Particle * part = m_particles->at(particleId);
        part->v = ((aimVector - (m_body->ccenter - part->p)) - part->p) / delta;
    }

    return reachedKeyframe;
}



Animation::Animation(Body *body, QList<Particle *> *particles) : m_body(body), m_particles(particles) {}


void Animation::addKeyFrame(glm::dvec2 pos){
    std::shared_ptr<KeyFrame> kf = std::make_shared<PositionKeyFrame>(m_body, m_particles, pos);
    m_keyFrames.push(kf);
}

void Animation::addRotationKeyframe(double angle){
    std::shared_ptr<KeyFrame> kf = std::make_shared<RotationKeyFrame>(m_body, m_particles, angle);
    m_keyFrames.push(kf);
}

void Animation::addDelay(double seconds){
    std::shared_ptr<KeyFrame> kf = std::make_shared<DelayKeyFrame>(seconds);
    m_keyFrames.push(kf);
}

bool Animation::tick(double delta){

    if (m_keyFrames.empty())
        return true;

    // process keyframe, if finished (= returns true) remove from list
    m_keyFrames.front()->tick(delta);
    if (m_keyFrames.front()->tick(delta)){
        m_keyFrames.pop();
    }

    if (m_keyFrames.empty())
        return true;
    return false;
}
