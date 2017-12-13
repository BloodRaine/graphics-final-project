#include "ParticleSystem.h"

struct Vertex
{
    GLfloat x, y, z;
};

ParticleSystem::ParticleSystem() {
    printf("Yoohoo");
}

ParticleSystem::ParticleSystem(glm::vec3 loc, float maxVel, float minVel, float g, float rate) {
    this->spawnPoint = loc;
    this->maxVelocity = maxVel;
    this->minVelocity = minVel;
    this->gravity = g;
    this->spawnRate = rate;
}

GLfloat ParticleSystem::randNumber(int max)
{
    return rand() / (GLfloat)RAND_MAX * max * 2.0 - max;
}

void ParticleSystem::draw(GLuint pointsVAO, GLuint pointsVBO, GLuint textureHandle)
{
    Vertex verts[this->particles.size()];
    int i = 0;
    for (Particle *particle : this->particles)
    {
        glm::vec3 loc = particle->position;
        Vertex v = {loc.x, loc.y, loc.z};
        verts[i] = v;
        i++;
    }

    glBindVertexArray(pointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glDrawArrays(GL_POINTS, 0, this->particles.size());
}