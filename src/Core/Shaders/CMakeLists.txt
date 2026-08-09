include(compile_shaders)
compile_shaders(
    TARGET Core
    SHADER_MODULE core_shaders

    SHADERS
    terrainSolid.vert
    terrainSolidShadow.vert
    terrainSolid.frag
)
