#version 450
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in FS_IN {
    vec3 fragPos;
    vec4 fragColor;
    vec2 texCoords;
    mat3 TBN;
    float biomeWeights[8];
} fs_in;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outSpecular;
layout(location = 2) out vec4 outPosition;
layout(location = 3) out vec4 outNormal;

#define GLOBALS_SET_NUMBER 0
#define SCENE_SET_NUMBER 1
#include "3D/Helpers/uniforms.glsl"

#include <biomeDescriptors.glsl>

#include "3D/Helpers/constants.glsl"
#include "3D/Helpers/parallaxMap.glsl"

void main() {
    vec4 diffuseColor = vec4(0.0);
    vec3 specular = vec3(0.0);
    vec3 normal = vec3(0.0);
    float shininess = 0.0;

    vec3 viewDir = normalize(camera.camPos - fs_in.fragPos);

    for (int i = 0; i < 8; ++i) {
        float biomeWeight = fs_in.biomeWeights[i];
        if (biomeWeight <= 0.0) {
            continue;
        }

        BiomeDescriptor biome = biomeDescriptorBuffer.biomeDescriptors[i];
        Material material = materials.pool[biome.materialIndex];

        vec2 texCoords = parallaxMap(material, viewDir, fs_in.TBN);
        if (material.heightScale > 0.0) {
            if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0) {
                continue;
            }
        }

        uint diffuseIndex = material.diffuseId;
        vec4 diffuse = texture(textures[diffuseIndex], texCoords);
        if (diffuse.a == 0.0) {
            continue;
        }
        diffuseColor += diffuse * biomeWeight;

        vec3 spec = texture(textures[material.specularId], texCoords).xyz;
        specular += spec * biomeWeight;

        uint normalIndex = material.normalId;
        vec3 n = texture(textures[normalIndex], texCoords).rgb;
        n = normalize(n * 2.0 - 1.0);
        normal += n * biomeWeight;

        shininess += material.shininess * biomeWeight;
    }

    outColor = fs_in.fragColor * diffuseColor;
    outSpecular = vec4(fs_in.fragColor.xyz * specular, shininess);
    outPosition = vec4(fs_in.fragPos, float(lightingEnabled));
    outNormal.xyz = normalize(fs_in.TBN * normal);
}
