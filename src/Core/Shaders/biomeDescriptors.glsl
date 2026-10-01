#ifndef BIOME_DESCRIPTORS_INCLUDED
#define BIOME_DESCRIPTORS_INCLUDED

#include <terrainTypes.glsl>

layout(std430, set = 2, binding = 0) uniform biome_descriptor_buffer {
    BiomeDescriptor biomeDescriptors[8];
} biomeDescriptorBuffer;

#endif
