#ifndef CORE_ARENA_GENERATION_PROTOTERRAIN_HPP
#define CORE_ARENA_GENERATION_PROTOTERRAIN_HPP

#include <BLIB/Containers/Vector2d.hpp>
#include <Core/Arena/Generation/SuperpositionedNode.hpp>
#include <queue>

namespace core
{
namespace arena
{
namespace gen
{
class Generator;

/**
 * @brief Represents transient terrain during arena generation
 *
 * @ingroup Arena
 */
class ProtoTerrain {
public:
    /**
     * @brief Special iterator for superpositioned nodes in a ProtoTerrain
     */
    class Iterator {
    public:
        /**
         * @brief Creates an invalid iterator
         */
        Iterator();

        /**
         * @brief Creates an iterator for the given range
         *
         * @param terrain The terrain to iterate over
         * @param x The x index of the node to iterate around
         * @param y The y index of the node to iterate around
         */
        Iterator(ProtoTerrain& terrain, unsigned int x, unsigned int y);

        /**
         * @brief Dereferences the iterator
         */
        SuperpositionedNode& operator*() { return terrain->getNode(x, y); }

        /**
         * @brief Dereferences the iterator
         */
        SuperpositionedNode* operator->() { return &terrain->getNode(x, y); }

        /**
         * @brief Increments the iterator
         */
        Iterator& operator++();

        /**
         * @brief Increments the iterator
         */
        Iterator operator++(int);

        /**
         * @brief Returns whether the iterator is valid
         */
        operator bool() const { return terrain != nullptr; }

    private:
        ProtoTerrain* terrain;
        unsigned int originX;
        unsigned int originY;
        unsigned int x;
        unsigned int y;
    };

    /**
     * @brief Helper struct for iterating over nodes
     */
    struct QueryResult {
        Iterator start;

        /**
         * @brief Returns the first iterator
         */
        Iterator begin() { return start; }

        /**
         * @brief Returns the end iterator
         */
        Iterator end() { return Iterator(); }
    };

    /**
     * @brief Creates empty proto terrain
     */
    ProtoTerrain();

    /**
     * @brief Creates the initial set of superpositioned nodes. Does not apply constraints
     *
     * @param generator The generator to use for creating the nodes
     */
    void populate(Generator& generator);

    /**
     * @brief Returns the width of the node grid
     */
    unsigned int getNodesWidth() const { return nodes.getWidth(); }

    /**
     * @brief Returns the height of the node grid
     */
    unsigned int getNodesHeight() const { return nodes.getHeight(); }

    /**
     * @brief Returns the node at the given position. Does not validate the position
     *
     * @param x The x index of the node
     * @param y The y index of the node
     * @return A reference to the node at the given position
     */
    SuperpositionedNode& getNode(unsigned int x, unsigned int y) { return nodes(x, y); }

    /**
     * @brief Finds and returns the most constrained node
     */
    SuperpositionedNode* getMostConstrainedNode();

    /**
     * @brief Builsd the priority queue for node domain collapse
     */
    void buildPriorityQueue();

    /**
     * @brief Returns the neighbors of the node at the given position
     *
     * @param x The x index of the node to get neighbors for
     * @param y The y index of the node to get neighbors for
     * @return The query result that can be iterated over
     */
    QueryResult getNodeNeighbors(unsigned int x, unsigned int y);

private:
    struct PriorityNode {
        SuperpositionedNode* node;
        float cachedWeight;

        PriorityNode(SuperpositionedNode& node);
        void update();
        bool operator<(const PriorityNode& node) const;
    };

    bl::ctr::Vector2D<SuperpositionedNode> nodes;
    // TODO - will need different data structure if we can change nodes
    std::priority_queue<PriorityNode, std::vector<PriorityNode>> collapseQueue;
};

} // namespace gen
} // namespace arena
} // namespace core

#endif
