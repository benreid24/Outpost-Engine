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
using Rule  = gen::RuledBiome::Rule;
using Bonus = gen::AdjacencyBonus;

constexpr std::uint64_t NominalWeight = 1000;
constexpr std::uint64_t HalfWeight    = NominalWeight / 2;
constexpr std::uint64_t QuarterWeight = NominalWeight / 4;
constexpr std::uint64_t DoubleWeight  = NominalWeight * 2;

const gen::RuledBiome LakeBiome = gen::RuledBiome{
    .biome      = gen::Biome::Water,
    .heightRule = Rule{.allowedRange = {0.f, 0.45f}, .idealValue = 0.f, .maxWeight = HalfWeight},
    .moistureRule =
        Rule{.allowedRange = {0.2f, 1.f}, .idealValue = 1.f, .maxWeight = NominalWeight},
    .adjacencyBonuses =
        std::vector<Bonus>{Bonus{.biome = gen::Biome::Water, .bonus = NominalWeight}}};

const gen::RuledBiome RiverBiome = gen::RuledBiome{
    .biome = gen::Biome::River,
    .heightRule =
        Rule{.allowedRange = {0.45f, 0.85f}, .idealValue = 0.65f, .maxWeight = QuarterWeight},
    .moistureRule =
        Rule{.allowedRange = {0.8f, 1.f}, .idealValue = 1.f, .maxWeight = NominalWeight},
    .adjacencyBonuses = std::vector<Bonus>{Bonus{.biome         = gen::Biome::River,
                                                 .stackBehavior = Bonus::Behavior::Multiplicative,
                                                 .bonus         = DoubleWeight}}};

const gen::RuledBiome DesertBiome = gen::RuledBiome{
    .biome = gen::Biome::Desert,
    .heightRule =
        Rule{.allowedRange = {0.f, 0.75f}, .idealValue = 0.35f, .maxWeight = QuarterWeight},
    .moistureRule =
        Rule{.allowedRange = {0.f, 0.2f}, .idealValue = 0.f, .maxWeight = DoubleWeight}};

const gen::RuledBiome GrasslandBiome = gen::RuledBiome{
    .biome = gen::Biome::Grassland,
    .heightRule =
        Rule{.allowedRange = {0.f, 0.75f}, .idealValue = 0.5f, .maxWeight = NominalWeight},
    .moistureRule = Rule{.allowedRange = {0.4f, 1.f}, .idealValue = 0.6f, .maxWeight = HalfWeight}};

const gen::RuledBiome MountainBiome = gen::RuledBiome{
    .biome = gen::Biome::Mountain,
    .heightRule =
        Rule{.allowedRange = {0.75f, 1.f}, .idealValue = 0.85f, .maxWeight = DoubleWeight},
    .moistureRule =
        Rule{.allowedRange = {0.f, 1.f}, .idealValue = 0.5f, .maxWeight = NominalWeight},
    .adjacencyBonuses = std::vector<Bonus>{Bonus{.biome         = gen::Biome::Mountain,
                                                 .stackBehavior = Bonus::Behavior::Multiplicative,
                                                 .bonus         = DoubleWeight}}};

const gen::RuledBiome SnowBiome = gen::RuledBiome{
    .biome = gen::Biome::Snow,
    .heightRule =
        Rule{.allowedRange = {0.65f, 1.f}, .idealValue = 0.85f, .maxWeight = DoubleWeight},
    .moistureRule = Rule{.allowedRange = {0.5f, 1.f}, .idealValue = 1.f, .maxWeight = DoubleWeight},
    .adjacencyBonuses = std::vector<Bonus>{Bonus{.biome         = gen::Biome::Snow,
                                                 .stackBehavior = Bonus::Behavior::Multiplicative,
                                                 .bonus         = DoubleWeight}}};

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
