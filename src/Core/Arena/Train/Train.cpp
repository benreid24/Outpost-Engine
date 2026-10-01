#include <Core/Arena/Train/Train.hpp>

namespace core
{
namespace arena
{
namespace train
{
namespace
{
template<typename TIt>
TIt advance(TIt it, unsigned int n) {
    std::advance(it, n);
    return it;
}
} // namespace

Train::Train(const Track& track)
: track(track)
, world(nullptr) {
    cars.emplace_back(Car::DefaultLocomotiveConfig);
    cars.front().update(0.f, track);
    updateCars();
}

const Car& Train::getCar(unsigned int index) const { return *advance(cars.begin(), index); }

void Train::addCar(const Car::Config& car, int position) {
    Car* newCar = nullptr;
    if (position < 0 || position >= static_cast<int>(cars.size())) {
        newCar = &cars.emplace_back(car);
    }
    else {
        const auto it = advance(cars.begin(), position);
        newCar        = &*cars.emplace(it, car);
    }

    if (world) { newCar->addToWorld(*world); }

    updateCars();
}

void Train::update(float dt) {
    cars.front().advance(velocity * dt, track);
    updateCars();
}

void Train::setVelocity(float v) { velocity = v; }

void Train::updateCars() {
    Car* prev = &cars.front();
    for (auto it = std::next(cars.begin()); it != cars.end(); ++it) {
        it->update(*prev, track);
        prev = &*it;
    }
}

void Train::addToWorld(bl::engine::World& world) {
    this->world = &world;
    for (auto& car : cars) { car.addToWorld(world); }
}

float Train::getLength() const {
    float length = 0.f;
    for (const auto& car : cars) {
        length += car.getConfig().length + car.getConfig().couplerStandoff;
    }
    return length;
}

void Train::setPosition(float position) {
    cars.front().update(position, track);
    updateCars();
}

} // namespace train
} // namespace arena
} // namespace core
