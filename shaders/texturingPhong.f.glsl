#version 330 core

in vec3 normalVec;
in vec3 lightVec[2];
in vec3 halfwayVec[2];
in vec2 texCoord;

in vec3 distToPlayer;

uniform vec4 lightDiffuse[2];
uniform vec4 lightSpecular[2];
uniform vec4 lightAmbient[2];

uniform vec4 materialDiffuse;
uniform vec4 materialSpecular;
uniform float materialShininess;
uniform vec4 materialAmbient;
uniform sampler2D txtr;

out vec4 fragColorOut;

void main() {
	
	float toPlayer = length(distToPlayer);
	if(toPlayer < 1.0) toPlayer = 1.0;
	
	fragColorOut = vec4(0, 0, 0, 0);
	vec4 texel = texture( txtr, texCoord );
	if(texel.a < 0.4){
		discard;
		return;
	}
	vec3 normalVec2 = normalize(normalVec);
	for(int i = 0; i < lightVec.length(); i++){
		vec3 lightVec2 = normalize(lightVec[i]);
		vec3 halfwayVec2 = normalize(halfwayVec[i]);
		float sDotN = max( dot(lightVec2, normalVec2), 0.0 );
		
		vec4 diffuse = materialDiffuse * sDotN;
		if(i != 0 || toPlayer > 10.0) diffuse *= lightDiffuse[i];
		else diffuse *= vec4(vec3(1, 1, 1)/(toPlayer/2.0), 1.0);
		vec4 specular = vec4(0.0);
		if( sDotN > 0.0 )
			specular = lightSpecular[i] * materialSpecular * pow( max( 0.0, dot( halfwayVec2, normalVec2 ) ), materialShininess );
		vec4 ambient = lightAmbient[i] * materialAmbient;
		fragColorOut += diffuse + specular + ambient;
	}
    fragColorOut *= texel;
}
