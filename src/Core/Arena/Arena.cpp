#include <Core/Arena/Arena.hpp>

#include <Core/Arena/Generation/Generator.hpp>
#include <Core/Arena/Generation/RuledBiome.hpp>
#include <array>

namespace core
{
namespace arena
{
namespace
{
using Rule = gen::RuledBiome::Rule;

const gen::RuledBiome LakeBiome =
    gen::RuledBiome(gen::Biome::Water, Rule(0.f, 0.45f, 0.f, 3.f), Rule(0.2f, 1.f, 1.f, 5.f));

const gen::RuledBiome RiverBiome =
    gen::RuledBiome(gen::Biome::River, Rule(0.45f, 0.85f, 0.65f, 1.f), Rule(0.8f, 1.f, 1.f, 5.f));

const gen::RuledBiome DesertBiome =
    gen::RuledBiome(gen::Biome::Desert, Rule(0.f, 0.75f, 0.35f, 1.f), Rule(0.f, .2f, 0.f, 5.f));

const gen::RuledBiome GrasslandBiome =
    gen::RuledBiome(gen::Biome::Grassland, Rule(0.f, 0.85f, 0.6f, 3.f), Rule(0.4f, 1.f, 0.6f, 3.f));

const gen::RuledBiome MountainBiome =
    gen::RuledBiome(gen::Biome::Mountain, Rule(0.8f, 1.f, 1.f, 5.f), Rule(0.f, 1.f, 0.2f, 1.f));

const gen::RuledBiome SnowBiome =
    gen::RuledBiome(gen::Biome::Snow, Rule(0.65f, 1.f, 0.85f, 5.f), Rule(0.5f, 1.f, 1.f, 5.f));

// TODO - may need inter-biome constraints for beach

const std::vector<gen::RuledBiome> Biomes = {
    LakeBiome, RiverBiome, DesertBiome, GrasslandBiome, MountainBiome, SnowBiome};

constexpr float Step                  = 1.f;
constexpr unsigned int TerrainOctaves = 16;
constexpr float TerrainPersistence    = 0.4f;
constexpr float TerrainFrequency      = 0.005f;

constexpr unsigned int MoistureOctaves = 3;
constexpr float MoisturePersistence    = 0.2f;
constexpr float MoistureFrequency      = 0.001f;

} // namespace

Arena::Arena() {}

void Arena::generate(std::uint64_t seed, const glm::vec2& size, float maxHeight) {
    gen::Parameters genParams(size, Step, maxHeight);
    genParams.terrainPerlin.frequency    = TerrainFrequency;
    genParams.terrainPerlin.octaves      = TerrainOctaves;
    genParams.terrainPerlin.persistence  = TerrainPersistence;
    genParams.moisturePerlin.frequency   = MoistureFrequency;
    genParams.moisturePerlin.octaves     = MoistureOctaves;
    genParams.moisturePerlin.persistence = MoisturePersistence;
    genParams.biomes                     = Biomes;

    gen::Generator generator(seed, genParams);
    generator.generate(*this);
}

void Arena::addToWorld(bl::engine::World& world) { terrain.addToWorld(world); }

} // namespace arena
} // namespace core
