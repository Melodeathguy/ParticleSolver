#include "keyframe.h"


DelayKeyFrame::DelayKeyFrame(double seconds, int cutAfter) : KeyFrame(0, 0, cutAfter), m_seconds(0.), m_targetSeconds(seconds) {};

bool DelayKeyFrame::tick(double delta) {
    if (m_cutAfter > 0 && m_stepCounter >= m_cutAfter){
        return true;
    }
    increaseStepCounter();
    return (m_seconds += delta) >= m_targetSeconds;
}


RotationKeyFrame::RotationKeyFrame(Body *body, QList<Particle *> *particles, double angle, int cutAfter) : KeyFrame(body, particles, cutAfter), m_angle(angle), m_done(false) {};

bool RotationKeyFrame::tick(double delta) {
    if (m_cutAfter > 0 && m_stepCounter >= m_cutAfter){
        m_done = true;
    }

    double pi = 3.14159265359;

    // reached targ  et state last step, setting digger particle's velocity back to 0
    if (m_done){
        glm::dvec2 zeroVec = glm::dvec2(0.,0.);
        for(int i = 0; i < m_body->particles.size(); i++){
            int particleId = m_body->particles.at(i);
            Particle * part = m_particles->at(particleId);
            //part->v = zeroVec;
        }
        increaseStepCounter();
        return true;
    }


    double current_angle = m_body->getAngle(m_particles); // [-pi, pi]
    double target_angle = m_angle;

    double shiftAngle = target_angle - current_angle; // [-2pi, 2pi]

    // choose the turning direction with the smallest angle
    if (shiftAngle > pi){
        shiftAngle = shiftAngle - 2*pi;
    }
    else if(shiftAngle < -pi){
        shiftAngle = (2*pi) + shiftAngle;
    } // [-pi, pi]

    assert(-pi < shiftAngle && shiftAngle < pi);

    // example for pi := 8
    //  2 - 1 = 1 // case 1 no hops, clockwise
    //  -2 --1 = -1 -> 2 // case 2 no hops, counterclockwise
    //  -3 - 3 = -6 -> 2 // case 3 hop over pi, clockwise
    //  3 --3 = 6 // case 4 hop over pi, counterclockwise
    //  -1 - 1 = -2 // case 5 hop over 0, clockwise
    //  1 -- 1 = 2 // case 6 hop over 0, counterclockwise


    double angleImpulse;
    if(abs(shiftAngle) > MAX_ANGLE){ // Angle step too big: limit to MAX_ANGLE
        angleImpulse = shiftAngle > 0 ? MAX_ANGLE : -MAX_ANGLE;
    } else {
        angleImpulse = shiftAngle;
        m_done = true;

        //std::cout << "Reached: " << 180 * m_angle / pi << std::endl;
    } // [-MAX_ANGLE, MAX_ANGLE]

    m_body->setAngleImpulseHint(angleImpulse);

    for(int i = 0; i < m_body->particles.size(); i++){
        int particleId = m_body->particles.at(i);
        Particle * part = m_particles->at(particleId);
        glm::dvec2 rotated = glm::rotate(m_body->ccenter - part->p, angleImpulse);
        part->v += 0.01 * ((m_body->ccenter - rotated)-part->p) / delta;
    }
    increaseStepCounter();
    return false;
}


PositionKeyFrame::PositionKeyFrame(Body *body, QList<Particle *> *particles, glm::dvec2 pos, int cutAfter) : KeyFrame(body, particles, cutAfter), m_pos(pos) {};

bool PositionKeyFrame::tick(double delta) {
    if (m_cutAfter > 0 && m_stepCounter >= m_cutAfter){
        return true;
    }

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

    m_body->setVelocityHint(aimVector);

    aimVector += m_body->ccenter;

    for(int i = 0; i < m_body->particles.size(); i++){
        int particleId = m_body->particles.at(i);
        Particle * part = m_particles->at(particleId);
        part->v += 0.1 * ((aimVector - (m_body->ccenter - part->p)) - part->p) / delta;
    }
    increaseStepCounter();
    return reachedKeyframe;
}
