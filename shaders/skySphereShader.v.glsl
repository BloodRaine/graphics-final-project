/*
 *   Vertex Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

uniform mat4 mvpMatrix;
uniform float time;

in vec3 vPosition;
layout( location = 1) in vec2 texCoord;

out vec2 texCoordV;

void main() {
    /*****************************************/
    /********* Vertex Calculations  **********/
    /*****************************************/
    
    gl_Position = mvpMatrix * vec4(vPosition, 1.0);
    texCoordV = texCoord;
}
