#include <Core/Arena/Terrain.hpp>

namespace core
{
namespace arena
{
namespace
{
constexpr float TerrainStep           = 1.f;
constexpr unsigned int TerrainOctaves = 16;
constexpr float TerrainPersistence    = 0.4f;
constexpr float TerrainFrequency      = 0.005f;

struct TerrainColor {
    float minHeight;
    float maxHeight;
    bl::rc::Color color;
};

const std::array<TerrainColor, 6> TerrainColors = {{
    {0.f, 0.43f, bl::rc::Color(0.f, 0.f, 0.6f)},          // deep water
    {0.43f, 0.45f, bl::rc::Color(0.f, 0.1f, 0.95f)},      // water
    {0.45f, 0.47f, bl::rc::Color(0.965f, 0.828f, 0.45f)}, // sand
    {0.47f, 0.55f, bl::rc::Color(0.2f, 0.8f, 0.2f)},      // grass
    {0.55f, 0.65f, bl::rc::Color(0.31f, 0.31f, 0.26f)},   // rock
    {0.65f, 1.1f, bl::rc::Color(1.5f, 1.5f, 1.5f)}        // snow
}};
constexpr float WaterHeight                     = 0.45f;

const std::unordered_map<gen::Biome, bl::rc::Color> BiomeColors = {
    std::make_pair(gen::Biome::Desert, bl::rc::Color(0.965f, 0.828f, 0.45f)),
    std::make_pair(gen::Biome::Forest, bl::rc::Color(0.15f, 0.68f, 0.1f)),
    std::make_pair(gen::Biome::Grassland, bl::rc::Color(0.2f, 0.88f, 0.2f)),
    std::make_pair(gen::Biome::Mountain, bl::rc::Color(0.31f, 0.31f, 0.26f)),
    std::make_pair(gen::Biome::Snow, bl::rc::Color(1.5f, 1.5f, 1.5f)),
    std::make_pair(gen::Biome::Beach, bl::rc::Color(0.965f, 0.828f, 0.45f)),
    std::make_pair(gen::Biome::Water, bl::rc::Color(0.f, 0.f, 0.6f)),
    std::make_pair(gen::Biome::River, bl::rc::Color(0.f, 0.1f, 0.95f))};
} // namespace

Terrain::Terrain() {}

float Terrain::sampleHeight(const glm::vec2& pos) const {
    const glm::vec2 normalPos = pos / worldSize + glm::vec2(0.5f, 0.5f);
    if (normalPos.x < 0.f || normalPos.x > 1.f || normalPos.y < 0.f || normalPos.y > 1.f) {
        return 0.f;
    }
    const float xi        = normalPos.x * static_cast<float>(heightmap.getWidth() - 1);
    const float yi        = normalPos.y * static_cast<float>(heightmap.getHeight() - 1);
    const unsigned int x0 = std::floor(xi) + 0.01f;
    const unsigned int y0 = std::floor(yi) + 0.01f;
    const unsigned int x1 = std::min(x0 + 1, heightmap.getWidth() - 1);
    const unsigned int y1 = std::min(y0 + 1, heightmap.getHeight() - 1);
    const float xn        = xi - std::floor(xi);
    const float yn        = yi - std::floor(yi);
    const float h00       = heightmap(x0, y0);
    const float h10       = heightmap(x1, y0);
    const float h01       = heightmap(x0, y1);
    const float h11       = heightmap(x1, y1);
    return h00 * (1.f - xn) * (1.f - yn) + h10 * xn * (1.f - yn) + h01 * (1.f - xn) * yn +
           h11 * xn * yn;
}

glm::u32vec2 Terrain::worldPosToIndex(const glm::vec2& pos) const {
    const glm::vec2 normalPos = pos / worldSize + glm::vec2(0.5f, 0.5f);
    const float xi            = normalPos.x * static_cast<float>(heightmap.getWidth() - 1);
    const float yi            = normalPos.y * static_cast<float>(heightmap.getHeight() - 1);
    return {xi, yi};
}

void Terrain::addToWorld(bl::engine::World& world) {
    terrainDrawable.createFromHeightmap(
        world, heightmap, glm::vec2(-worldSize.x * 0.5f, -worldSize.y * 0.5f), worldSize);
    postprocess();
    terrainDrawable.addToScene(world.scene(), bl::rc::UpdateSpeed::Static);
}

void Terrain::postprocess() {
    if (terrainDrawable.exists()) {
        terrainDrawable.updateFromHeightmap(
            heightmap, glm::vec2(-worldSize.x * 0.5f, -worldSize.y * 0.5f), worldSize);
    }

    auto& verts = terrainDrawable.component().gpuBuffer.vertices();
    for (auto& v : verts) {
        v.pos.y *= maxHeight;
        const glm::u32vec2 i = worldPosToIndex({v.pos.x, v.pos.z});
        const auto it        = BiomeColors.find(nodes(i.x, i.y).biome);
        if (it != BiomeColors.end()) { v.color = it->second; }
        else { v.color = bl::rc::Color(0.f, 0.f, 0.f); }
    }
    terrainDrawable.commitUpdate();
}

} // namespace arena
} // namespace core
