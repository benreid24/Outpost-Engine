#include <Core/Arena/Terrain.hpp>

namespace core
{
namespace arena
{
namespace
{
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

Terrain::Terrain()
: step(1.f) {}

float Terrain::sampleHeight(const glm::vec2& pos) const {
    const auto nodes  = sampleBiomes(pos);
    float waterHeight = 0.f;
    for (const auto& n : nodes) {
        if (n.biome == gen::Biome::Water) { waterHeight = n.height; }
    }
    const float h = std::max(heightSampler.sample(pos + worldSize * 0.5f), waterHeight);
    return h * maxHeight;
}

bl::ctr::StaticVector<Terrain::SampledBiome, 4> Terrain::sampleBiomes(const glm::vec2& pos) const {
    const auto weightedPositions = getPositionWeights(pos);
    bl::ctr::StaticVector<SampledBiome, 4> result;
    for (const auto& wp : weightedPositions) {
        const Node& node = nodes(wp.first.x, wp.first.y);
        for (auto& sample : result) {
            if (sample.biome == node.biome) {
                const float tw = sample.weight + wp.second;
                sample.height  = sample.height * sample.weight / tw + node.height * wp.second / tw;
                sample.weight += wp.second;
                goto next;
            }
        }
        result.emplace_back(SampledBiome{node.biome, node.height, wp.second});
    next:;
    }
    return result;
}

std::array<std::pair<glm::u32vec2, float>, 4> Terrain::getPositionWeights(
    const glm::vec2& pos) const {
    const glm::vec2 normalPos = pos / worldSize + glm::vec2(0.5f, 0.5f);
    if (normalPos.x < 0.f || normalPos.x > 1.f || normalPos.y < 0.f || normalPos.y > 1.f) {
        return {};
    }
    const float xi        = normalPos.x * static_cast<float>(nodes.getWidth() - 1);
    const float yi        = normalPos.y * static_cast<float>(nodes.getHeight() - 1);
    const unsigned int x0 = std::floor(xi) + 0.01f;
    const unsigned int y0 = std::floor(yi) + 0.01f;
    const unsigned int x1 = std::min(x0 + 1, nodes.getWidth() - 1);
    const unsigned int y1 = std::min(y0 + 1, nodes.getHeight() - 1);
    const float xn        = xi - std::floor(xi);
    const float yn        = yi - std::floor(yi);

    const float d00 = std::sqrt(xn * xn + yn * yn);
    const float d10 = std::sqrt((1.f - xn) * (1.f - xn) + yn * yn);
    const float d01 = std::sqrt(xn * xn + (1.f - yn) * (1.f - yn));
    const float d11 = std::sqrt((1.f - xn) * (1.f - xn) + (1.f - yn) * (1.f - yn));

    // handle the case where the sample lands exactly on a node
    constexpr float Epsilon = 1e-6f;
    if (d00 < Epsilon) {
        return {{{{x0, y0}, 1.f}, {{x1, y0}, 0.f}, {{x0, y1}, 0.f}, {{x1, y1}, 0.f}}};
    }
    if (d10 < Epsilon) {
        return {{{{x0, y0}, 0.f}, {{x1, y0}, 1.f}, {{x0, y1}, 0.f}, {{x1, y1}, 0.f}}};
    }
    if (d01 < Epsilon) {
        return {{{{x0, y0}, 0.f}, {{x1, y0}, 0.f}, {{x0, y1}, 1.f}, {{x1, y1}, 0.f}}};
    }
    if (d11 < Epsilon) {
        return {{{{x0, y0}, 0.f}, {{x1, y0}, 0.f}, {{x0, y1}, 0.f}, {{x1, y1}, 1.f}}};
    }

    const float w00    = 1.f / d00;
    const float w10    = 1.f / d10;
    const float w01    = 1.f / d01;
    const float w11    = 1.f / d11;
    const float wTotal = w00 + w10 + w01 + w11;

    return {{{{x0, y0}, w00 / wTotal},
             {{x1, y0}, w10 / wTotal},
             {{x0, y1}, w01 / wTotal},
             {{x1, y1}, w11 / wTotal}}};
}

glm::u32vec2 Terrain::worldPosToIndex(const glm::vec2& pos) const {
    const glm::vec2 normalPos = pos / worldSize + glm::vec2(0.5f, 0.5f);
    const float xi            = normalPos.x * static_cast<float>(nodes.getWidth() - 1);
    const float yi            = normalPos.y * static_cast<float>(nodes.getHeight() - 1);
    return {xi, yi};
}

void Terrain::addToWorld(bl::engine::World& world, float s) {
    step = s;
    terrainDrawable.createFromHeightFunction(
        world,
        [this](const glm::vec2& pos) { return sampleHeight(pos); },
        glm::vec2(-worldSize.x * 0.5f, -worldSize.y * 0.5f),
        worldSize,
        step);
    postprocess();
    terrainDrawable.addToScene(world.scene(), bl::rc::UpdateSpeed::Static);
}

void Terrain::postprocess() {
    if (terrainDrawable.exists()) {
        terrainDrawable.updateFromHeightFunction(
            [this](const glm::vec2& pos) { return sampleHeight(pos); },
            glm::vec2(-worldSize.x * 0.5f, -worldSize.y * 0.5f),
            worldSize,
            step);
    }

    auto& verts = terrainDrawable.component().gpuBuffer.vertices();
    for (auto& v : verts) {
        v.color     = bl::rc::Color(0.f, 0.f, 0.f);
        auto biomes = sampleBiomes({v.pos.x, v.pos.z});
        for (const auto& b : biomes) {
            const auto it = BiomeColors.find(b.biome);
            if (it != BiomeColors.end()) { v.color += it->second.toVec4() * b.weight; }
        }
    }
    terrainDrawable.commitUpdate();
}

} // namespace arena
} // namespace core
