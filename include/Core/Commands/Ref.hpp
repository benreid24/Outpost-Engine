#ifndef CORE_COMMANDS_REF_HPP
#define CORE_COMMANDS_REF_HPP

#include <Core/Commands/Command.hpp>
#include <type_traits>

namespace core
{
namespace cmd
{
template<typename T>
class SingleStore;

/**
 * @brief Reference to a stored command. Performs ref counting
 *
 * @tparam T The derived command type
 * @ingroup Commands
 */
template<typename T>
class Ref {
    static_assert(std::is_base_of_v<Command, T>, "T must derive from Command");

public:
    /**
     * @brief Creates an empty ref
     */
    Ref()
    : owner(nullptr) {}

    /**
     * @brief Copies from the given ref
     *
     * @param copy The ref to copy from
     */
    Ref(const Ref& copy)
    : owner(copy.owner)
    , i(copy.i) {
        if (owner) { increment(); }
    }

    /**
     * @brief Moves from the given ref
     *
     * @param move The ref to move from
     */
    Ref(Ref&& move)
    : owner(move.owner)
    , i(move.i) {
        move.owner = nullptr;
    }

    /**
     * @brief Destroys this ref, possibly freeing the underlying command
     */
    ~Ref() { release(); }

    /**
     * @brief Releases this ref, possibly freeing the underlying command
     */
    void release();

    /**
     * @brief Copies from the given ref
     *
     * @param copy The ref to copy from
     * @return A reference to this ref
     */
    Ref& operator=(const Ref& copy) {
        release();
        owner = copy.owner;
        i     = copy.i;
        if (owner) { increment(); }
        return *this;
    }

    /**
     * @brief Moves from the given ref
     *
     * @param move The ref to move from
     * @return A reference to this ref
     */
    Ref& operator=(Ref&& move) {
        release();
        owner      = move.owner;
        i          = move.i;
        move.owner = nullptr;
        return *this;
    }

    /**
     * @brief Access the underlying command
     */
    T* operator->();

    /**
     * @brief Access the underlying command
     */
    const T* operator->() const;

    /**
     * @brief Access the underlying command
     */
    T& operator*();

    /**
     * @brief Access the underlying command
     */
    const T& operator*() const;

    /**
     * @brief Returns whether the ref points to a valid command or not
     */
    operator bool() const { return owner; }

    /**
     * @brief Returns whether the ref points to a valid command or not
     */
    bool isValid() const { return owner; }

private:
    SingleStore<T>* owner;
    std::size_t i;

    Ref(SingleStore<T>* owner, std::size_t i)
    : owner(owner)
    , i(i) {}

    void increment();

    friend class SingleStore<T>;
};

} // namespace cmd
} // namespace core

#endif
