#ifndef CPPTOOLS_CONTAINER_THREAD_SAFE_LIST_HPP
#define CPPTOOLS_CONTAINER_THREAD_SAFE_LIST_HPP

#include <condition_variable>
#include <mutex>
#include <optional>
#include <type_traits>
#include <utility>
#include <set>
#include <stop_token>

#include <cpptools/exception/internal_exception.hpp>

#ifndef CPPTOOLS_DEBUG_THREAD_SAFE_LIST
# define CPPTOOLS_DEBUG_THREAD_SAFE_LIST CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH
#endif

#define CPPTOOLS_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE
#define CPPTOOLS_LOCAL_DEBUG_MACRO CPPTOOLS_DEBUG_THREAD_SAFE_LIST
#include <cpptools/_internal/debug_macros.hpp>

namespace tools {

template<typename T>
class thread_safe_set {
    std::set<T> _set;
    std::mutex _mtx;
    std::condition_variable_any _cv;

    T _locked_pop() {
        auto it = _set.begin();

        CPPTOOLS_DEBUG_ASSERT(it != _set.end(), "thread_safe_set", critical, "set is empty", exception::internal::precondition_failure_error);

        T result = std::move(*it);
        _set.erase(it);

        return result;
    }

public:
    thread_safe_set() = default;

    /// @brief Insert a value if not already present
    /// @param val Value to insert
    void insert(T val) {
        std::lock_guard l(_mtx);
        _set.insert(std::move(val));
    }

    /// @brief Erase an element from the set if present
    /// @param val Value to erase
    bool remove(const T& val) {
        std::lock_guard l(_mtx);
        return _set.erase(val) != 0;
    }

    /// @brief Pop the first value from the set, waiting for one if none is available immediately
    /// @return The popped value
    T pop() {
        std::unique_lock l(_mtx);
        if (_set.empty()) {
            _cv.wait(l, [&](){ return !_set.empty(); });
        }

        return _locked_pop();
    }

    /// @brief Pop the first value from the set, waiting for one if none is available immediately, until cancellation is
    /// requested
    /// @param stoken Token to indicate stopping to wait for a value to pop
    /// @return The popped value, or std::nullopt if a stop was requested
    std::optional<T> pop(std::stop_token stoken) {
        std::unique_lock l(_mtx);
        if (_set.empty()) {
            _cv.wait(l, stoken, [&](){ return !_set.empty(); });
        }

        if (stoken.stop_requested()) {
            return std::nullopt;
        }

        return std::make_optional(_locked_pop());
    }

    /// @brief Pop the first value from the set if one is available immediately, return nothing otherwise
    /// @return A value if one could be popped, std::nullopt if none was available
    std::optional<int> try_pop() {
        std::lock_guard l(_mtx);
        if (_set.empty()) {
            return std::nullopt;
        }

        return _locked_pop();
    }

    /// @brief Run a callable on every element of the set. The callable should be invocable with a const ref to an
    /// element of the set and should return whether or not iteration should continue.
    /// @param visitor A callable as described above
    template<typename F>
    requires std::is_invocable_r_v<bool, F, const T&>
    void visit(F&& visitor) {
        bool should_continue = true;
        std::lock_guard l(_mtx);
        for (auto it = _set.begin(); it != _set.end() && should_continue; ++it) {
            should_continue = std::forward<F>(visitor)(*it);
        }
    }

    /// @brief Run a callable on every element of the set. The callable should be invocable with a const ref to an
    /// element of the set and should return whether or not iteration should continue.
    /// @param visitor A callable as described above
    template<typename F>
    requires std::is_invocable_r_v<bool, F, const T&>
    void visit_and_pop(F&& visitor) {
        bool should_continue = true;
        std::lock_guard l(_mtx);
        for (auto it = _set.begin(); it != _set.end() && should_continue;) {
            should_continue = std::forward<F>(visitor)(*it);
            it = _set.erase(it); // ok: std::set is stable under erasure
        }
    }
};

} // namespace tools

#endif//CPPTOOLS_CONTAINER_THREAD_SAFE_LIST_HPP
