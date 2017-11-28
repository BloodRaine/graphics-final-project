/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

// TODO J: add varying input
in vec2 texCoord;

out vec4 fragColorOut;

// TODO K: add texture uniform
uniform sampler2D tex;


void main() {

    //*****************************************
    //******* Final Color Calculations ********
    //*****************************************
    
    // TODO L: load texel
    fragColorOut = texture(tex, texCoord);
}
