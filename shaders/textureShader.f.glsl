#version 330 core

in vec2 texCoord;

out vec4 fragColorOut;

uniform sampler2D tex;
uniform vec3 color;

void main() {
  vec4 texel = texture(tex, texCoord);
  fragColorOut = texel * vec4(color, 1);
}
