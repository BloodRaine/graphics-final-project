/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

uniform sampler2D textureMap;

in vec2 texCoordV;

out vec4 fragColorOut;

void main() {

    /*****************************************/
    /******* Final Color Calculations ********/
    /*****************************************/

    // get the texel for the interpolated texture coords
    vec4 texel = vec4(1,1,1,1);
    texel = texture(textureMap, texCoordV);

    // modulate the final color
    //fragColorOut = texel * lightColor;
    // or decal it
    
    fragColorOut = texel;
}
