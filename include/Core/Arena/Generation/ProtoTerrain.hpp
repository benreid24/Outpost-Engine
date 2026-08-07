#ifndef CORE_ARENA_GENERATION_PROTOTERRAIN_HPP
#define CORE_ARENA_GENERATION_PROTOTERRAIN_HPP

#include <BLIB/Containers/PriorityQueue.hpp>
#include <BLIB/Containers/Vector2d.hpp>
#include <Core/Arena/Generation/SuperpositionedNode.hpp>

namespace core
{
namespace arena
{
namespace gen
{
class Generator;
class Environment;

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
        SuperpositionedNode& operator*();

        /**
         * @brief Dereferences the iterator
         */
        SuperpositionedNode* operator->();

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
        unsigned int i;

        void checkIndex();
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

    /**
     * @brief Recomputes the adjacency bonuses for the given node and updates the priority queue
     *
     * @param environment The environment containing the adjacency bonuses
     * @param node The node to update
     */
    void recomputeAdjacencyBonuses(Environment& environment, SuperpositionedNode& node);

private:
    bl::ctr::Vector2D<SuperpositionedNode> nodes;
    bl::ctr::PriorityQueue<SuperpositionedNode*, SuperpositionedNode::Priority> collapseQueue;
};

} // namespace gen
} // namespace arena
} // namespace core

#endif
