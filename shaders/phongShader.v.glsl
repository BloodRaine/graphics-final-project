/*
 *   Vertex Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

// varying inputs
in vec3 vPosition;
in vec3 vNormal;

// varying outputs
out vec3 position;
out vec3 normal;

// uniforms
uniform mat4 modelView, projection, normalMtx;
uniform vec3 matDiff, matAmb, matSpec;
uniform vec3 lDiff, lAbm, lSpec, lPos, camPos;
uniform float shininess;

void main() {
    /*****************************************/
    /********* Vertex Calculations  **********/
    /*****************************************/

    position = vec3(modelView * vec4(vPosition, 1));
    normal = vec3(normalMtx * vec4(vNormal,1));

    gl_Position = projection * modelView * vec4(vPosition, 1);
}
