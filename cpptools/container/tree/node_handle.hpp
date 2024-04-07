#ifndef CPPTOOLS__CONTAINER__TREE__NODE_HANDLE_HPP
#define CPPTOOLS__CONTAINER__TREE__NODE_HANDLE_HPP

#include <cpptools/exception/internal_exception.hpp>

#include "node.hpp"

#ifndef CPPTOOLS_DEBUG_NODE_HANDLE
# define CPPTOOLS_DEBUG_NODE_HANDLE CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH
#endif

#define CPPTOOLS_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE
#define CPPTOOLS_LOCAL_DEBUG_MACRO CPPTOOLS_DEBUG_NODE_HANDLE
#include <cpptools/_internal/debug_macros.hpp>

namespace tools {

template<typename>
class tree;

namespace detail {

/// @brief Safe wrapper for node<T>, to be used with tools::tree<T>.
///
/// @note Enable debug assertions with #define CPPTOOLS_DEBUG_NODE_HANDLE 1
template<typename T>
class const_node_handle {
protected:
    using node_t     = detail::node<T>;
    using node_ptr   = const node_t*;

    node_ptr _node;

    node_ptr ptr() const {
        return _node;
    }

    template<typename>
    friend class tools::tree;

public:
    const_node_handle(node_ptr from_node) :
        _node(from_node)
    {

    }

    const_node_handle() :
        const_node_handle(nullptr)
    {

    }

    static const const_node_handle null_handle;

    using value_type      = T;
    using size_type       = typename node_t::size_type;
    using const_reference = const T&;
    using const_pointer   = const T*;
    using reference       = const_reference;
    using pointer         = const_pointer;

    const_node_handle(const const_node_handle&  other)            = default;
    const_node_handle(      const_node_handle&& other)            = default;
    const_node_handle& operator=(const const_node_handle&  other) = default;
    const_node_handle& operator=(      const_node_handle&& other) = default;

    const_reference value() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return _node->value;
    }

    const_node_handle left_sibling() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return const_node_handle(_node->left_sibling());
    }

    const_node_handle right_sibling() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return const_node_handle(_node->right_sibling());
    }

    bool is_leftmost_sibling() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return _node->is_leftmost_sibling();
    }

    bool is_rightmost_sibling() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return _node->is_rightmost_sibling();
    }

    const_node_handle parent() const CPPTOOLS_NOEXCEPT_RELEASE {
        return const_node_handle(_node->parent());
    }

    bool has_parent(const const_node_handle& other) const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return _node->has_parent(other._node);
    }

    bool is_parent_of(const const_node_handle& other) const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return other.has_parent(*this);
    }

    const_node_handle child(size_type index) const {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node),              "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);
        CPPTOOLS_DEBUG_ASSERT(index < _node->child_count(), "const_node_handle", critical, "index out of bounds", exception::parameter::invalid_value_error, "index", index);
        
        return const_node_handle(_node->children()[index]);
    };

    auto children() const {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);
        
        return _node->children() | std::views::transform([](node_t* n) { return const_node_handle(n); });
    }

    size_type child_count() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return _node->child_count();
    }

    size_type descendant_count() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return _node->descendant_count();
    }

    size_t sibling_index() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return _node->sibling_index();
    }

    const_reference operator*() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return value();
    }

    const_pointer operator->() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return &(value());
    }

    bool operator==(const const_node_handle& other) const noexcept {
        return _node == other._node;
    }
};

template<typename T>
const const_node_handle<T> const_node_handle<T>::null_handle = const_node_handle<T>();

template<typename T>
class node_handle : public const_node_handle<T> {
private:
    using base     = const_node_handle<T>;
    using node_t   = typename base::node_t;
    using node_ptr = node_t*;

    node_ptr ptr() const {
        return const_cast<node_t*>(this->_node);
    }

    node_handle(const base&  other)            = delete;
    node_handle(      base&& other)            = delete;
    node_handle& operator=(const base&  other) = delete;
    node_handle& operator=(      base&& other) = delete;

    template<typename>
    friend class tools::tree;

public:
    using base::const_node_handle;

    static const node_handle null_handle;

    using value_type      = T;
    using size_type       = typename base::size_type;
    using const_reference = typename base::const_reference;
    using const_pointer   = typename base::const_pointer;
    using reference       = T&;
    using pointer         = T*;

    node_handle(const node_handle&  other)            = default;
    node_handle(      node_handle&& other)            = default;
    node_handle& operator=(const node_handle&  other) = default;
    node_handle& operator=(      node_handle&& other) = default;

    reference value() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(this->_node), "const_node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return const_cast<reference>(base::value());
    }

    node_handle left_sibling() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(this->_node), "node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return node_handle(const_cast<node_t*>(this->_node->left_sibling()));
    }

    node_handle right_sibling() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(this->_node), "node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);

        return node_handle(const_cast<node_t*>(this->_node->right_sibling()));
    }

    node_handle parent() const CPPTOOLS_NOEXCEPT_RELEASE {
        return node_handle(const_cast<node_t*>(this->_node->parent()));
    }

    bool has_parent(const node_handle& other) const CPPTOOLS_NOEXCEPT_RELEASE {
        return base::has_parent(static_cast<const base&>(other));
    }

    bool is_parent_of(const node_handle& other) const CPPTOOLS_NOEXCEPT_RELEASE {
        return base::is_parent_of(static_cast<const base&>(other));
    }

    using base::is_leftmost_sibling;
    using base::is_rightmost_sibling;
    using base::child_count;
    using base::descendant_count;
    using base::sibling_index;

    node_handle child(size_type index) const {
        CPPTOOLS_DEBUG_ASSERT(not_null(this->_node),              "node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);
        CPPTOOLS_DEBUG_ASSERT(index < this->_node->child_count(), "node_handle", critical, "index out of bounds", exception::parameter::invalid_value_error, "index", index);
        
        return node_handle(this->_node->children()[index]);
    };

    auto children() const {
        CPPTOOLS_DEBUG_ASSERT(not_null(this->_node), "node_handle", critical, "node handle is null", exception::internal::precondition_failure_error);
        
        return this->_node->children() | std::views::transform([](node_t* n) { return node_handle(n); });
    }

    reference operator*() const CPPTOOLS_NOEXCEPT_RELEASE {
        return value();
    }

    pointer operator->() const CPPTOOLS_NOEXCEPT_RELEASE {
        return &(value());
    }

    bool operator==(const node_handle& other) const noexcept {
        return base::operator==(other);
    }
};

template<typename T>
const node_handle<T> node_handle<T>::null_handle = node_handle<T>();

} // namespace detail

template<typename T>
using const_node_handle = detail::const_node_handle<T>;

template<typename T>
using node_handle = detail::node_handle<T>;

} // namespace tools

#include <cpptools/_internal/undef_debug_macros.hpp>

#endif//CPPTOOLS__CONTAINER__TREE__NODE_HANDLE_HPP