#include <Core/Player/HUD/TempUnitController.hpp>

#include <Core/Game.hpp>
#include <Core/Player/Player.hpp>
#include <Core/World/World.hpp>

namespace core
{
namespace player
{
namespace hud
{
TempUnitController::TempUnitController(Player& owner)
: owner(owner)
, controlling(nullptr)
, self(nullptr) {}

void TempUnitController::init() {
    auto& game  = bl::game::Game::getInstance<Game>();
    auto& world = owner.getCurrentWorld<world::World>();
    auto scene  = owner.getRenderObserver().getOrCreateSceneOverlay();

    background.create(world, {100.f, 60.f});
    background.setFillColor(sf::Color(160, 160, 160));
    background.setOutlineColor(sf::Color::Black);
    background.setOutlineThickness(1.f);
    background.getTransform().setOrigin(100.f, 0.f);
    background.getOverlayScaler().positionInParentSpace({1.f, 0.f});
    background.getOverlayScaler().scaleToWidthPercent(0.11f);
    background.addToScene(scene, bl::rc::UpdateSpeed::Static);

    hint.create(world, game.defaultFont(), "", 36);
    hint.getOverlayScaler().positionInParentSpace({0.5f, 0.5f});
    hint.getOverlayScaler().scaleToWidthPercent(0.85f);
    hint.setParent(background);
    hint.addToScene(scene, bl::rc::UpdateSpeed::Static);

    makeEmptyState();
}

void TempUnitController::reset() {
    hint.destroy();
    background.destroy();
    controlling = nullptr;
    self        = nullptr;
}

bool TempUnitController::processEvent(const Event& event) {
    auto& game = bl::game::Game::getInstance<Game>();

    if (event.source().type == sf::Event::MouseMoved) {
        if (controlling) {
            if (event.target()) {
                auto* unit = game.engine().ecs().getComponent<com::UnitAI>(event.target()->getId());
                if (unit != controlling) { makeAttackState(); }
                else { makeMoveState(); }
            }
            else { makeMoveState(); }
        }
        return true;
    }
    else if (event.source().type == sf::Event::MouseButtonPressed) {
        if (event.source().mouseButton.button == sf::Mouse::Left) {
            if (controlling) {
                if (event.target() && self != event.target()) {
                    controlling->queueCommand(game.commandStore().unitMakeAttack(
                        event.target(), cmd::AggroLevel::Aggressive));
                }
                else {
                    controlling->queueCommand(game.commandStore().unitMakeMove(
                        event.worldPosition(), cmd::AggroLevel::Neutral));
                }
            }
            else {
                self = event.target();
                controlling =
                    event.target() ?
                        game.engine().ecs().getComponent<com::UnitAI>(event.target()->getId()) :
                        nullptr;
                if (controlling) { makeMoveState(); }
                return true;
            }
        }
        else if (event.source().mouseButton.button == sf::Mouse::Right && controlling) {
            controlling = nullptr;
            self        = nullptr;
            makeEmptyState();
            return true;
        }
    }
    return false;
}

void TempUnitController::observe(const bl::ecs::event::ComponentRemoved<com::UnitAI>& event) {
    if (&event.component == controlling) {
        controlling = nullptr;
        makeEmptyState();
    }
}

void TempUnitController::makeEmptyState() {
    hint.getSection().setString("Click to command");
    hint.getSection().setFillColor(sf::Color::Black);
    centerText();
}

void TempUnitController::makeAttackState() {
    hint.getSection().setString("Attack unit");
    hint.getSection().setFillColor(sf::Color::Red);
    centerText();
}

void TempUnitController::makeMoveState() {
    hint.getSection().setString("Move here");
    hint.getSection().setFillColor(sf::Color::Green);
    centerText();
}

void TempUnitController::centerText() { hint.getTransform().setOrigin(hint.getLocalSize() * 0.5f); }

} // namespace hud
} // namespace player
} // namespace core
