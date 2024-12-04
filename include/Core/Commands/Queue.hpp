#ifndef CORE_COMMANDS_QUEUE_HPP
#define CORE_COMMANDS_QUEUE_HPP

#include <BLIB/Containers/StaticRingBuffer.hpp>
#include <Core/Commands/AddMode.hpp>
#include <Core/Commands/Command.hpp>
#include <Core/Commands/ExecutorHandle.hpp>
#include <type_traits>

namespace core
{
namespace cmd
{
/**
 * @brief Command queue to be used by classes that can have command queues
 *
 * @tparam T The type of command to queue
 * @tparam Size The max queue size
 * @ingroup Commands
 */
template<typename T, std::size_t Size>
class Queue {
    static_assert(std::is_base_of_v<Command, T>, "T must derive from Command");

public:
    /**
     * @brief Returns whether the queue has a current command or not
     */
    bool hasCommand() const { return currentCommand; }

    /**
     * @brief Returns the current command
     */
    const T& getCurrentCommand() { return *currentCommand; }

    /**
     * @brief Marks the current command as complete and pops the next from the queue
     */
    void completeCurrentCommand() {
        currentCommand.markComplete();
        popQueue();
    }

    /**
     * @brief Marks the current command as failed and pops the next from the queue
     */
    void failCurrentCommand() {
        currentCommand.markFailed();
        popQueue();
    }

    /**
     * @brief Marks the current command as canceled and pops the next from the queue
     */
    void cancelCurrentCommand() {
        currentCommand.markCanceled();
        popQueue();
    }

    /**
     * @brief Adds a command to the queue
     *
     * @param command The command to queue
     * @param addType How to do the add
     */
    void queueCommand(ExternalHandle<T>&& command, AddMode addType = AddMode::QueueEnd) {
        const auto makeCurrent = [this, &command]() {
            currentCommand = std::forward<ExternalHandle<T>>(command);
            currentCommand.markQueued();
            currentCommand.markInProgress();
        };

        if (!currentCommand) { makeCurrent(); }
        else {
            switch (addType) {
            case AddMode::QueueEnd:
                queue.emplace_back(ExecutorHandle<T>(std::forward<ExternalHandle<T>>(command)));
                queue.back().markQueued();
                break;

            case AddMode::QueueStart:
                queue.emplace_front(ExecutorHandle<T>(std::forward<ExternalHandle<T>>(command)));
                queue.front().markQueued();
                break;

            case AddMode::Immediate:
                currentCommand.markQueued();
                queue.emplace_front(std::move(currentCommand));
                makeCurrent();
                break;

            case AddMode::ReplaceAll:
                currentCommand.markCanceled();
                for (unsigned int i = 0; i < queue.size(); ++i) { queue[i].markCanceled(); }
                queue.clear();
                makeCurrent();
                break;

            default:
                break;
            }
        }
    }

private:
    ExecutorHandle<T> currentCommand;
    bl::ctr::StaticRingBuffer<ExecutorHandle<T>, Size> queue;

    void popQueue() {
        currentCommand->release();
        if (!queue.empty()) {
            currentCommand = std::move(queue.front());
            queue.pop_front();
            currentCommand.markInProgress();
        }
    }
};

} // namespace cmd
} // namespace core

#endif
