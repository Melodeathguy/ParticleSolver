#include "animation.h"

void Animation::addPositionKeyFrame(glm::dvec2 pos, int cutAfter){
    std::shared_ptr<KeyFrame> kf = std::make_shared<PositionKeyFrame>(m_body, m_particles, pos, cutAfter);
    m_keyFrames.push(kf);
}

void Animation::addRotationKeyframe(double angle, int cutAfter){
    std::shared_ptr<KeyFrame> kf = std::make_shared<RotationKeyFrame>(m_body, m_particles, angle, cutAfter);
    m_keyFrames.push(kf);
}

void Animation::addDelay(double seconds, int cutAfter){
    std::shared_ptr<KeyFrame> kf = std::make_shared<DelayKeyFrame>(seconds, cutAfter);
    m_keyFrames.push(kf);
}

/*
Static Animations: Keframes are set manually BEFORE the simulation
*/

bool StaticAnimation::tick(double delta){

    if (m_keyFrames.empty())
        return true;

    // process keyframe, if finished (= returns true) remove from list
    if (m_keyFrames.front()->tick(delta)){
        m_keyFrames.pop();
    }

    if (m_keyFrames.empty())
        return true;
    return false;
}

/*
Dynamic Animations: Keframes are drawn randomly DURING the simulation. The probabilities can be provided via constructor parameters.
If keyframes are added manually before the simulation, they are processed beforehand. If the keyframe stack is empty, new keyframes are generated.
*/

void DynamicAnimation::generateRandomKeyframe(){
    double randVal = urand(0, 1);
    // draw position keyframe

    if (randVal < m_posProb){



        m_body->updateCC(m_particles);

        float xPos = m_body->ccenter.x;
        float yPos = m_body->ccenter.y;

        // examples: borders = - 25, 25 / -25, 25, padding = 2, radius = 7
        float spaceLeft =     xPos - (m_xBoundaries.x + PADDING + m_radius);    // x - (-25 + 2 + 7) = x + 16
        float spaceRight =    m_xBoundaries.y - PADDING - m_radius - xPos;      // 25 - 2 - 7 - x = 16 - x
        float spaceTop =      m_yBoundaries.y - PADDING - m_radius - yPos;      // 25 - 2 - 7 -y = 16 - y
        float spaceBottom =   yPos - (m_yBoundaries.x + PADDING + m_radius);    // y - (-25 + 2 + 7) = y + 16

        float aimX = urand(-min(POINT_SAMPLE_RADIUS, spaceLeft), min(POINT_SAMPLE_RADIUS, spaceRight));
        float aimY = urand(-min(POINT_SAMPLE_RADIUS, spaceBottom), min(POINT_SAMPLE_RADIUS, spaceTop));

        //std::cout << spaceLeft << ", " << spaceRight << ", " << spaceBottom << ", " << spaceTop << "\n";

        glm::dvec2 randomPos(xPos + aimX, yPos + aimY);
        this->addPositionKeyFrame(randomPos, m_cutAfter);
    }
    // draw rotation keyframe
    else if (randVal < m_posProb + m_rotProb){
        float randomAngle = urand(-PI, PI);
        this->addRotationKeyframe(randomAngle, m_cutAfter);
    }
    // draw delay keyframe
    else {
        float waitTime = urand(0, MAX_WAIT_TIME);
        this->addDelay(waitTime, m_cutAfter);
    }
}

bool DynamicAnimation::tick(double delta){

    // no more keyframes in the slope: draw a new random one!
    if (m_keyFrames.empty()){
        generateRandomKeyframe();
    }

    // process keyframe, if finished (= returns true) remove from list
    if (m_keyFrames.front()->tick(delta)){
        m_keyFrames.pop();
    }

    return true;
}
