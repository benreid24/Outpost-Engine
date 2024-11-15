#ifndef CORE_COMMANDS_SINGLESTORE_HPP
#define CORE_COMMANDS_SINGLESTORE_HPP

#include <BLIB/Containers/ObjectWrapper.hpp>
#include <BLIB/Util/IdAllocatorUnbounded.hpp>
#include <Core/Commands/Ref.hpp>
#include <Core/Constants.hpp>
#include <type_traits>

namespace core
{
namespace cmd
{
/**
 * @brief Storage for a specific command type. Use CommandStore
 *
 * @tparam T The command type to store
 * @ingroup Commands
 */
template<typename T>
class SingleStore {
    static_assert(std::is_base_of_v<Command, T>, "T must derive from Command");

public:
    /**
     * @brief Creates the command store
     */
    SingleStore();

    /**
     * @brief Creates a new command and returns a reference to it
     *
     * @tparam ...TArgs Argument types to the command's constructor
     * @param ...args Arguments to the command's constructor
     * @return A ref to the newly created command
     */
    template<typename... TArgs>
    Ref<T> allocate(TArgs&&... args);

private:
    std::vector<bl::ctr::ObjectWrapper<T>> pool;
    std::vector<std::uint32_t> refCounts;
    bl::util::IdAllocatorUnbounded<std::size_t> freeIndices;

    void increment(std::size_t i);
    void decrement(std::size_t i);

    friend class Ref<T>;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
void Ref<T>::increment() {
    owner->increment(i);
}

template<typename T>
void Ref<T>::release() {
    if (owner) {
        owner->decrement(i);
        owner = nullptr;
    }
}

template<typename T>
T* Ref<T>::operator->() {
    return &owner->pool[i].get();
}

template<typename T>
const T* Ref<T>::operator->() const {
    return &owner->pool[i].get();
}

template<typename T>
T& Ref<T>::operator*() {
    return owner->pool[i].get();
}

template<typename T>
const T& Ref<T>::operator*() const {
    return owner->pool[i].get();
}

template<typename T>
SingleStore<T>::SingleStore()
: freeIndices(Constants::CommandPoolInitialSize) {
    pool.resize(Constants::CommandPoolInitialSize);
    refCounts.resize(Constants::CommandPoolInitialSize, 0);
}

template<typename T>
void SingleStore<T>::increment(std::size_t i) {
    ++refCounts[i];
}

template<typename T>
void SingleStore<T>::decrement(std::size_t i) {
    --refCounts[i];
    if (refCounts[i] == 0) { freeIndices.release(i); }
}

template<typename T>
template<typename... TArgs>
Ref<T> SingleStore<T>::allocate(TArgs&&... args) {
    const std::size_t i = freeIndices.allocate();
    if (i >= pool.size()) {
        pool.resize(pool.size() * 2);
        refCounts.resize(pool.size(), 0);
    }
    refCounts[i] = 1;
    pool[i].emplace(std::forward<TArgs>(args)...);
    return Ref<T>(this, i);
}

} // namespace cmd
} // namespace core

#endif
