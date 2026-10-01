#ifndef CORE_ARENA_TRAIN_TRAIN_HPP
#define CORE_ARENA_TRAIN_TRAIN_HPP

#include <Core/Arena/Train/Car.hpp>
#include <list>

namespace core
{
namespace arena
{
namespace train
{
class Train {
public:
    /**
     * @brief Creates a train with a locomotive only
     *
     * @param track The track the train is on
     */
    Train(const Track& track);

    /**
     * @brief Adds a car to the train
     *
     * @param car The car to add
     * @param position The position to add the car at, -1 for the end of the train
     */
    void addCar(const Car::Config& car, int position = -1);

    /**
     * @brief Access the car at the given index
     *
     * @param index The position to access, 0 is the front of the train
     * @return The car at the given index
     */
    const Car& getCar(unsigned int index) const;

    /**
     * @brief Returns the number of cars in the train
     */
    unsigned int getCarCount() const { return cars.size(); }

    /**
     * @brief Sets the track position of the locomotive
     *
     * @param position The position along the track of the front of the train
     */
    void setPosition(float position);

    /**
     * @brief Sets the velocity of the train in world units per second
     *
     * @param v The velocity of the train in world units per second
     */
    void setVelocity(float v);

    /**
     * @brief Returns the trains velocity in world units per second
     */
    float getVelocity() const { return velocity; }

    /**
     * @brief Returns the total approximate length of the train in world units
     */
    float getLength() const;

    /**
     * @brief Updates the train position based on the velocity
     *
     * @param dt Elapsed time in seconds
     */
    void update(float dt);

    /**
     * @brief Adds the train to the given world for rendering
     *
     * @param world The world to add the train to
     */
    void addToWorld(bl::engine::World& world);

private:
    const Track& track;
    std::list<Car> cars;
    float velocity;
    bl::engine::World* world;

    void updateCars();
};

} // namespace train
} // namespace arena
} // namespace core

#endif
