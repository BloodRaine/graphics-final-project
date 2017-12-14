/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

in vec3 theColorG;

out vec4 fragColorOut;

void main() {

    /*****************************************/
    /******* Final Color Calculations ********/
    /*****************************************/

    fragColorOut = vec4(1,1,1,1);
    // fragColorOut = vec4(theColor,1);
}
