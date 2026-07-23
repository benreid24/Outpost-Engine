#include <Core/Arena/Arena.hpp>

namespace core
{
namespace arena
{
Arena::Arena() {}

void Arena::generate(std::uint64_t seed, const glm::vec2& size, float maxHeight) {
    generator.reseed(seed);
    terrain.generate(generator, size, maxHeight);
}

void Arena::addToWorld(bl::engine::World& world) { terrain.addToWorld(world); }

} // namespace arena
} // namespace core
