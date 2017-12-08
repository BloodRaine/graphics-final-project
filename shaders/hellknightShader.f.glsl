#version 330 core

in vec2 texCoord;
  
layout(location=0)out vec4 fragColorOut;
  
uniform sampler2D tex;
  
void main() {
  fragColorOut = texture( tex, texCoord );
}