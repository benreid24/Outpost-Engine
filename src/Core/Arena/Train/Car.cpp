#include <Core/Arena/Train/Car.hpp>

#include <Core/Arena/Train/Track.hpp>

namespace core
{
namespace arena
{
namespace train
{
namespace
{
constexpr float WheelEmbedFactor = 0.8f;
}

Car::Car(const Config& config)
: config(config)
, state{} {}

void Car::advance(float distance, const Track& track) {
    update(state.frontAxlePosition + distance, track);
}

void Car::update(float position, const Track& track) {
    state.frontAxlePosition = position;
    state.rearAxlePosition  = track.findDistanceFromOffset(
        state.frontAxlePosition, -(config.frontAxleOffset + config.rearAxleOffset));

    updateSpatialState(track);
}

void Car::update(const Car& connectedTo, const Track& track) {
    // distance from rear axle to rear of car for the one we are connected to
    const float connectedCouplerOffset =
        connectedTo.config.length * 0.5f - connectedTo.config.rearAxleOffset;

    // distance from front axle to front of car for this one
    const float ourCouplerOffset = config.length * 0.5f - config.frontAxleOffset;

    // track position of rear coupler of the car we are connected to
    const float connectedCouplerPos =
        track.findDistanceFromOffset(connectedTo.state.rearAxlePosition, connectedCouplerOffset);

    // track position of front coupler of this car based on the other coupler
    const float ourCouplerPos = track.findDistanceFromOffset(
        connectedCouplerPos, config.couplerStandoff + connectedTo.config.couplerStandoff);

    state.frontAxlePosition = track.findDistanceFromOffset(ourCouplerPos, -ourCouplerOffset);
    state.rearAxlePosition  = track.findDistanceFromOffset(
        state.frontAxlePosition, -(config.frontAxleOffset + config.rearAxleOffset));

    updateSpatialState(track);
}

void Car::updateSpatialState(const Track& track) {
    const glm::vec3 frontAxlePos = track.getPositionAtDistance(state.frontAxlePosition);
    const glm::vec3 frontAxleDir = track.getRightAtDistance(state.frontAxlePosition);
    const glm::vec3 frontAxleUp  = track.getUpAtDistance(state.frontAxlePosition);
    state.frontWheelWorldTransform.lookAt(frontAxleDir);
    state.frontWheelWorldTransform.setPosition(frontAxlePos +
                                               frontAxleUp * config.wheelHeight * 0.5f);

    const glm::vec3 rearAxlePos = track.getPositionAtDistance(state.rearAxlePosition);
    const glm::vec3 rearAxleDir = track.getRightAtDistance(state.rearAxlePosition);
    const glm::vec3 rearAxleUp  = track.getUpAtDistance(state.rearAxlePosition);
    state.rearWheelWorldTransform.lookAt(rearAxleDir);
    state.rearWheelWorldTransform.setPosition(rearAxlePos + rearAxleUp * config.wheelHeight * 0.5f);

    const glm::vec3 rearWheelAttachPos =
        rearAxlePos + rearAxleUp * config.wheelHeight * WheelEmbedFactor;
    const glm::vec3 frontWheelAttachPos =
        frontAxlePos + frontAxleUp * config.wheelHeight * WheelEmbedFactor;

    const glm::vec3 rearToFront = glm::normalize(frontWheelAttachPos - rearWheelAttachPos);
    const glm::vec3 carUp       = glm::normalize((frontAxleUp + rearAxleUp) * 0.5f);
    const float carHeight       = config.wheelHeight * WheelEmbedFactor + config.height * 0.5f;
    glm::vec3 carWorldPos       = rearAxlePos + rearToFront * config.rearAxleOffset;
    carWorldPos += carUp * carHeight;
    state.carWorldTransform.setPosition(carWorldPos);
    state.carWorldTransform.lookAt(frontAxlePos, rearAxlePos);

    if (carBody.exists()) {
        carBody.getTransform()    = state.carWorldTransform;
        frontWheel.getTransform() = state.frontWheelWorldTransform;
        rearWheel.getTransform()  = state.rearWheelWorldTransform;
    }
}

void Car::addToWorld(bl::engine::World& world) {
    carBody.create(world, config.length, {config.width, config.height});
    frontWheel.create(world, config.width, config.wheelHeight * 0.5f, 300);
    rearWheel.create(world, config.width, config.wheelHeight * 0.5f, 300);

    carBody.addToScene(world.scene(), bl::rc::UpdateSpeed::Dynamic);
    frontWheel.addToScene(world.scene(), bl::rc::UpdateSpeed::Dynamic);
    rearWheel.addToScene(world.scene(), bl::rc::UpdateSpeed::Dynamic);
}

} // namespace train
} // namespace arena
} // namespace core
