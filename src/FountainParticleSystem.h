#pragma once

#include "ParticleSystem.h"

class FountainParticleSystem : public ParticleSystem {
    public:
      FountainParticleSystem(glm::vec3 loc, float maxVel, float minVel, float g, float rate);
      void generateParticle(int t);
      void updateParticles(int t);

      float mass;
      float minAge;
      float maxAge;
      float maxAngleFromY;
};