#include "ParticleSystem.h"

class ShowerParticleSystem : public ParticleSystem {
	public:
		ShowerParticleSystem(Type type, glm::vec3 pos, float maxVel, float minVel, float age,   float g, float rate, float life, GLint handle, GLuint colorLoc);
		void generateParticle(int t);
		void updateParticles(int t);
		bool removeSystem();
		
		void draw(GLuint pointsVAO, GLuint pointsVBO, GLuint textureHandle);
		
		float mass, minAge, maxAge;
		
		float life;
		glm::vec3 color;
		GLuint color_loc;
};