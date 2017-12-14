#include "FireworkParticleSystem.h"

FireworkParticleSystem::FireworkParticleSystem(Type type, glm::vec3 pos, float maxVel, float age, float g,float num) {
    this->type = type;
    this->spawnPoint = pos;
    this->maxVelocity = maxVel;
    this->gravity = g;
    this->maxAge = abs(age);
    this->minAge = 60;
    this->spawnRate = num;
    this->mass = 1000;
    this->handle = -1;
}

void FireworkParticleSystem::updateParticles(int t) {
    if (particles.size() > 0) {
        int c = 0;
        for (Particle *particle : this->particles)
        {
            // if (particle->position.y < -0.01 || particle->maxAge < 0.0)
            // {
            //     printf("erasing\n");
            //     this->particles.erase(this->particles.begin() + c);
            // }
            // else
            // {
                // printf("Moving");
                glm::vec3 a = glm::vec3(0, gravity, 0) / particle->mass;
                particle->velocity += (a * (t - particle->startTime));
                particle->updatePosition();
                c++;
            // }
            
        }
    }   
}

void FireworkParticleSystem::generateParticle(int t) {
    float vel = abs(randNumber((maxVelocity/2.0) * 10)) / 10.0f;

    float xComp = vel * tan(randNumber(2 * M_PI * 1000000000) / 1000000000.0f * M_PI / 180.0f);
    float zComp = vel * tan(randNumber(2 * M_PI * 1000000000) / 1000000000.0f * M_PI / 180.0f);
    // printf("%f %f %f\n", xComp, vel, zComp);

    startVelocity.x = xComp;
    startVelocity.y = vel;
    startVelocity.z = zComp;
    float age = randNumber(int(maxAge - minAge)) + minAge;

    Particle *newParticle = new Particle(spawnPoint, startVelocity, mass, age, float(t));
    this->particles.push_back(newParticle);
}

void FireworkParticleSystem::start(float t) {
    printf("BOOM");
    for (int i = 0; i < spawnRate; i++) {
        generateParticle(t);
    }
}