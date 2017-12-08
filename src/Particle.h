#pragma once
#include <glm/glm.hpp>

class Particle {
    public:

        glm::vec3 position;
        glm::vec3 velocity;

        float maxAge;
        float mass;
        float startTime;

        Particle(glm::vec3 pos, glm::vec3 vel, float mass, float maxAge, float start)
        {
            this->position = pos;
            this->velocity = vel;
            this->mass = mass;
            this->maxAge = maxAge;
            this->startTime = start;
        }

        void updatePosition() {
            
        }
};