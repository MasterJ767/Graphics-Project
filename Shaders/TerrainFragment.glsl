#version 330 core
uniform sampler2D diffuseTexE;
uniform sampler2D bumpTexE; 
uniform sampler2D diffuseTexG;
uniform sampler2D bumpTexG; 

uniform vec3 cameraPos;

uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

uniform mat4 lightRot;

uniform vec4 lightColour2;
uniform vec3 lightPos2;
uniform float lightRadius2;

in Vertex {
	vec3 colour;
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

	vec2 yUV = IN.worldPos.xz / 64.0f;
	vec2 xUV = IN.worldPos.zy / 64.0f;
	vec2 zUV = IN.worldPos.xy / 64.0f;

	vec4 yDiffE = texture(diffuseTexE, yUV);
	vec4 xDiffE = texture(diffuseTexE, xUV);
	vec4 zDiffE = texture(diffuseTexE, zUV);

	vec4 yDiffG = texture(diffuseTexG, yUV);
	vec4 xDiffG = texture(diffuseTexG, xUV);
	vec4 zDiffG = texture(diffuseTexG, zUV);

	vec3 blendWeights = abs(IN.normal);
	blendWeights = blendWeights / (blendWeights.x + blendWeights.y + blendWeights.z);

	vec4 diffuseE = xDiffE * blendWeights.x + yDiffE * blendWeights.y + zDiffE * blendWeights.z;
	vec4 diffuseG = xDiffG * blendWeights.x + yDiffG * blendWeights.y + zDiffG * blendWeights.z;

	yDiffE = texture(bumpTexE, yUV);
	xDiffE = texture(bumpTexE, xUV);
	zDiffE = texture(bumpTexE, zUV);

	yDiffG = texture(bumpTexG, yUV);
	xDiffG = texture(bumpTexG, xUV);
	zDiffG = texture(bumpTexG, zUV);

	vec4 bumpE = xDiffE * blendWeights.x + yDiffE * blendWeights.y + zDiffE * blendWeights.z;
	vec4 bumpG = xDiffG * blendWeights.x + yDiffG * blendWeights.y + zDiffG * blendWeights.z;

	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));	

	vec3 bumpNormalE = normalize(TBN * normalize(bumpE.rgb * 2.0 - 1.0));
	vec3 bumpNormalG = normalize(TBN * normalize(bumpG.rgb * 2.0 - 1.0));
	
	float horizontal = IN.normal.x > IN.normal.z ? IN.normal.x : IN.normal.z;
	float ratio = horizontal / (horizontal + IN.normal.y);

	vec3 bumpNormal = mix(bumpNormalE, mix(bumpNormalE, bumpNormalG, ratio), IN.worldPos.y / 255);
	vec4 diffuse =  mix(diffuseE, mix(diffuseE, diffuseG, ratio), IN.worldPos.y / 255);

	float lambert = max(dot(incident , bumpNormal), 0.0f);
	float lambert2 = max(dot(incident2 , bumpNormal), 0.0f);
	float distance = length(lightPos - IN.worldPos);
	float distance2 = length(lightPos2 - IN.worldPos);
	float attenuation = 1.0f - clamp(distance / lightRadius ,0.0 ,1.0);
	float attenuation2 = 1.0f - clamp(distance2 / lightRadius2 ,0.0 ,1.0);

	float specFactor = clamp(dot(halfDir , bumpNormal) ,0.0 ,1.0);
	float specFactor2 = clamp(dot(halfDir2 , bumpNormal) ,0.0 ,1.0);
	specFactor = pow(specFactor , 60.0 );
	specFactor2 = pow(specFactor2 , 60.0 );
	vec3 surface = (diffuse.rgb * lightColour.rgb);
	vec3 surface2 = (diffuse.rgb * lightColour2.rgb);
	fragColour.rgb = surface * lambert * attenuation;
	fragColour.rgb += surface2 * lambert2 * attenuation2;
	fragColour.rgb += (lightColour.rgb * specFactor )* attenuation *0.33;
	fragColour.rgb += (lightColour2.rgb * specFactor2 )* attenuation2 *0.33;
	fragColour.rgb += surface * 0.1f;
	fragColour.a = 1;
}