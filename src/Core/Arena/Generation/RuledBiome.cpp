#include <Core/Arena/Generation/RuledBiome.hpp>

namespace core
{
namespace arena
{
namespace gen
{
std::uint64_t RuledBiome::Rule::computeWeight(float value) const {
    if (!allowedRange.contains(value)) { return 0; }

    if (value >= idealValue) {
        const float range = allowedRange.max - idealValue;
        const float delta = value - idealValue;
        return static_cast<std::uint64_t>(static_cast<float>(maxWeight) * (1.f - delta / range));
    }
    else {
        const float range = idealValue - allowedRange.min;
        const float delta = idealValue - value;
        return static_cast<std::uint64_t>(static_cast<float>(maxWeight) * (1.f - delta / range));
    }
}

} // namespace gen
} // namespace arena
} // namespace core
