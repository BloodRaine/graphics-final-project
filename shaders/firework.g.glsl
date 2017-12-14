/*
 *   Geometry Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

in vec3 theColor[1];
layout( points ) in;

layout( points, max_vertices = 1 ) out;

uniform mat4 projMatrix;
out vec3 theColorG;

void main() {
    //*****************************************
    //********* Vertex Calculations  **********
    //*****************************************
    gl_Position = projMatrix * gl_in[0].gl_Position;

    theColorG = theColor[0];
    
    EmitVertex();
    EndPrimitive();
}