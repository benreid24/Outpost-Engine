#include <Core/World/Collisions.hpp>

namespace core
{
namespace world
{
b2Filter Collisions::getCoverFilter() {
    b2Filter filter     = b2DefaultFilter();
    filter.categoryBits = Category::Cover;
    filter.maskBits     = Category::All;
    return filter;
}

b2Filter Collisions::getUnitFilter() {
    b2Filter filter     = b2DefaultFilter();
    filter.categoryBits = Category::Unit;
    filter.maskBits     = Category::All;
    return filter;
}

b2Filter Collisions::getBulletFilter() {
    b2Filter filter     = b2DefaultFilter();
    filter.categoryBits = Category::Bullet;
    filter.maskBits     = Category::All;
    return filter;
}

b2QueryFilter Collisions::getCoverQueryFilter() {
    b2QueryFilter filter = b2DefaultQueryFilter();
    filter.categoryBits  = Category::CoverQueryRay;
    filter.maskBits      = Category::Cover;
    return filter;
}

} // namespace world
} // namespace core
