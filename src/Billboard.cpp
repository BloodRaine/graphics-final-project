#include "Billboard.h"
#include <stdio.h>

Billboard::Billboard(){
	numBill = 0;
}

void Billboard::setUniformLocation(GLint mv, GLint n){
	mvMtx_loc = mv;
	nMtx_loc = n;
}

void Billboard::setAttributeLocation(GLint vPos, GLint vTex, GLint vNorm){
	vPos_loc = vPos;
	vTex_loc = vTex;
	vNorm_loc = vNorm;
}

void Billboard::setupBillboardBuffer(){
	VertexNormalTexture texturedQuadVerts[4] = {
		{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}, // 0 - BL
		{1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},  // 1 - BR
		{-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},  // 2 - TL
		{1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f}	// 3 - TR
	};
	GLuint vbod;
	glGenVertexArrays(1, &billboardVAO);
	glBindVertexArray(billboardVAO);
	glGenBuffers(1, &vbod);
	glBindBuffer(GL_ARRAY_BUFFER, vbod);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texturedQuadVerts), texturedQuadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vPos_loc);
	glVertexAttribPointer(vPos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTexture), (void *)0);
	glEnableVertexAttribArray(vTex_loc);
	glVertexAttribPointer(vTex_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTexture), (void *)(sizeof(float) * 6));
	
	glEnableVertexAttribArray(vNorm_loc);
	glVertexAttribPointer(vNorm_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTexture), (void *) (sizeof(float) * 3));
}

void Billboard::add(glm::vec3 l, glm::vec2 s){
	location.push_back(l);
	size.push_back(s);
	rotateAngle.push_back(0.0f);
	axisRot.push_back(glm::vec3(0, 1, 0));
	numBill++;
}

void Billboard::drawBillboard(glm::mat4 m, glm::mat4 v){

    glBindVertexArray(billboardVAO);

	for(GLuint i = 0; i < numBill; i++){
		glm::mat4 modelMtx = glm::translate(m, location.at(i));
		modelMtx = glm::rotate(modelMtx, rotateAngle.at(i), axisRot.at(i));
		modelMtx = glm::scale(modelMtx, glm::vec3(size.at(i), 1));
		
		glm::mat4 mv = v * modelMtx;
		glm::mat4 nMtx = glm::transpose(glm::inverse(mv));
		
		glUniformMatrix4fv(mvMtx_loc, 1, GL_FALSE, &mv[0][0]);
		glUniformMatrix4fv(nMtx_loc, 1, GL_FALSE, &nMtx[0][0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

}

void Billboard::updateBillboardAngle(glm::vec3 eye){
	for(GLuint i = 0; i < numBill; i++){
		glm::vec3 locToEye = eye - location.at(i);
		locToEye = glm::normalize(glm::vec3(locToEye.x, 0, locToEye.z));
		rotateAngle.at(i) = acos(glm::dot(glm::vec3(0, 0, 1), locToEye));
		axisRot.at(i) = glm::normalize(glm::cross(glm::vec3(0, 0, 1), locToEye));
		
	}
}