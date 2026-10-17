#include <Core/Arena/Generation/Domain.hpp>

namespace core
{
namespace arena
{
namespace gen
{
Domain::Domain()
: biomes() {}

std::uint64_t Domain::totalWeight() const {
    std::uint64_t total = 0;
    for (const auto& wb : biomes) { total += wb.weight; }
    return total;
}

Biome Domain::select(bl::rand::Seed& gen) const {
    const float selection = gen.getFloat<float>(0.f, totalWeight());
    float currentWeight   = 0.f;
    for (const auto& wb : biomes) {
        currentWeight += wb.weight;
        if (selection <= currentWeight) { return wb.biome; }
    }
    return biomes.back().biome;
}

} // namespace gen
} // namespace arena
} // namespace core
