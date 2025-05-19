#version 400 core

layout (vertices = 3) out; // Number of vertices in output patch

in Vertex {
    vec2 texCoord;
    vec4 colour;
	vec3 normal;
    vec3 tangent; 
    vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
} IN[];

out Vertex {
    vec2 texCoord;
    vec4 colour;
	vec3 normal;
    vec3 tangent; 
    vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
} OUT[];

uniform vec3 VertexScale;
uniform mat4 viewMatrix;
uniform vec3 cameraPosition;
uniform bool colourMode = false;

// Configurable tessellation parameters
const float MAX_TESS_LEVEL = 3.0;        // Maximum tessellation for closest geometry
const float MIN_TESS_LEVEL = 1.5;         // Minimum tessellation for distant geometry

const float NEAR_DISTANCE = 5000.0;        // Distance for maximum tessellation
const float FAR_DISTANCE = 30000000.0;        // Distance for minimum tessellation


float GetTessLevel(float distance0, float distance1)
{
	float avgDistance = (distance0 + distance1) * 0.5;
	float tessLevel;
	
    if (avgDistance <= NEAR_DISTANCE) {
        tessLevel = MAX_TESS_LEVEL;
    } 
    else if (avgDistance >= FAR_DISTANCE) {
        tessLevel = MIN_TESS_LEVEL;
    }
    else {
        float t = (avgDistance - NEAR_DISTANCE) / (FAR_DISTANCE - NEAR_DISTANCE);
        tessLevel = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, t);
    }
    
	tessLevel *= VertexScale.x * 0.04;
    return tessLevel;
}  

void main() {
	float distanceSquared0 = dot(cameraPosition - IN[0].worldPos, cameraPosition - IN[0].worldPos);
	float distanceSquared1 = dot(cameraPosition - IN[1].worldPos, cameraPosition - IN[1].worldPos);
	float distanceSquared2 = dot(cameraPosition - IN[2].worldPos, cameraPosition - IN[2].worldPos);

    gl_TessLevelOuter[0] = GetTessLevel(distanceSquared1, distanceSquared2);
    gl_TessLevelOuter[1] = GetTessLevel(distanceSquared2, distanceSquared0);
    gl_TessLevelOuter[2] = GetTessLevel(distanceSquared0, distanceSquared1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];

	OUT[gl_InvocationID].texCoord = IN[gl_InvocationID].texCoord;
    //OUT[gl_InvocationID].colour = IN[gl_InvocationID].colour;

	float normalizedLevel = (gl_TessLevelInner[0] - MIN_TESS_LEVEL) / (MAX_TESS_LEVEL - MIN_TESS_LEVEL);
    OUT[gl_InvocationID].colour = mix(vec4(0.0, 0.0, 1.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), colourMode ? normalizedLevel : 1.0/ 0.0); // undefined but looks good somehow?
	
    OUT[gl_InvocationID].normal = IN[gl_InvocationID].normal;
    OUT[gl_InvocationID].tangent = IN[gl_InvocationID].tangent;
    OUT[gl_InvocationID].binormal = IN[gl_InvocationID].binormal;
    OUT[gl_InvocationID].worldPos = IN[gl_InvocationID].worldPos;
    
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
