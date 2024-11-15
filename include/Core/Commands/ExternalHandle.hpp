#ifndef CORE_COMMANDS_EXTERNALHANDLE_HPP
#define CORE_COMMANDS_EXTERNALHANDLE_HPP

#include <BLIB/Events.hpp>
#include <Core/Commands/Events.hpp>
#include <Core/Commands/Ref.hpp>
#include <Core/Commands/SingleStore.hpp>

namespace core
{
namespace cmd
{
template<typename T>
class ExecutorHandle;
class CommandStore;

/**
 * @brief Handle to a command. This handle type is for UI, AI, and observers of commands
 *
 * @tparam T The command type
 * @ingroup Commands
 */
template<typename T>
class ExternalHandle {
public:
    /**
     * @brief Creates an empty handle
     */
    ExternalHandle() = default;

    /**
     * @brief Copies from another handle
     */
    ExternalHandle(const ExternalHandle&) = default;

    /**
     * @brief Moves from another handle
     */
    ExternalHandle(ExternalHandle&&) = default;

    /**
     * @brief Destroys the handle, potentially releasing the underlying command
     */
    ~ExternalHandle() = default;

    /**
     * @brief Copies from another handle
     */
    ExternalHandle& operator=(const ExternalHandle&) = default;

    /**
     * @brief Moves from another handle
     */
    ExternalHandle& operator=(ExternalHandle&&) = default;

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
     * @brief Cancels the underlying command
     */
    void cancel() {
        if (isValid() && (ref == Command::Queued || ref == Command::Current)) {
            ref->status = Command::Canceled;
            bl::event::Dispatcher::dispatch<CommandStatusChange<T, Command::Canceled>>({*ref});
        }
    }

private:
    Ref<T> ref;

    ExternalHandle(Ref<T>&& ref)
    : ref(std::forward<Ref<T>>(ref)) {}

    friend class CommandStore;
    friend class ExecutorHandle<T>;
};

} // namespace cmd
} // namespace core

#endif
