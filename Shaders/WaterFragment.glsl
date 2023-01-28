#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform samplerCube cubeTex;
uniform sampler2D altTex;
uniform sampler2D environTex;

uniform vec3 cameraPos;
uniform vec3 heightmapDim;

uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

uniform mat4 lightRot;

uniform vec4 lightColour2;
uniform vec3 lightPos2;
uniform float lightRadius2;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal; 
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
	vec3 incident = normalize(normalize(lightPos - IN.worldPos) * mat3(lightRot));
	vec3 incident2 = normalize(normalize(lightPos2 - IN.worldPos));
	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);
	vec3 halfDir2 = normalize(incident2 + viewDir);

	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	
	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));	
	vec3 bumpNormal = normalize(TBN * normalize(texture(bumpTex, IN.texCoord).rgb * 2.0 - 1.0)); 

	vec4 altDiffuse = texture(altTex, IN.texCoord);
	vec4 environ = texture(environTex, IN.texCoord);
	diffuse = mix(diffuse, altDiffuse, environ.g);
	
	vec3 reflectDir = reflect(-viewDir, normalize(IN.normal));
	vec4 reflectTex = texture(cubeTex, reflectDir );
	vec4 reflectCol = reflectTex + (diffuse * 0.25f);
	
	float lambert = max(dot(incident , bumpNormal), 0.0f);
	float lambert2 = max(dot(incident2 , bumpNormal), 0.0f);
	float distance = length(lightPos - IN.worldPos );
	float distance2 = length(lightPos2 - IN.worldPos);
	float attenuation = 1.0f - clamp(distance / lightRadius ,0.0 ,1.0);
	float attenuation2 = 1.0f - clamp(distance2 / lightRadius2 ,0.0 ,1.0);

	float specFactor = clamp(dot(halfDir , bumpNormal ) ,0.0 ,1.0);
	float specFactor2 = clamp(dot(halfDir2 , bumpNormal) ,0.0 ,1.0);
	specFactor = pow(specFactor , 60.0 );
	specFactor2 = pow(specFactor2 , 60.0 );
	vec3 surface = (reflectCol.rgb * lightColour.rgb);
	vec3 surface2 = (reflectCol.rgb * lightColour2.rgb);
	fragColour.rgb = surface * lambert * attenuation;
	fragColour.rgb += surface2 * lambert2 * attenuation2;
	fragColour.rgb += (lightColour.rgb * specFactor )* attenuation *0.33;
	fragColour.rgb += (lightColour2.rgb * specFactor2 )* attenuation2 *0.33;
	fragColour.rgb += surface * 0.1f;
	
	float viewDistance = length(cameraPos - IN.worldPos);
	fragColour.a = reflectCol.a;
	fragColour.a *= clamp(viewDistance / (heightmapDim.x * 0.1875f), 0.0, 1.0);
}