#ifndef CORE_ARENA_TRAIN_CAR_HPP
#define CORE_ARENA_TRAIN_CAR_HPP

#include <BLIB/Components/Transform3D.hpp>
#include <BLIB/Engine/World.hpp>
#include <BLIB/Graphics/Cylinder.hpp>
#include <BLIB/Graphics/RectangularPrism.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>

namespace core
{
namespace arena
{
/// Collection of classes and methods for the train
namespace train
{
class Track;

/**
 * @brief Representation of a train car and its state
 *
 * @ingroup Arena
 */
class Car {
public:
    /// The type of train car
    enum struct Type { Locomotive, Cargo, Passenger };

    /// The configuration of a train car
    struct Config {
        Type type;

        float length;
        float width;
        float height;
        float wheelHeight;

        float couplerStandoff;
        float frontAxleOffset;
        float rearAxleOffset;
    };

    /// The state of a train car
    struct State {
        float frontAxleTrackPosition;
        float rearAxleTrackPosition;

        bl::com::Transform3D carWorldTransform;
        bl::com::Transform3D frontWheelWorldTransform;
        bl::com::Transform3D rearWheelWorldTransform;
    };

    static constexpr float DefaultLength          = 10.f;
    static constexpr float DefaultWidth           = 3.f;
    static constexpr float DefaultHeight          = 3.f;
    static constexpr float DefaultWheelHeight     = 1.f;
    static constexpr float DefaultCouplerStandoff = 0.5f;
    static constexpr float DefaultFrontAxleOffset = 4.f;
    static constexpr float DefaultRearAxleOffset  = 4.f;

    static constexpr Config DefaultLocomotiveConfig =
        Config{.type            = Type::Locomotive,
               .length          = DefaultLength,
               .width           = DefaultWidth,
               .height          = DefaultHeight,
               .wheelHeight     = DefaultWheelHeight,
               .couplerStandoff = DefaultCouplerStandoff,
               .frontAxleOffset = DefaultFrontAxleOffset,
               .rearAxleOffset  = DefaultRearAxleOffset};

    static constexpr Config DefaultCargoConfig = Config{.type            = Type::Cargo,
                                                        .length          = DefaultLength,
                                                        .width           = DefaultWidth,
                                                        .height          = DefaultHeight,
                                                        .wheelHeight     = DefaultWheelHeight,
                                                        .couplerStandoff = DefaultCouplerStandoff,
                                                        .frontAxleOffset = DefaultFrontAxleOffset,
                                                        .rearAxleOffset  = DefaultRearAxleOffset};

    static constexpr Config DefaultPassengerConfig =
        Config{.type            = Type::Passenger,
               .length          = DefaultLength,
               .width           = DefaultWidth,
               .height          = DefaultHeight,
               .wheelHeight     = DefaultWheelHeight,
               .couplerStandoff = DefaultCouplerStandoff,
               .frontAxleOffset = DefaultFrontAxleOffset,
               .rearAxleOffset  = DefaultRearAxleOffset};

    /**
     * @brief Creates a train car with the given config
     *
     * @param config The train car configuration
     */
    Car(const Config& config);

    /**
     * @brief Advances the car along the track by the given distance
     *
     * @param distance The distance to advance
     * @param track The track the car is on
     */
    void advance(float distance, const Track& track);

    /**
     * @brief Updates the car's position and orientation based on the given position along the track
     *
     * @param position The position along the track
     * @param track The track the car is on
     */
    void update(float position, const Track& track);

    /**
     * @brief Updates the car's position and orientation based on the given position along the track
     *
     * @param connectedTo The car this car is connected to
     * @param track The track the car is on
     */
    void update(const Car& connectedTo, const Track& track);

    /**
     * @brief Returns the train car config
     */
    const Config& getConfig() const { return config; }

    /**
     * @brief Returns the train car state
     */
    const State& getState() const { return state; }

    /**
     * @brief Adds the car to the given world for rendering
     *
     * @param world The world to add the car to
     */
    void addToWorld(bl::engine::World& world);

private:
    Config config;
    State state;

    bl::gfx::RectangularPrism carBody;
    bl::gfx::Cylinder frontWheel;
    bl::gfx::Cylinder rearWheel;

    void updateSpatialState(const Track& track);

    friend struct bl::refl::ReflectedObject<Car>;
};

} // namespace train
} // namespace arena
} // namespace core

namespace bl
{
namespace refl
{
template<>
struct ReflectedObject<core::arena::train::Car::Config> {
    inline static const auto spec = makeSpec<core::arena::train::Car::Config>(
        "CarConfig",
        memberList(
            defineMember(1, "type", &core::arena::train::Car::Config::type),
            defineMember(2, "length", &core::arena::train::Car::Config::length),
            defineMember(3, "width", &core::arena::train::Car::Config::width),
            defineMember(4, "height", &core::arena::train::Car::Config::height),
            defineMember(5, "couplerStandoff", &core::arena::train::Car::Config::couplerStandoff),
            defineMember(6, "frontAxleOffset", &core::arena::train::Car::Config::frontAxleOffset),
            defineMember(7, "rearAxleOffset", &core::arena::train::Car::Config::rearAxleOffset)));
};

template<>
struct ReflectedObject<core::arena::train::Car::State> {
    inline static const auto spec = makeSpec<core::arena::train::Car::State>(
        "CarState",
        memberList(
            defineMember(1, "frontAxleTrackPosition",
                         &core::arena::train::Car::State::frontAxleTrackPosition),
            defineMember(2, "rearAxleTrackPosition",
                         &core::arena::train::Car::State::rearAxleTrackPosition),
            defineMember(3, "carTranform", &core::arena::train::Car::State::carWorldTransform),
            defineMember(4, "frontAxleTransform",
                         &core::arena::train::Car::State::frontWheelWorldTransform),
            defineMember(5, "rearAxleTransform",
                         &core::arena::train::Car::State::rearWheelWorldTransform)));
};

template<>
struct ReflectedObject<core::arena::train::Car> {
    inline static const auto spec = makeSpec<core::arena::train::Car>(
        "Car", memberList(defineMember(1, "config", &core::arena::train::Car::config),
                          defineMember(2, "state", &core::arena::train::Car::state)));
};

} // namespace refl
} // namespace bl

#endif
