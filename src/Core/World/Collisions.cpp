#include <Core/World/Collisions.hpp>

namespace core
{
namespace world
{
b2Filter Collisions::getShortCoverFilter() {
    b2Filter filter     = b2DefaultFilter();
    filter.categoryBits = Category::ShortCover;
    filter.maskBits     = Category::All;
    return filter;
}

b2Filter Collisions::getTallCoverFilter() {
    b2Filter filter     = b2DefaultFilter();
    filter.categoryBits = Category::TallCover;
    filter.maskBits     = Category::All;
    return filter;
}

b2Filter Collisions::getUnitFilter() {
    b2Filter filter     = b2DefaultFilter();
    filter.categoryBits = Category::Unit;
    filter.maskBits     = Category::All;
    return filter;
}

b2Filter Collisions::getBuildingFilter() {
    b2Filter filter     = b2DefaultFilter();
    filter.categoryBits = Category::Building;
    filter.maskBits     = Category::All;
    return filter;
}

b2Filter Collisions::getBulletFilter() {
    b2Filter filter     = b2DefaultFilter();
    filter.categoryBits = Category::Bullet;
    filter.maskBits     = Category::All;
    return filter;
}

b2Filter Collisions::getSensorFilter() {
    b2Filter filter     = b2DefaultFilter();
    filter.categoryBits = Category::WorldSensor;
    filter.maskBits     = Category::Unit | Category::SensorQueryRay;
    return filter;
}

b2QueryFilter Collisions::getCoverQueryFilter() {
    b2QueryFilter filter = b2DefaultQueryFilter();
    filter.categoryBits  = Category::CoverQueryRay;
    filter.maskBits      = Category::ShortCover;
    return filter;
}

b2QueryFilter Collisions::getNodeConnectionFilter() {
    b2QueryFilter filter = b2DefaultQueryFilter();
    filter.categoryBits  = Category::SensorQueryRay;
    filter.maskBits      = Category::ShortCover | Category::TallCover | Category::WorldSensor;
    return filter;
}

b2QueryFilter Collisions::getUnitQueryFilter() {
    b2QueryFilter filter = b2DefaultQueryFilter();
    filter.categoryBits  = Category::Unit;
    filter.maskBits      = Category::Unit;
    return filter;
}

b2QueryFilter Collisions::getBuildingQueryFilter() {
    b2QueryFilter filter = b2DefaultQueryFilter();
    filter.categoryBits  = Category::Building;
    filter.maskBits      = Category::Building;
    return filter;
}

b2QueryFilter Collisions::getTargetQueryFilter() {
    b2QueryFilter filter = b2DefaultQueryFilter();
    filter.categoryBits  = Category::Unit | Category::Building;
    filter.maskBits      = Category::Unit | Category::Building;
    return filter;
}

b2QueryFilter Collisions::getLineOfSightQueryFilter() {
    b2QueryFilter filter = b2DefaultQueryFilter();
    filter.categoryBits  = Category::Unit | Category::Building;
    filter.maskBits =
        Category::TallCover | Category::ShortCover | Category::Unit | Category::Building;
    return filter;
}

b2QueryFilter Collisions::getUnitMovementQueryFilter() {
    b2QueryFilter filter = b2DefaultQueryFilter();
    filter.categoryBits  = Category::Unit;
    filter.maskBits      = Category::Unit | Category::ShortCover | Category::TallCover;
    return filter;
}

} // namespace world
} // namespace core
