#include <Core/Arena/Generation/ProtoTerrain.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Arena/Generation/Environment.hpp>
#include <Core/Arena/Generation/Generator.hpp>

namespace core
{
namespace arena
{
namespace gen
{
ProtoTerrain::ProtoTerrain()
: nodes() {}

void ProtoTerrain::populate(Generator& generator) {
    const glm::u32vec2 size(generator.getHeightmap().getWidth(),
                            generator.getHeightmap().getHeight());

    nodes.setSize(size.x, size.y);
    for (unsigned int y = 0; y < size.y; ++y) {
        for (unsigned int x = 0; x < size.x; ++x) {
            nodes(x, y) = SuperpositionedNode(glm::u32vec2(x, y),
                                              generator.getHeightmap()(x, y),
                                              generator.getMoistureMap()(x, y));
        }
    }
}

void ProtoTerrain::buildPriorityQueue() {
    for (unsigned int x = 0; x < nodes.getWidth(); ++x) {
        for (unsigned int y = 0; y < nodes.getHeight(); ++y) {
            auto ref                 = collapseQueue.push(&nodes(x, y));
            (*ref)->priorityQueueRef = ref;
        }
    }
}

SuperpositionedNode* ProtoTerrain::getMostConstrainedNode() {
    if (!collapseQueue.empty()) {
        SuperpositionedNode* result = collapseQueue.front();
        collapseQueue.pop();
        return result;
    }
    return nullptr;
}

ProtoTerrain::QueryResult ProtoTerrain::getNodeNeighbors(unsigned int x, unsigned int y) {
    return QueryResult{Iterator(*this, x, y)};
}

namespace
{
constexpr std::array<std::pair<int, int>, 8> Offsets(
    {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}});
}

ProtoTerrain::Iterator::Iterator()
: terrain(nullptr)
, originX(0)
, originY(0)
, i(0) {}

ProtoTerrain::Iterator::Iterator(ProtoTerrain& terrain, unsigned int x, unsigned int y)
: terrain(&terrain)
, originX(x)
, originY(y)
, i(0) {
    checkIndex();
}

void ProtoTerrain::Iterator::checkIndex() {
    while (i < Offsets.size()) {
        const int nx = static_cast<int>(originX) + Offsets[i].first;
        const int ny = static_cast<int>(originY) + Offsets[i].second;

        if (nx >= 0 && ny >= 0 && nx < static_cast<int>(terrain->getNodesWidth()) &&
            ny < static_cast<int>(terrain->getNodesHeight())) {
            return;
        }

        ++i;
    }

    terrain = nullptr;
}

ProtoTerrain::Iterator& ProtoTerrain::Iterator::operator++() {
    ++i;
    checkIndex();
    return *this;
}

ProtoTerrain::Iterator ProtoTerrain::Iterator::operator++(int) {
    Iterator copy = *this;
    ++(*this);
    return copy;
}

SuperpositionedNode& ProtoTerrain::Iterator::operator*() {
    const unsigned int nx = static_cast<unsigned int>(static_cast<int>(originX) + Offsets[i].first);
    const unsigned int ny =
        static_cast<unsigned int>(static_cast<int>(originY) + Offsets[i].second);
    return terrain->getNode(nx, ny);
}

SuperpositionedNode* ProtoTerrain::Iterator::operator->() {
    const unsigned int nx = static_cast<unsigned int>(static_cast<int>(originX) + Offsets[i].first);
    const unsigned int ny =
        static_cast<unsigned int>(static_cast<int>(originY) + Offsets[i].second);
    return &terrain->getNode(nx, ny);
}

} // namespace gen
} // namespace arena
} // namespace core
