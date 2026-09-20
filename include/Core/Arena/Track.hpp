#ifndef CORE_ARENA_TRACK_HPP
#define CORE_ARENA_TRACK_HPP

#include <BLIB/Graphics/IndexBuffer3D.hpp>
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

    /**
     * @brief Creates engine objects for the track and adds them to the world
     *
     * @param world The world to create the objects in
     * @param step The step size for the track
     */
    void addToWorld(bl::engine::World& world, float step);

    /**
     * @brief Generates the geometry for the track
     */
    void generateGeometry();

private:
    std::vector<TrackNode> nodes;
    bl::gfx::IndexBuffer3D railsDrawable;
    bl::gfx::IndexBuffer3D tiesDrawable;
    float step;

    const TrackNode& getNodeAtDistance(float d) const;
    void generateRail(float offset, std::uint32_t& vertexOffset, std::uint32_t& indexOffset);
    void generateTies();
};

} // namespace arena
} // namespace core

#endif
