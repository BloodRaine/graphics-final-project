/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

in vec3 position;
in vec3 normal;

out vec4 colorOut;

uniform vec3 matDiff, matAmb, matSpec;
uniform vec3 lDiff, lAbm, lSpec, lightPos, camPos;
uniform float shininess;

void main() {

//     /*****************************************/
//     /******* Final Color Calculations ********/
//     /*****************************************/

//     Implimenting Phong Shading Model
    
    vec3 ambient = lAbm * matAmb;

    vec3 surface2Light = normalize(lightPos - position);
    vec3 norm = normalize(normal);
    float dCont = max(0.0, dot(surface2Light, norm));
    vec3 diffuse = lDiff * matDiff * dCont;

    

    vec3 specular = vec3(0.0);
    if (dCont > 0.0) {
        vec3 surface2Cam = normalize(camPos-position);
        vec3 reflection = normalize(reflect(-surface2Light, norm));
        float sCont = pow(max(0.0, dot(surface2Cam, reflection)), shininess);
        specular = lSpec * matSpec * sCont;
    }
    

    colorOut = vec4(ambient + diffuse + specular, 1.0);
}
