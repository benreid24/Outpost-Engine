#include <Core/Arena/Generation/ProtoTerrain.hpp>

#include <BLIB/Logging.hpp>
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
            collapseQueue.push(PriorityNode(nodes(x, y)));
        }
    }
}

ProtoTerrain::PriorityNode::PriorityNode(SuperpositionedNode& node)
: node(&node)
, cachedWeight(node.domain.totalWeight()) {}

void ProtoTerrain::PriorityNode::update() {
    cachedWeight = node->domain.totalWeight();
    // ref.reposition();
}

bool ProtoTerrain::PriorityNode::operator<(const PriorityNode& right) const {
    return cachedWeight < right.cachedWeight;
}

SuperpositionedNode* ProtoTerrain::getMostConstrainedNode() {
    if (!collapseQueue.empty()) {
        SuperpositionedNode* result = collapseQueue.front().node;
        collapseQueue.pop();
        return result;
    }
    return nullptr;
}

ProtoTerrain::QueryResult ProtoTerrain::getNodeNeighbors(unsigned int x, unsigned int y) {
    return QueryResult{Iterator(*this, x, y)};
}

ProtoTerrain::Iterator::Iterator()
: terrain(nullptr)
, originX(0)
, originY(0)
, x(0)
, y(0) {}

ProtoTerrain::Iterator::Iterator(ProtoTerrain& terrain, unsigned int x, unsigned int y)
: terrain(&terrain)
, originX(x)
, originY(y)
, x(x)
, y(y) {
    if (x > 0 && y > 0) { // go up left if possible
        --this->x;
        --this->y;
    }
    else {
        if (y > 0) { --this->y; }      // next try go up only
        else if (x > 0) { --this->x; } // try go left only
        else {
            if (y < terrain.getNodesHeight() - 1) { ++this->y; }     // try go down only
            else if (x < terrain.getNodesWidth() - 1) { ++this->x; } // try go right only
            else { this->terrain = nullptr; }                        // no more neighbors
        }
    }
}

ProtoTerrain::Iterator& ProtoTerrain::Iterator::operator++() {
    // try to go right, if we go past right neighbor or hit edge then go down
    if (x < originX + 1 && x < terrain->getNodesWidth() - 1) { ++x; }
    else {
        if (y < originY + 1 && y < terrain->getNodesHeight() - 1) {
            ++y;
            x = originX > 0 ? originX - 1 : originX;
        }
        else { terrain = nullptr; } // no more neighbors
    }

    // skip over origin
    if (terrain && x == originX && y == originY) { ++(*this); }

    return *this;
}

ProtoTerrain::Iterator ProtoTerrain::Iterator::operator++(int) {
    Iterator copy = *this;
    ++(*this);
    return copy;
}

} // namespace gen
} // namespace arena
} // namespace core
