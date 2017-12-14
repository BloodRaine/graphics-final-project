#pragma once

#include "ParticleSystem.h"

class FireworkParticleSystem : public ParticleSystem
{
  public:
    FireworkParticleSystem(Type type, glm::vec3 pos, float maxVel, float age, float g, float num);
    void generateParticle(int t);
    void updateParticles(int t);
    void start(float t);

    float mass;
    float minAge;
    float maxAge;
};