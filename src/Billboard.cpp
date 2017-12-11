#include "Billboard.h"
#include <stdio.h>

Billboard::Billboard(GLint m, GLint vp, GLint vPos, GLint vTex){
	numBill = 0;
	mMtxLoc = m;
	vpMtxLoc = vp;
	vPosLoc = vPos;
	vTexLoc = vTex;
}

void Billboard::setupBillboardBuffer(){
	VertexText texturedQuadVerts[4] = {
		{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f}, // 0 - BL
		{1.0f, -1.0f, 0.0f, 1.0f, 0.0f},  // 1 - BR
		{-1.0f, 1.0f, .0f, 0.0f, 1.0f},  // 2 - TL
		{1.0f, 1.0f, 0.0f, 1.0f, 1.0f}	// 3 - TR
	};
	GLuint vbod;
	glGenVertexArrays(1, &billboardVAO);
	glBindVertexArray(billboardVAO);
	glGenBuffers(1, &vbod);
	glBindBuffer(GL_ARRAY_BUFFER, vbod);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texturedQuadVerts), texturedQuadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vPosLoc);
	glVertexAttribPointer(vPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexText), (void *)0);
	glEnableVertexAttribArray(vTexLoc);
	glVertexAttribPointer(vTexLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexText), (void *)(sizeof(float) * 3));
}

void Billboard::add(glm::vec3 l, glm::vec2 s){
	location.push_back(l);
	size.push_back(s);
	rotateAngle.push_back(0.0f);
	axisRot.push_back(glm::vec3(0, 1, 0));
	numBill++;
}

void Billboard::drawBillboard(glm::mat4 m, glm::mat4 vp){

    glBindVertexArray(billboardVAO);
	glUniformMatrix4fv(vpMtxLoc, 1, GL_FALSE, &vp[0][0]);

	for(GLuint i = 0; i < numBill; i++){
		glm::mat4 modelMtx = glm::translate(m, location.at(i));
		modelMtx = glm::rotate(modelMtx, rotateAngle.at(i), axisRot.at(i));
		modelMtx = glm::scale(modelMtx, glm::vec3(size.at(i), 1));
		
		glUniformMatrix4fv(mMtxLoc, 1, GL_FALSE, &modelMtx[0][0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

}

void Billboard::updateBillboardAngle(glm::vec3 eye){
	for(GLuint i = 0; i < numBill; i++){
		glm::vec3 locToEye = eye - location.at(i);
		locToEye = glm::normalize(glm::vec3(locToEye.x, 0, locToEye.z));
		//fprintf(stdout, " %f ", locToEye.y);
		rotateAngle.at(i) = acos(glm::dot(glm::vec3(0, 0, 1), locToEye));
		axisRot.at(i) = glm::normalize(glm::cross(glm::vec3(0, 0, 1), locToEye));
		
	}
}