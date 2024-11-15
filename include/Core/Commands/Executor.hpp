#ifndef CORE_COMMANDS_EXECUTOR_HPP
#define CORE_COMMANDS_EXECUTOR_HPP

#include <Core/Commands/ExecutorHandle.hpp>
#include <Core/Commands/ExternalHandle.hpp>

namespace core
{
namespace cmd
{
/**
 * @brief Base class for executors of commands. Allows the conversion of ExternalHandle to
 *        ExecutorHandle in order to allow status changing
 *
 * @tparam T The type of commands that will be executed
 * @ingroup Commands
 */
template<typename T>
class Executor {
protected:
    /**
     * @brief Creates an executor handle from the given external command handle
     *
     * @param extHandle The handle to convert
     * @return An executor handle for the same command
     */
    ExecutorHandle<T> makeCommandHandle(ExternalHandle<T>&& extHandle) {
        return ExecutorHandle<T>(std::forward<ExternalHandle<T>>(extHandle));
    }

    /**
     * @brief Creates an executor handle from the given external command handle
     *
     * @param extHandle The handle to convert
     * @return An executor handle for the same command
     */
    ExecutorHandle<T> makeCommandHandle(const ExternalHandle<T>& extHandle) {
        return ExecutorHandle<T>(extHandle);
    }
};

} // namespace cmd
} // namespace core

#endif
