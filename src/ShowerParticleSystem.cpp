#include "ShowerParticleSystem.h"

ShowerParticleSystem::ShowerParticleSystem(Type type, glm::vec3 pos, float maxVel, float minVel, float age, float g, float rate, float life, GLint handle, GLuint colorLoc){
	this->type = type;
	this->spawnPoint = pos;
    this->minVelocity = minVel;
    this->maxVelocity = maxVel;
    this->gravity = g;
    this->maxAge = abs(age);
    this->minAge = 60;
    this->spawnRate = rate;
    this->handle = handle;
    this->mass = 10;
	this->life = life;
	this->color = glm::vec3(rand()/(float)RAND_MAX, rand()/(float)RAND_MAX, rand()/(float)RAND_MAX);
	this->color_loc = colorLoc;
}

void ShowerParticleSystem::updateParticles(int t) {
	if(this->life > 0){
		for (int i = 0; i < spawnRate / 90; i++) {
			generateParticle(t);
		}
	}
    int c = 0;

    for (Particle *particle : this->particles)
    {
        if (particle->position.y < -0.01 || particle->maxAge < 0.0) { 
            this->particles.erase(this->particles.begin() + c);
			c--;
        } else {//(particle->maxAge > 0.0) {
            glm::vec3 a = glm::vec3(0, gravity, 0) / particle->mass;
            particle->velocity += (a * (t - particle->startTime));
            // applyForce(glm::vec3(0, gravity, 0), (t - particle->startTime)/60.0f);
            particle->updatePosition();
        }
        c++;
    }
	this->life--;
}

void ShowerParticleSystem::generateParticle(int t){
	startVelocity = glm::normalize(glm::vec3(randNumber(1), randNumber(1), randNumber(1)));
	startVelocity *= (rand()/(GLfloat) RAND_MAX * (maxVelocity - minVelocity) + minVelocity);
	
    float age = randNumber(int(maxAge - minAge)) + minAge;

    Particle *newParticle = new Particle(spawnPoint, startVelocity, mass, age, float(t));
    this->particles.push_back(newParticle);
}

void ShowerParticleSystem::draw(GLuint pointsVAO, GLuint pointsVBO, GLuint textureHandle){
	glUniform3fv(color_loc, 1, &color[0]);
	ParticleSystem::draw(pointsVAO, pointsVBO, textureHandle);
}

bool ShowerParticleSystem::removeSystem(){
	if(particles.empty())return true;
	else return false;
}