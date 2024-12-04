#include <Core/Systems/Unit.hpp>

#include <BLIB/Components/Hitbox2D.hpp>
#include <BLIB/Components/MarkedForDeath.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Math.hpp>
#include <Core/Components/Damager.hpp>
#include <Core/Game.hpp>
#include <Core/Properties.hpp>
#include <Core/World/Collisions.hpp>
#include <cmath>

namespace core
{
namespace sys
{
namespace
{
using BulletEcsTransaction = bl::ecs::Transaction<
    bl::ecs::tx::EntityWrite, bl::ecs::tx::ComponentRead<>,
    bl::ecs::tx::ComponentWrite<bl::com::Transform2D, bl::com::Physics2D, com::Damager,
                                bl::com::MarkedForDeath, bl::com::Hitbox2D>>;

glm::vec2 changeVectorDirection(glm::vec2 forward, unit::able::Move::MoveDirection dir) {
    switch (dir) {
    case unit::able::Move::MoveDirection::Forward:
        return forward;
    case unit::able::Move::MoveDirection::Right:
        return {-forward.y, forward.x};
    case unit::able::Move::MoveDirection::Left:
        return {forward.y, -forward.x};
    case unit::able::Move::MoveDirection::Backward:
    default:
        return -forward;
    }
}

unit::able::Rotate::RotateDirection getRotateDirection(float angle, float target) {
    auto dir       = unit::able::Rotate::Clockwise;
    float opposite = angle + 180.f;
    if (opposite >= 360.f) {
        opposite -= 360.f;
        if (target < angle && target > opposite) { dir = unit::able::Rotate::CounterClockwise; }
    }
    else {
        if (target < angle || target > opposite) { dir = unit::able::Rotate::CounterClockwise; }
    }
    return dir;
}

} // namespace

void Unit::init(bl::engine::Engine& e) {
    engine = &e;
    units  = &e.ecs().getAllComponents<com::Unit>();
}

void Unit::update(std::mutex&, float dt, float, float, float) {
    // TODO - build tiered AI state machine
    // processAI(dt);
    doCapabilities(dt);
}

// void Unit::processAI(float dt) {
//     units->forEach([this, dt](bl::ecs::Entity entity, com::Unit& unit) {
//         processHighLevelAI(entity, unit, dt);
//     });
// }

void Unit::doCapabilities(float dt) {
    units->forEach([this, dt](bl::ecs::Entity entity, com::Unit& unit) {
        auto* rotater = unit.capabilities().get<unit::Capability::Rotate>();
        if (rotater) {
            using Dir        = unit::able::Rotate::RotateDirection;
            const float sign = rotater->rotateDir == Dir::Clockwise ? 1.f : -1.f;
            unit.physics.getTransform().rotate(sign * rotater->rotateRate * rotater->rotateFactor *
                                               dt);
            rotater->rotateDir = Dir::NoRotate;
        }

        auto* mover = unit.capabilities().get<unit::Capability::Move>();
        if (mover) {
            using Dir = unit::able::Move::MoveDirection;

            if (mover->moveDir != Dir::NoMove) {
                unit.physics.setLinearDamping(0.f);
                const float mass = unit.physics.getMass();
                const float radians =
                    bl::math::degreesToRadians(unit.physics.getTransform().getRotation());
                const float c = std::cos(radians);
                const float s = std::sin(radians);

                const glm::vec2 vel = unit.physics.getLinearVelocity();
                const float velMag  = glm::length(vel);
                if (velMag > 0.f) {
                    const glm::vec2 diff =
                        (changeVectorDirection({c, s}, mover->moveDir) - glm::normalize(vel)) *
                        velMag;
                    unit.physics.applyImpulseToCenter(diff * mass *
                                                      mover->directionCorrectionFactor);
                }

                const glm::vec2 f =
                    glm::vec2(c, s) * (mover->acceleration * mass) * mover->moveFactor;
                unit.physics.applyForceToCenter(changeVectorDirection(f, mover->moveDir));
                mover->moveDir = Dir::NoMove;
            }
            else { unit.physics.setLinearDamping(mover->damping); }

            if (mover->maxSpeed > 0.f) { unit.physics.clampLinearVelocity(mover->maxSpeed); }
        }

        auto* shooter = unit.capabilities().get<unit::Capability::Shoot>();
        if (shooter) {
            constexpr float Velocity = 30.f;
            constexpr glm::vec2 Size(8.f, 3.f);

            shooter->timeSinceLastFire += dt;
            if (shooter->isFiring) {
                shooter->isFiring = false;
                if (shooter->timeSinceLastFire >= shooter->fireDelay) {
                    shooter->timeSinceLastFire = 0.f;

                    auto& game = bl::game::Game::getInstance<Game>();
                    auto& ecs  = engine->ecs();

                    // TODO - place bullet firing somewhere else
                    BulletEcsTransaction tx(ecs);
                    bl::com::Transform2D* transform =
                        ecs.getComponent<bl::com::Transform2D>(entity, tx);
                    if (!transform) {
                        BL_LOG_ERROR << "Tried to fire bullet from entity without transform: "
                                     << entity;
                        return;
                    }

                    const bl::ecs::Entity bullet =
                        ecs.createEntity(entity.getWorldIndex(), bl::ecs::Flags::WorldObject, tx);
                    const glm::vec2 pos   = transform->getGlobalPosition();
                    const float r         = bl::math::degreesToRadians(transform->getRotation());
                    const float c         = std::cos(r);
                    const float s         = std::sin(r);
                    auto* bulletTransform = ecs.emplaceComponentWithTx<bl::com::Transform2D>(
                        bullet,
                        tx,
                        pos + glm::vec2(c, s) * shooter->bulletOffset,
                        transform->getRotation());

                    ecs.emplaceComponentWithTx<com::Damager>(bullet, tx, shooter->damage);
                    ecs.emplaceComponentWithTx<bl::com::MarkedForDeath>(
                        bullet, tx, 2.f, bl::engine::StateMask::Running);
                    game.renderSystem().addTestGraphicsToEntity(bullet, Size, sf::Color::Black);

                    auto bodyDef             = b2DefaultBodyDef();
                    auto shapeDef            = b2DefaultShapeDef();
                    bodyDef.type             = b2_dynamicBody;
                    bodyDef.fixedRotation    = true;
                    bodyDef.isBullet         = true;
                    bodyDef.linearVelocity.x = c * Velocity;
                    bodyDef.linearVelocity.y = s * Velocity;
                    bodyDef.linearDamping    = 0.f;
                    shapeDef.filter          = world::Collisions::getBulletFilter();
                    ecs.emplaceComponentWithTx<bl::com::Hitbox2D>(
                        bullet, tx, bulletTransform, Size);
                    game.physicsSystem().addPhysicsToEntity(bullet, bodyDef, shapeDef);
                }
            }
        }
    });
}

// void Unit::processHighLevelAI(bl::ecs::Entity entity, com::Unit& unit, float dt) {
//     // first move queued actions into slots if possible
//     while (!unit.queuedCommands.empty()) {
//         auto& cmd  = unit.queuedCommands.front();
//         auto& slot = unit.activeCommands[cmd->getConcurrencyType()];
//         if (!slot || slot->isTerminal()) {
//             slot = std::move(cmd);
//             slot.markInProgress();
//             unit.commandStates[slot->getConcurrencyType()].init(*slot);
//             unit.queuedCommands.pop_front();
//         }
//         else { break; }
//     }
//
//     // process active commands
//     using Type = cmd::UnitCommand::Type;
//     for (int i = 0; i < cmd::UnitCommand::ConcurrencyType::COUNT; ++i) {
//         auto& cmd = unit.activeCommands[i];
//         if (cmd && !cmd->isTerminal()) {
//             switch (cmd->getType()) {
//             case Type::MoveToPosition:
//                 processMoveToNodeCommand(entity, unit, cmd, dt);
//                 break;
//             case Type::KillUnit:
//                 processKillUnitCommand(unit, cmd, dt);
//                 break;
//             default:
//                 BL_LOG_ERROR << "Invalid command type: " << cmd->getType();
//                 cmd.markFailed();
//                 cmd.release();
//                 break;
//             }
//         }
//     }
// }

// void Unit::processMoveToNodeCommand(bl::ecs::Entity entity, com::Unit& unit,
//                                     com::Unit::CmdHandle& cmd, float dt) {
//     using Waypoint = unit::Path::Waypoint;
//
//     if (!unit.canMove()) {
//         cmd.markFailed();
//         return;
//     }
//
//     auto& game          = bl::game::Game::getInstance<core::Game>();
//     auto world          = game.engine().getWorld<world::World>(entity.getWorldIndex());
//     auto& mover         = unit.getMover();
//     auto& ctx           = unit.commandStates[cmd->getConcurrencyType()].getPathContext();
//     const glm::vec2 pos = unit.physics.getTransform().getGlobalPosition();
//
//     // first make sure we have a path
//     if (ctx.path.waypoints.empty()) {
//         if (!world->computePath(unit.physics.getTransform().getGlobalPosition(),
//                                 cmd->getTargetPosition(),
//                                 ctx.path)) {
//             cmd.markFailed();
//             cmd.release();
//             return;
//         }
//         ctx.currentNode = 0;
//     }
//
//     const Waypoint node     = ctx.path.waypoints[ctx.currentNode];
//     const float distance    = glm::distance(pos, node.position);
//     const float targetAngle = bl::math::computeAngle(pos, node.position);
//
//     // point towards next node
//     const float angle     = unit.physics.getTransform().getRotation();
//     const float angleDiff = std::abs(angle - targetAngle);
//     if (angleDiff > 1.f) {
//         rotateUnit(unit, targetAngle, dt);
//         if (distance / mover.maxSpeed < angleDiff / mover.rotateRate) { return; }
//     }
//
//     // move towards target
//     if (distance < Properties.UnitAiDistanceStopThresh.get()) {
//         if (ctx.currentNode == ctx.path.waypoints.size() - 1) {
//             cmd.markComplete();
//             unit.commandStates[cmd->getConcurrencyType()].clear();
//             cmd.release();
//         }
//         else {
//             ++ctx.currentNode;
//             processMoveToNodeCommand(entity, unit, cmd, dt);
//         }
//     }
//     else {
//         const bool occupied = node.worldNode &&
//                               node.worldNode->getOccupier() != bl::ecs::InvalidEntity &&
//                               node.worldNode->getOccupier() != entity;
//         if (occupied || !world->pathToPositionIsClear(pos, node.position)) {
//             ctx.path.waypoints.clear();
//             processMoveToNodeCommand(entity, unit, cmd, dt);
//         }
//         else { mover.move(unit::Moveable::Forward); }
//     }
// }

// void Unit::processKillUnitCommand(com::Unit& unit, com::Unit::CmdHandle& cmd, float dt) {
//     if (!unit.canShoot()) {
//         cmd.markFailed();
//         cmd.release();
//         return;
//     }
//
//     auto& game    = bl::game::Game::getInstance<core::Game>();
//     auto& mover   = unit.getMover();
//     auto& shooter = unit.getShooter();
//
//     // stop if target dead
//     if (!game.engine().ecs().entityExists(cmd->getTargetUnit()->getId())) {
//         cmd.markComplete();
//         cmd.release();
//         return;
//     }
//     else {
//         com::Mortal* m =
//             game.engine().ecs().getComponent<com::Mortal>(cmd->getTargetUnit()->getId());
//         if (m && m->health <= 0.f) {
//             cmd.markComplete();
//             cmd.release();
//             return;
//         }
//     }
//
//     // point towards target and fire if close enough
//     const float angle = unit.physics.getTransform().getRotation();
//     const float targetAngle =
//         bl::math::computeAngle(unit.physics.getTransform().getGlobalPosition(),
//                                cmd->getTargetUnit()->physics.getTransform().getGlobalPosition());
//     if (std::abs(angle - targetAngle) > 1.f) { rotateUnit(unit, targetAngle, dt); }
//     else { shooter.fire(); }
// }

// void Unit::rotateUnit(com::Unit& unit, float target, float dt) {
//     auto dir          = unit::Moveable::Clockwise;
//     const float angle = unit.physics.getTransform().getRotation();
//     float opposite    = angle + 180.f;
//     if (opposite >= 360.f) {
//         opposite -= 360.f;
//         if (target < angle && target > opposite) { dir = unit::Moveable::CounterClockwise; }
//     }
//     else {
//         if (target < angle || target > opposite) { dir = unit::Moveable::CounterClockwise; }
//     }
//
//     const float maxRot = unit.getMover().rotateRate * dt;
//     const float diff   = std::abs(angle - target);
//     const float factor = diff >= maxRot ? 1.f : diff / maxRot;
//     unit.getMover().rotate(dir, factor);
// }

} // namespace sys
} // namespace core
