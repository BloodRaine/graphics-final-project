#pragma once

#include "Particle.h"
#include <vector>
#include <GL/glew.h>

using namespace std;

class ParticleSystem {
    public:
        vector<Particle*> particles;

        glm::vec3 spawnPoint;
        glm::vec3 startVelocity;

        float maxVelocity;
        float minVelocity;
        float spawnRate;
        float gravity;

        ParticleSystem(glm::vec3 loc, float maxVel, float minVel, float g, float rate);
        virtual void generateParticle(int t) = 0;
        virtual void updateParticles(int t) = 0;
        virtual void draw(GLuint pointsVAO, GLuint pointsVBO, GLuint textureHandle);
        GLfloat randNumber(int max);
};