#include "FountainParticleSystem.h"

// FountainParticleSystem::FountainParticleSystem(glm::vec3 loc, float maxVel, float minVel, float g, float rate) {
//     ParticleSystem(loc, maxVel, minVel, g, rate);
//     // this->maxAge = age;
// }

void FountainParticleSystem::updateParticles(int t) {
    for (int i = 0; i < spawnRate / 90; i++)
    {
        generateParticle(t);
    }
    int c = 0;

    for (Particle *particle : this->particles)
    {
        if (particle->maxAge > 0.0 || particle->position.y > -3)
        {
            glm::vec3 a = glm::vec3(0, gravity, 0) / particle->mass;
            particle->velocity += (a * (t - particle->startTime) / 60.0f);
            // applyForce(glm::vec3(0, gravity, 0), (t - particle->startTime)/60.0f);
            particle->updatePosition();
        }
        else
        {
            this->particles.erase(this->particles.begin() + c);
        }
        c++;
    }
}

void FountainParticleSystem::generateParticle(int t)
{

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