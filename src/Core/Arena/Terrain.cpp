#include <Core/Arena/Terrain.hpp>

#include <Core/Rendering/PipelineIds.hpp>

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
    const glm::vec2 normalPos = pos / worldSize + glm::vec2(0.5f);
    if (normalPos.x < 0.f || normalPos.x > 1.f || normalPos.y < 0.f || normalPos.y > 1.f) {
        return {};
    }
    const float xi        = normalPos.x * static_cast<float>(nodes.getWidth() - 1);
    const float yi        = normalPos.y * static_cast<float>(nodes.getHeight() - 1);
    const unsigned int x0 = static_cast<unsigned int>(std::floor(xi));
    const unsigned int y0 = static_cast<unsigned int>(std::floor(yi));
    const unsigned int x1 = std::min(x0 + 1, nodes.getWidth() - 1);
    const unsigned int y1 = std::min(y0 + 1, nodes.getHeight() - 1);
    const float xn        = xi - static_cast<float>(x0);
    const float yn        = yi - static_cast<float>(y0);
    const float w00       = (1.f - xn) * (1.f - yn);
    const float w10       = xn * (1.f - yn);
    const float w01       = (1.f - xn) * yn;
    const float w11       = xn * yn;
    return {{{{x0, y0}, w00}, {{x1, y0}, w10}, {{x0, y1}, w01}, {{x1, y1}, w11}}};
}

glm::u32vec2 Terrain::worldPosToIndex(const glm::vec2& pos) const {
    const glm::vec2 normalPos = pos / worldSize + glm::vec2(0.5f, 0.5f);
    const float xi            = normalPos.x * static_cast<float>(nodes.getWidth() - 1);
    const float yi            = normalPos.y * static_cast<float>(nodes.getHeight() - 1);
    return {xi, yi};
}

void Terrain::addToWorld(bl::engine::World& world, float s) {
    step = s;
    terrainDrawable.createEmptyGrid(world,
                                    glm::vec2(-worldSize.x * 0.5f, -worldSize.y * 0.5f),
                                    worldSize,
                                    step,
                                    {},
                                    render::MaterialPipelineIds::SolidTerrain);
    generateGeometry(world.engine().engineLoopThreadpool());
    terrainDrawable.addToScene(world.scene(), bl::rc::UpdateSpeed::Static);
}

void Terrain::generateGeometry(bl::util::ThreadPool& threadPool) {
    if (!terrainDrawable.exists()) { return; }

    auto& verts = terrainDrawable.component().gpuBuffer.vertices();
    std::list<std::future<void>> futures;
    const unsigned int chunkSize = verts.size() / threadPool.threadCount();
    const unsigned int chunks = verts.size() / chunkSize + (verts.size() % chunkSize != 0 ? 1 : 0);
    for (unsigned int i = 0; i < chunks; ++i) {
        futures.emplace_back(threadPool.queueTask([this, &verts, i, chunkSize] {
            const unsigned int start = i * chunkSize;
            const unsigned int end =
                std::min(start + chunkSize, static_cast<unsigned int>(verts.size()));
            for (unsigned int j = start; j < end; ++j) {
                auto& v        = verts[j];
                v.pos.y        = sampleHeight({v.pos.x, v.pos.z});
                v.texCoord.x   = v.pos.x / step / 4.f;
                v.texCoord.y   = v.pos.z / step / 4.f;
                v.color        = bl::rc::Color(1.f, 1.f, 1.f);
                v.biomeIndices = glm::u32vec4(0);
                v.biomeWeights = glm::vec4(0.f);
                auto biomes    = sampleBiomes({v.pos.x, v.pos.z});
                unsigned int i = 0;
                for (const auto& b : biomes) {
                    v.biomeIndices[i] = static_cast<std::uint32_t>(b.biome);
                    v.biomeWeights[i] = b.weight;
                    ++i;
                }
            }
        }));
    }

    for (auto& f : futures) { f.wait(); }

    terrainDrawable.commitUpdate();
}

} // namespace arena
} // namespace core
