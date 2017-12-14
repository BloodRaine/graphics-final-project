#include "FountainParticleSystem.h"
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

FountainParticleSystem::FountainParticleSystem(Type type, glm::vec3 pos, float maxVel, float minVel, float angle, float age, float g, float rate, GLint handle)
{
    this->type = type;
    this->spawnPoint = pos;
    this->minVelocity = minVel;
    this->maxVelocity = maxVel;
    this->maxAngleFromY = angle;
    this->gravity = g;
    this->maxAge = abs(age);
    this->minAge = 180;
    this->spawnRate = rate;
    this->handle = handle;
    this->mass = 1000;
}

void FountainParticleSystem::updateParticles(int t) {
    for (int i = 0; i < spawnRate / 90; i++) {
        generateParticle(t);
    }
    int c = 0;

    for (Particle *particle : this->particles)
    {
        if (particle->position.y < -0.01) {
            this->particles.erase(this->particles.begin() + c);
			c--;
        } else {//(particle->maxAge > 0.0) {

            glm::vec3 a = glm::vec3(0, gravity, 0) / particle->mass;
            particle->velocity += (a * (t - particle->startTime));
            // applyForce(glm::vec3(0, gravity, 0), (t - particle->startTime)/60.0f);
            particle->updatePosition();
            c++;
        }
        
    }
}

void FountainParticleSystem::generateParticle(int t) {
    float vel = abs(randNumber((maxVelocity - minVelocity) * 10)) / 10.0f + minVelocity;

    float xComp = vel * tan(randNumber(maxAngleFromY) * M_PI / 180.0f);
    float zComp = vel * tan(randNumber(maxAngleFromY) * M_PI / 180.0f);
    // printf("%f %f %f\n", xComp, vel, zComp);

    startVelocity.x = xComp;
    startVelocity.y = vel;
    startVelocity.z = zComp;
    float age = randNumber(int(maxAge - minAge)) + minAge;

    Particle *newParticle = new Particle(spawnPoint, startVelocity, mass, age, float(t));
    this->particles.push_back(newParticle);
}