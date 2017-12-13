#include <GL/glew.h>
// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Billboard{
	
private:
	struct VertexNormalTexture { GLfloat x, y, z, nx, ny, nz, s, t; };
	GLuint billboardVAO;
	GLint mvMtx_loc, nMtx_loc;
	GLint vPos_loc, vTex_loc, vNorm_loc;
	
	std::vector<GLfloat> rotateAngle;
	std::vector<glm::vec3> axisRot;
	std::vector<glm::vec3> location;
	std::vector<glm::vec2> size;
	GLuint numBill;

public:
	Billboard();
    void setUniformLocation(GLint, GLint);
	void setAttributeLocation(GLint, GLint, GLint);
	void setupBillboardBuffer();
	void updateBillboardAngle(glm::vec3);
	void drawBillboard(glm::mat4, glm::mat4);
	void add(glm::vec3, glm::vec2);
};
	
