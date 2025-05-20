#version 330 core

layout(triangles) in;  
layout(triangle_strip, max_vertices = 42) out; 

in Vertex {
    vec2 texCoord;
    vec4 colour;
    vec3 normal;
    vec3 tangent; 
    vec3 binormal;
    vec3 worldPos;
    vec4 shadowProj;
} IN[3];

out Vertex {
    vec2 texCoord;  
    vec4 colour; 
    vec3 normal;
    vec3 tangent; 
    vec3 binormal; 
    vec3 worldPos;
    vec4 shadowProj;
} OUT;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 cameraPosition;

uniform float grassHeight;        
uniform float bladeWidth;         
uniform vec4 colourBase;  
uniform vec4 colourTop; 
uniform bool useGrassColour;

uniform sampler2D windMap;
uniform float windTraslate;
uniform float windStrength;

uniform int numSegments = 5;
const float leaningFactor= 0.3;
    
vec4 toClipSpace(vec3 coord){
    return projMatrix * viewMatrix * vec4(coord, 1.0);
}

float rand(vec3 vec){
    return fract(sin(dot(vec, vec3(12.9898, 78.233, 54.53))) * 43758.5453);
}

float rand2(vec3 vec){
    return fract(sin(dot(vec, vec3(39.346, 11.135, 83.155))) * 58934.2329);
}
 
vec3 bezier(vec3 p0, vec3 p1, vec3 p2, float t){
    vec3 a = mix(p0, p1, t);
    vec3 b = mix(p1, p2, t);
    return mix(a, b, t);
}

vec3 bezierDerivative(vec3 p0, vec3 p1, vec3 p2, float t){
    return 2.0 * (1.0 - t) * (p1 - p0) + 2.0 * t * (p2 - p1);
}

// Jahrmann & Wimmer, "Responsive real-time grass rendering," I3D '17, 2017.
void MakePersistentLength(in vec3 v0, inout vec3 v1, inout vec3 v2, in float height)
{
    //Persistent length
    vec3 v01 = v1 - v0;
    vec3 v12 = v2 - v1;
    float lv01 = length(v01);
    float lv12 = length(v12);

    float L1 = lv01 + lv12;
    float L0 = length(v2-v0);
    float L = (2.0f * L0 + L1) / 3.0f; //http://steve.hollasch.net/cgindex/curves/cbezarclen.html

    float ldiff = height / L;
    v01 = v01 * ldiff;
    v12 = v12 * ldiff;
    v1 = v0 + v01;
    v2 = v1 + v12;
}

void main(void) {
	// Get pseudorandom position and values
    int idx = 0;
    float randValue = rand(IN[idx].worldPos);
	idx = int(randValue * 3.0);
    if (idx > 2) idx = 2;
    float randValue2 = rand2(IN[idx].worldPos);
    
    vec4 colourBladeTop = useGrassColour ? colourTop : IN[0].colour;
    vec4 colourBladeBase = useGrassColour ? colourBase : IN[0].colour;

    float randomAngle = randValue * 2.0 * 3.14159; 
    float randomTilt = leaningFactor * (0.7 + randValue2 * 0.6);
    vec3 bladeDir = vec3(cos(randomAngle), 0.0, sin(randomAngle));
   
    float height = (grassHeight + 10.0) * (0.8 + randValue2 * 0.4); 
    float width = bladeWidth * (0.7 + randValue * 0.6);   
	
    // Create bezier control points
	vec3 surfaceNormal = IN[idx].normal;
    vec3 p0 = IN[idx].worldPos;   
    vec3 p1 = p0 + surfaceNormal * height;
    vec3 p2 = p1 + bladeDir * height * randomTilt;
    
    // wind 
    vec4 windSample = (texture(windMap, IN[idx].texCoord / 50.0 + windTraslate) * 2.0 - 1.0);
    vec3 wind = normalize(vec3(windSample.r, 0.0, windSample.g));
    p2 += wind * windStrength;
    
    MakePersistentLength(p0, p1, p2, height);
	vec3 bladeRight = normalize(vec3(bladeDir.z, 0.0, -bladeDir.x)); // Perpendicular offset for points
	
	// Vertex order check
    vec3 viewDirection = normalize(cameraPosition - p0);
    vec3 faceNormal = cross(bladeRight, surfaceNormal);
    float facingCamera = dot(viewDirection, faceNormal);
    
    if (facingCamera < 0.0) {
        faceNormal = -faceNormal;
    }
  
    // Generate blade segments
    for (int i = 0; i <= numSegments; ++i) {
        float t = float(i) / float(numSegments);
   
        vec3 centralPoint = bezier(p0, p1, p2, t);
        float taperWidth = width * (1.0 - pow(t, 1.5));
        
        vec3 vertRight = centralPoint + bladeRight * (taperWidth / 2.0);
        vec3 vertLeft = centralPoint - bladeRight * (taperWidth / 2.0);
        
        // Normal for lighting using derivative
        vec3 segmentNormal = normalize(cross(bladeRight, bezierDerivative(p0, p1, p2, t)));
		if (dot(segmentNormal, faceNormal) < 0.0) {
            segmentNormal = -segmentNormal; // Keep normal consistent across all blade
        }
        
        OUT.colour = mix(colourBladeBase, colourBladeTop, t);
        
        OUT.worldPos = vertRight;
        OUT.normal = segmentNormal;
        OUT.texCoord = IN[idx].texCoord;
        OUT.shadowProj = IN[idx].shadowProj;

        gl_Position = toClipSpace(vertRight);
        EmitVertex();
        
        // Emit left vertex
        OUT.worldPos = vertLeft;
        gl_Position = toClipSpace(vertLeft);
        EmitVertex();
    }
    
    EndPrimitive();
}