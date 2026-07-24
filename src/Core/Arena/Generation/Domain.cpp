#include <Core/Arena/Generation/Domain.hpp>

#include <Core/Arena/Generator.hpp>

namespace core
{
namespace arena
{
namespace gen
{
Domain::Domain()
: biomes() {}

Biome Domain::select(Generator& gen) const {
    float totalWeight = 0.f;
    for (const auto& wb : biomes) { totalWeight += wb.weight; }
    const float selection = gen.getFloat(0.f, totalWeight);
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
