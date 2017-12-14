#version 330 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoord;

out vec3 normalVec;
out vec3 lightVec[2];
out vec3 halfwayVec[2];
out vec2 texCoord;

out vec3 distToPlayer;

uniform mat4 modelviewMtx;
uniform mat4 viewMtx;
uniform mat4 projectionMtx;
uniform mat4 normalMtx;

uniform vec3 playerCoord;
uniform vec3 lightPos[2];

void main() {
    gl_Position = projectionMtx * modelviewMtx * vec4(vPos, 1.0);
    
    vec3 cameraVec = normalize( -(modelviewMtx*vec4(vPos,1.0)).xyz );
    normalVec = normalize( (normalMtx * vec4(vNormal,0.0)).xyz );
    
	for(int i = 0; i < lightPos.length(); i++){
		lightVec[i] = normalize( (viewMtx * vec4(lightPos[i],1.0)).xyz - (modelviewMtx*vec4(vPos,1.0)).xyz );
		halfwayVec[i] = normalize( cameraVec + lightVec[i] );
	}
	
	distToPlayer = playerCoord - vPos;
	
    texCoord = vTexCoord;
}
