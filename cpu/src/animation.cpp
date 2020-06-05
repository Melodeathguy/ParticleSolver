#include "animation.h"



RotationKeyFrame::RotationKeyFrame(Body *body, QList<Particle *> *particles, double angle) : KeyFrame(body, particles), m_angle(angle) {};

bool RotationKeyFrame::tick(double delta) const {

    double pi = 3.14159265359;

    int dirFactor = 1;

    double totalMoveAngle = abs(m_angle - m_body->angle);

    if (totalMoveAngle < pi){
        dirFactor = -1;
    }
    if(totalMoveAngle > MAX_ANGLE){
        totalMoveAngle = MAX_ANGLE;
    } else if (totalMoveAngle < EPSILON) {
        return true;
    }

    for(int i = 0; i < m_body->particles.size(); i++){
        int particleId = m_body->particles.at(i);
        Particle * part = m_particles->at(particleId);
        glm::dvec2 rotated = glm::rotate(m_body->ccenter - part->p, dirFactor * totalMoveAngle);
        part->v = ((m_body->ccenter - rotated)-part->p) / delta;
    }


    m_body->angle = fmod(m_body->angle + dirFactor * totalMoveAngle, 2*pi);

    return false;
}


PositionKeyFrame::PositionKeyFrame(Body *body, QList<Particle *> *particles, glm::dvec2 pos) : KeyFrame(body, particles), m_pos(pos) {};

bool PositionKeyFrame::tick(double delta) const {

    bool reachedKeyframe = true;

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
    m_body->ccenter = aimVector;
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
