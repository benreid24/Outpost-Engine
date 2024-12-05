#ifndef CORE_COMMANDS_EXECUTORHANDLE_HPP
#define CORE_COMMANDS_EXECUTORHANDLE_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Logging.hpp>
#include <Core/Commands/ExternalHandle.hpp>
#include <Core/Commands/Ref.hpp>
#include <Core/Commands/SingleStore.hpp>
#include <Core/Events/CommandStatusChange.hpp>

namespace core
{
namespace cmd
{
template<typename T, std::size_t Size>
class Queue;

template<typename T>
class ExecutorHandle {
public:
    /**
     * @brief Creates an empty handle
     */
    ExecutorHandle() = default;

    /**
     * @brief Copies from another handle
     */
    ExecutorHandle(const ExecutorHandle&) = default;

    /**
     * @brief Moves from another handle
     */
    ExecutorHandle(ExecutorHandle&&) = default;

    /**
     * @brief Destroys the handle, potentially releasing the underlying command
     */
    ~ExecutorHandle() = default;

    /**
     * @brief Copies from another handle
     */
    ExecutorHandle& operator=(const ExecutorHandle&) = default;

    /**
     * @brief Moves from another handle
     */
    ExecutorHandle& operator=(ExecutorHandle&&) = default;

    /**
     * @brief Returns the underlying command
     */
    const T& operator*() const { return ref.operator*(); }

    /**
     * @brief Returns the underlying command
     */
    const T* operator->() const { return ref.operator->(); }

    /**
     * @brief Returns whether the ref points to a valid command or not
     */
    operator bool() const { return ref.isValid(); }

    /**
     * @brief Returns whether the ref points to a valid command or not
     */
    bool isValid() const { return ref.isValid(); }

    /**
     * @brief Releases the handle to the command
     */
    void release() { ref.release(); }

    /**
     * @brief Sets the status to Current if Queued
     */
    void markInProgress() {
        if (ref->getStatus() == Command::Queued) {
            ref->status = Command::Current;
            bl::event::Dispatcher::dispatch<event::CommandStatusChange<T, Command::Current>>(
                {*ref});
        }
        else {
            BL_LOG_ERROR << "Cannot move command to Current from invalid state: "
                         << ref->getStatus();
        }
    }

    /**
     * @brief Sets the status to Complete if Current
     */
    void markComplete() {
        if (ref->getStatus() == Command::Current) {
            ref->status = Command::Complete;
            bl::event::Dispatcher::dispatch<event::CommandStatusChange<T, Command::Complete>>(
                {*ref});
        }
        else {
            BL_LOG_ERROR << "Cannot move command to Complete from invalid state: "
                         << ref->getStatus();
        }
    }

    /**
     * @brief Sets the status to Failed if Current
     */
    void markFailed() {
        if (ref->getStatus() == Command::Current) {
            ref->status = Command::Failed;
            bl::event::Dispatcher::dispatch<event::CommandStatusChange<T, Command::Failed>>({*ref});
        }
        else {
            BL_LOG_ERROR << "Cannot move command to Failed from invalid state: "
                         << ref->getStatus();
        }
    }

    /**
     * @brief Marks the command as canceled
     */
    void markCanceled() {
        ref->status = Command::Canceled;
        bl::event::Dispatcher::dispatch<event::CommandStatusChange<T, Command::Canceled>>({*ref});
    }

    /**
     * @brief Marks the command to be in the queued state
     */
    void markQueued() {
        ref->status = Command::Queued;
        bl::event::Dispatcher::dispatch<event::CommandStatusChange<T, Command::Queued>>({*ref});
    }

private:
    Ref<T> ref;

    ExecutorHandle(ExternalHandle<T>&& ext)
    : ref(std::move(ext.ref)) {}

    ExecutorHandle(const ExternalHandle<T>& ext)
    : ref(ext.ref) {}

    ExecutorHandle& operator=(ExternalHandle<T>&& ext) {
        ref = std::move(ext.ref);
        return *this;
    }

    ExecutorHandle& operator=(const ExternalHandle<T>& ext) {
        ref = ext.ref;
        return *this;
    }

    template<typename T, std::size_t N>
    friend class Queue;
};

} // namespace cmd
} // namespace core

#endif
