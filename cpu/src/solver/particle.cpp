#include "particle.h"

SDFData Particle::getSDFData(QList<Body *> *bodies, int idx)
{
    if (ph != SOLID || bod < 0) {
        return SDFData();
    }

    Body *body = bodies->at(bod);
    SDFData out = body->sdf[idx];
    out.rotate(body->angle);
    return out;
}

void Body::updateCOM(QList<Particle *> *estimates, bool useEstimates)
{
    // Recompute center of mass
    glm::dvec2 total;
    for (int i = 0; i < particles.size(); i++) {
        Particle *p = estimates->at(particles[i]);
        total += (useEstimates ? p->ep : p->p) / p->imass;
    }
    center = total * imass;

    // Recompute angle delta guess
    angle = 0.0;
    double prev = 0.0;
    for (int i = 0; i < particles.size(); i++) {
        int index = particles[i];
        glm::dvec2 q = rs[index];
        if (glm::dot(q,q) == 0) {
            continue;
        }
        Particle *p = estimates->at(index);
        glm::dvec2 r = p->ep - center;

        double cos = r.x * q.x + r.y * q.y,
               sin = r.y * q.x - r.x * q.y,
               next = atan2(sin, cos);

        // Ensure all guesses are close to each other
        if (i > 0) {
            if (prev - next >= M_PI) {
                next += 2 * M_PI;
            }
        } else {
            if (next < 0) {
                next += 2 * M_PI;
            }
        }

        prev = next;
        next /= p->imass;
        angle += next;
    }
    angle *= imass;
}

void Body::findCircumcenterPoints(QList<Particle *> *estimates, glm::dvec2 &ex1, glm::dvec2 &ex2){
    // Recompute the circumcenter
    double maxDist = 0.;
    double dist;
    for (int i = 0; i < particles.size(); i++) {
        for (int j = 0; j < particles.size(); j++) {
            if (i == j) continue;
            Particle *p1 = estimates->at(particles.at(i));
            Particle *p2 = estimates->at(particles.at(j));

            dist = glm::length(p1->p - p2->p);

            if (dist > maxDist){
                ex1 = p1->p;
                ex2 = p2->p;
                maxDist = dist;
            }
        }
    }
}

void Body::updateCC(QList<Particle *> *estimates)
{
    glm::dvec2 ex1, ex2;
    findCircumcenterPoints(estimates, ex1, ex2);
    ccenter = (ex1 + ex2) / 2.;
}

float Body::getAngle(QList<Particle *> *estimates){
    glm::dvec2 circumAim = glm::normalize(estimates->at(angleRefIndex2)->p - estimates->at(angleRefIndex1)->p);
    float angle = atan2(circumAim.y, circumAim.x);
    //angle = (angle >= 0 ? angle : (2*PI + angle));
    return angle; //[-pi, pi]
}

void Body::setAngleReferencePoints(int i1, int i2){
    angleRefIndex1 = i1;
    angleRefIndex1 = i2;
}

void Body::computeRs(QList<Particle *> *estimates)
{
    imass = 0.0;
    for (int i = 0; i < particles.size(); i++) {
        int idx = particles[i];
        Particle *p = estimates->at(idx);
        glm::dvec2 r = p->p - center;
        rs[idx] = r;

        if (glm::dot(r,r) != 0) {
            imass += (1.0 / p->imass);
        }
    }
    imass = 1.0 / imass;
}

void Body::setVelocityHint(glm::dvec2 velocity){
    m_velocityHint = velocity;
}

void Body::setAngleImpulseHint(float angle){
    m_angleImpulseHint = angle;
}

void Body::resetHints(){
    m_angleImpulseHint = 0;
    m_velocityHint = glm::dvec2(0.,0.);
}

float Body::getAngleImpulseHint(){
    return m_angleImpulseHint;
}

glm::dvec2 Body::getVelocityHint(){
    return m_velocityHint;
}
