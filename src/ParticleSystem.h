#pragma once

#include "Particle.h"
#include <vector>
#include <GL/glew.h>
#include <string>

using namespace std;

enum Type { FOUNTAIN, FIREWORK, SHOWER};

class ParticleSystem {
    public:
        vector<Particle*> particles;

        glm::vec3 spawnPoint;
        glm::vec3 startVelocity;

        Type type;
        float maxVelocity;
        float minVelocity;
        float spawnRate;
        float gravity;
        GLint handle;

        ParticleSystem();
        ParticleSystem(glm::vec3 loc, float maxVel, float minVel, float g, float rate);
        virtual void generateParticle(int t) = 0;
        virtual void updateParticles(int t) = 0;
        virtual void draw(GLuint pointsVAO, GLuint pointsVBO, GLuint textureHandle);
        // virtual void render();
		virtual bool removeSystem() { return false; };
        GLfloat randNumber(int max);
};