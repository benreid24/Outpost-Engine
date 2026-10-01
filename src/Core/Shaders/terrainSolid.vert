#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inNormal;
layout(location = 5) in uvec4 inBiomeIndices;
layout(location = 6) in vec4 inBiomeWeights;

#include <terrainTypes.glsl>

layout(location = 0) out VS_OUT {
    vec3 fragPos;
    vec4 fragColor;
    vec2 texCoords;
    mat3 TBN;
    float biomeWeights[8];
} vs_out;

#define SCENE_SET_NUMBER 1
#include "3D/Helpers/uniforms.glsl"

void main() {
    vec4 inPos = vec4(inPosition, 1.0);

	gl_Position = camera.projection * camera.view * inPos;
    vs_out.fragPos = inPosition;
	vs_out.fragColor = inColor;
	vs_out.texCoords = inTexCoords;
    
    for (int i = 0; i < 8; ++i) {
        vs_out.biomeWeights[i] = 0.0;
    }
    for (int i = 0; i < 4; ++i) {
        vs_out.biomeWeights[inBiomeIndices[i]] += inBiomeWeights[i];
    }
    
    vec3 T = inTangent;
    vec3 N = inNormal;
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(T, N);
    vs_out.TBN = mat3(T, B, N);
}
