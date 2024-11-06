#ifndef CORE_PLAYER_HUD_DEBUGMENU_HPP
#define CORE_PLAYER_HUD_DEBUGMENU_HPP

#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Components/Unit.hpp>

namespace core
{
namespace player
{
/// Collection of menu classes for the player HUD
namespace hud
{
/**
 * @brief Debug menu class for the player HUD
 *
 * @ingroup Player
 */
class DebugMenu : public bl::event::Listener<bl::ecs::event::EntityDestroyed> {
public:
    /**
     * @brief Initializes some default state
     */
    DebugMenu();

    /**
     * @brief Destroys the debug menu
     */
    virtual ~DebugMenu() = default;

    /**
     * @brief Updates the debug menu
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    /**
     * @brief Creates the GUI components in the hidden state
     *
     * @param gui The HUD GUI object
     */
    void init(bl::gui::GUI& gui);

    /**
     * @brief Shows or hides the debug menu window
     */
    void toggle();

    /**
     * @brief Processes the window event
     *
     * @param event The event to process
     * @return True if the event was used, false if no effect
     */
    bool processEvent(const sf::Event& event);

private:
    enum struct TopTab {
        Console,
        World,
        Entity,
    } currentTopTab;

    enum struct WorldTab { Nodes, Cover } currentWorldTab;

    enum struct NodeTool { CreatePath, CreateCover, Remove };
    bl::gui::RadioButton::Group* nodeRadioGroup;
    NodeTool getCurrentNodeTool() const;

    enum struct CoverTool { Create, Remove };
    bl::gui::RadioButton::Group* coverRadioGroup;
    CoverTool getCurrentCoverTool() const;

    enum struct EntityTool { Create, Control, Kill };
    bl::gui::RadioButton::Group* entityRadioGroup;
    EntityTool getCurrentEntityTool() const;

    bl::gui::Window::Ptr window;

    bl::gui::Label::Ptr coverAngleLabel;
    bl::gui::Slider::Ptr coverAngle;
    void onCoverAngleChange();
    float getCoverAngle() const;

    bl::gui::Label::Ptr controlNameLabel;
    void onEntityControl(bl::ecs::Entity control);

    struct Controlling {
        bl::ecs::Entity entity;
        core::com::Unit* unit;

        Controlling()
        : entity(bl::ecs::InvalidEntity)
        , unit(nullptr) {}

        void reset() {
            entity = bl::ecs::InvalidEntity;
            unit   = nullptr;
        }
    } controlling;
    bl::gfx::Rectangle dragBox;
    glm::vec2 clickStart;

    bl::gui::Element::Ptr createConsoleTab();
    bl::gui::Element::Ptr createWorldTab();
    bl::gui::Element::Ptr createEntityTab();

    virtual void observe(const bl::ecs::event::EntityDestroyed& event) override;
};

} // namespace hud
} // namespace player
} // namespace core

#endif
