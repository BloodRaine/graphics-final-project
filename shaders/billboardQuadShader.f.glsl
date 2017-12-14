/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

in vec2 texCoord;
uniform vec3 color = vec3(1, 1, 1);

out vec4 fragColorOut;

uniform sampler2D tex;


void main() {

    //*****************************************
    //******* Final Color Calculations ********
    //*****************************************
   
	vec4 texel = texture(tex, texCoord);
	if(texel.a < 0.1) discard;
	if(texel.a < 0.4) texel.a = 0.4;
    fragColorOut = vec4(color, texel.a);
}
