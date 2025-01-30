#ifndef CORE_COMPONENTS_UNITAI_HPP
#define CORE_COMPONENTS_UNITAI_HPP

#include <Core/Commands/Queue.hpp>
#include <Core/Unit/AI/LowBrain.hpp>
#include <Core/Unit/AI/MidBrain.hpp>
#include <Core/Unit/AI/TopBrain.hpp>

namespace core
{
namespace com
{
/**
 * @brief Component that adds AI for units
 *
 * @ingroup Unit
 * @ingroup Components
 */
class UnitAI {
public:
    /// Max number of commands that may be queued
    static constexpr std::size_t CommandQueueSize = 4;

    /**
     * @brief Creates the unit AI
     *
     * @param owner The unit to control
     */
    UnitAI(Unit& owner);

    /**
     * @brief Performs the unit AI update
     *
     * @param ctx The update context
     */
    void update(const unit::ai::UpdateContext& ctx);

    /**
     * @brief Notify the AI of an event affecting its unit
     *
     * @param notification The notification to process
     */
    void notify(const unit::ai::Notification& notification);

    /**
     * @brief Queues a command to be carried out by the unit
     *
     * @param command The command to perform
     * @param addMode How to add the command to the queue
     */
    void queueCommand(const cmd::UnitCommandHandle& command,
                      cmd::AddMode addMode = cmd::AddMode::QueueEnd);

private:
    com::Unit& owner;
    unit::ai::TopBrain topBrain;
    unit::ai::MidBrain midBrain;
    unit::ai::LowBrain lowBrain;
    cmd::Queue<cmd::UnitCommand, CommandQueueSize> commandQueue;

    void startNextCommand();
};

} // namespace com
} // namespace core

#endif
