#ifndef CORE_ARENA_TRACK_HPP
#define CORE_ARENA_TRACK_HPP

#include <Core/Arena/TrackNode.hpp>
#include <vector>

namespace core
{
namespace arena
{
class Terrain;

/**
 * @brief Representation and storage of the train track in the arena
 *
 * @ingroup Arena
 */
class Track {
public:
    /**
     * @brief Creates an empty track
     */
    Track();

    /**
     * @brief Generates the track from the given terrain
     *
     * @param terrain The terrain in the arena
     */
    void generate(const Terrain& terrain);

    /**
     * @brief Returns the total length of the track
     */
    float getTrackLength() const;

    /**
     * @brief Returns the track position at the given distance
     *
     * @param distance The distance along the track
     * @return The position of the track at the given distance in world coordinates
     */
    glm::vec3 getPositionAtDistance(float distance) const;

    /**
     * @brief Returns the direction of the track at the given distance
     *
     * @param distance The distance along the track
     * @return The direction of the track at the given distance
     */
    glm::vec3 getDirectionAtDistance(float distance) const;

    /**
     * @brief Returns the up vector of the track at the given distance
     *
     * @param distance The distance along the track
     * @return The up vector of the track at the given distance
     */
    glm::vec3 getUpAtDistance(float distance) const;

    /**
     * @brief Returns the right vector of the track at the given distance
     *
     * @param distance The distance along the track
     * @return The right vector of the track at the given distance
     */
    glm::vec3 getRightAtDistance(float distance) const;

    /**
     * @brief Returns the nodes of the track
     */
    const std::vector<TrackNode>& getNodes() const { return nodes; }

private:
    std::vector<TrackNode> nodes;

    const TrackNode& getNodeAtDistance(float d) const;
};

} // namespace arena
} // namespace core

#endif
