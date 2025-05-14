#version 330 core

uniform sampler2D diffuseTex;
in Vertex {
    vec2 texCoord;  
    vec4 colour; 
	vec3 normal;
    vec3 tangent; 
    vec3 binormal; 
    vec3 worldPos;
	vec4 shadowProj;
} IN;

out vec4 fragColour;
uniform vec3 VertexScale = vec3(1.0, 1.0, 1.0);
uniform vec3 cameraPosition;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform sampler2D shadowTex;


void main(void) {
	vec3 scaledLightPos = max(VertexScale, vec3(1, 0.1, 1)) * lightPos;
	float scaledLightRad = VertexScale.x * lightRadius * 0.75;
	
	vec3 incident = normalize(scaledLightPos - IN.worldPos);
    vec3 viewDir = normalize(cameraPosition - IN.worldPos);
    vec3 halfDir = normalize(incident + viewDir);

    vec4 diffuse = texture(diffuseTex, IN.texCoord);
	
	float lambert = max(dot(incident, IN.normal), 0.0f);
	float distance = length(scaledLightPos - IN.worldPos);
	float attenuation = 1.0 - clamp(distance / scaledLightRad, 0.0, 1.0);
	float specFactor = clamp(dot(halfDir, IN.normal), 0.0, 1.0);
	specFactor = pow(specFactor, 4.0);
	
	vec4 groundColour;
	if (length(IN.colour.rgb) > 0.1) {
			groundColour = mix(diffuse, IN.colour, 0.45);
		} 
	else {
		groundColour = diffuse;
	}   
	
	float shadow = 1.0; // New!
	vec3 shadowNDC = IN.shadowProj.xyz / IN.shadowProj.w;
	if (abs(shadowNDC.x) < 1.0 &&
		abs(shadowNDC.y) < 1.0 &&
		abs(shadowNDC.z) < 1.0) {
		vec3 biasCoord = shadowNDC * 0.5 + 0.5;
		float shadowZ = texture(shadowTex, biasCoord.xy).x;
		if (shadowZ < biasCoord.z) {
			shadow = 0.0;
		}
	}
	
    vec3 surface = (groundColour.rgb * lightColour.rgb);
    fragColour.rgb = surface * lambert * attenuation;
    fragColour.rgb += (lightColour.rgb * specFactor) * attenuation * 0.2;
	fragColour.rgb *= shadow;
    fragColour.rgb += surface * 0.65;
	
    fragColour.a = diffuse.a;
}
