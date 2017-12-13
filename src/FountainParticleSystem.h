#pragma once

#include "ParticleSystem.h"

class FountainParticleSystem : public ParticleSystem {
    public:
        FountainParticleSystem(Type type, glm::vec3 pos, float maxVel, float minVel, float angle, float age, float g, float rate, GLint handle);
        void generateParticle(int t);
        void updateParticles(int t);

        float mass;
        float minAge;
        float maxAge;
        float maxAngleFromY;
};