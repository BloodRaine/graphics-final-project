/*
 *   Vertex Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

in vec3 vPos;

out vec3 theColor;

uniform mat4 mvMatrix, projMatrix;
uniform float time;

void main() {
    /*****************************************/
    /********* Vertex Calculations  **********/
    /*****************************************/

    float t = mod(time, 100.0)/100.0;
    
    gl_Position = projMatrix * mvMatrix * vec4(vPos, 1);

    // theColor = vec3(cos(t), sin(t), tan(t));
    
    theColor = vec3(1,1,1);
}
