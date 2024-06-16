#ifndef CPPTOOLS_CONTAINER_TREE_TRAVERSAL_HPP
#define CPPTOOLS_CONTAINER_TREE_TRAVERSAL_HPP

#include <iterator>
#include <ranges>
#include <type_traits>

#include <cpptools/exception/iterator_exception.hpp>
#include <cpptools/utility/concepts.hpp>

#include "unsafe_tree.hpp"
#include "node.hpp"
#include "node_handle.hpp"

#ifndef CPPTOOLS_DEBUG_TRAVERSAL
# define CPPTOOLS_DEBUG_TRAVERSAL CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH
#endif

#define CPPTOOLS_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE
#define CPPTOOLS_LOCAL_DEBUG_MACRO CPPTOOLS_DEBUG_TRAVERSAL
#include <cpptools/_internal/debug_macros.hpp>

namespace tools {

namespace detail {

enum class order_t {
    pre,  // N L R
    post, // L R N
};

struct  pre_order_tag { static constexpr order_t order = order_t::pre; };
struct post_order_tag { static constexpr order_t order = order_t::post; };

template<order_t O>
using order_tag = std::conditional_t<
    O == order_t::pre,  pre_order_tag,  std::conditional_t<
    O == order_t::post, post_order_tag,
    void>
>;

/// @brief Get a pointer to the next node from this one, as in a
/// pre-order traversal of the tree
/// @return A pointer as described above
template<typename T, any_cvref<node<T>> Node>
Node* dfs_next(Node* n, pre_order_tag) CPPTOOLS_NOEXCEPT_RELEASE {
    // Structure:        Traversal:                     //
    //        N                --- N (begin)            //
    //      /   \             /                         //
    //     L     R           L   ------ R               //
    //    / \   / \         /   /      /                //
    //   L1 L2 R1 R2      L1 - L2    R1 - R2 - (end)    //

    const auto&  children = n->children();
    const node<T>* parent = n->parent();

    // first child
    if (!std::ranges::empty(children)) {
        return *std::ranges::begin(children);
    } else if (parent) [[likely]] {
        if (!n->is_rightmost_sibling()) {
            return n->right_sibling();
        } else {
            // find the first parent node which has a right sibling
            const node<T>* top_parent = parent;
            while (top_parent->parent() && top_parent->is_rightmost_sibling()) {
                top_parent = top_parent->parent();
            }

            // right sibling of this parent if such a parent was found
            if (top_parent->parent()) {
                return top_parent->right_sibling();
            }
        }
    }
    return nullptr;
}

/// @brief Get a pointer to the previous node from this one, as in a
/// pre-order traversal of the tree
/// @return A pointer as described above
/// @pre \c this node must not be orphaned.
template<typename T, any_cvref<node<T>> Node>
Node* dfs_previous(Node* n, pre_order_tag) CPPTOOLS_NOEXCEPT_RELEASE {
    // Structure:        Traversal (reversed):          //
    //        N                --- N - (end)            //
    //      /   \             /                         //
    //     L     R           L   ------ R               //
    //    / \   / \         /   /      /                //
    //   L1 L2 R1 R2      L1 - L2    R1 - R2 (begin)    //

    const node<T>* parent = n->parent();

    return (parent && !n->is_leftmost_sibling())
        ? n->left_sibling()->rightmost_child_or_this()
        : parent;
}

/// @brief Get a pointer to the next node from this one, as in a post-
/// order traversal of the tree
/// @return A pointer as described above
/// @pre \c this node must not be orphaned.
template<typename T, any_cvref<node<T>> Node>
Node* dfs_next(Node* n, post_order_tag) CPPTOOLS_NOEXCEPT_RELEASE {
    // Structure:                Traversal:             //
    //        N                   (end) - N --          //
    //      /   \                             \         //
    //     L     R                  L -----    R        //
    //    / \   / \                  \     \    \       //
    //   L1 L2 R1 R2     (begin) L1 - L2    R1 - R2     //

    const node<T>* parent = n->parent();

    return (parent && !n->is_rightmost_sibling())
        ? n->right_sibling()->leftmost_child_or_this()
        : parent;
}

/// @brief Get a pointer to the previous node from this one, as in a
/// post-order traversal of the tree
/// @return A pointer as described above
/// @pre \c this node must not be orphaned.
template<typename T, any_cvref<node<T>> Node>
Node* dfs_previous(Node* n, post_order_tag) CPPTOOLS_NOEXCEPT_RELEASE {
    // Structure:                Traversal (reversed):  //
    //        N                   (begin) N --          //
    //      /   \                             \         //
    //     L     R                  L -----    R        //
    //    / \   / \                  \     \    \       //
    //   L1 L2 R1 R2     (end) - L1 - L2    R1 - R2     //

    const auto& children = n->children();
    const node<T>*    parent   = n->parent();

    // last child
    if (!std::ranges::empty(children)) {
        return *(--std::ranges::end(children));
    } else if (parent) {
        if (!n->is_leftmost_sibling()) {
            return n->left_sibling();
        } else {
            // find the first parent node which has a left sibling
            const node<T>* top_parent = parent;
            while (top_parent->parent() && top_parent->is_leftmost_sibling()) {
                top_parent = top_parent->parent();
            }

            // left sibling of this parent if such a parent was found
            if (top_parent->parent()) {
                return top_parent->left_sibling();
            }
        }
    }
    return nullptr;
}

/// @brief Generic iterator to an element in a tree. Allows bidirectional
/// DFS traversal of the tree.
/// @tparam C Type of container to be DFS-traversed by the iterator
/// @tparam O Order of traversal to be implemented by the iterator
/// @note Iterators are invalidated only when the node they point to is no
/// longer part of the tree which it originally belonged to when the 
/// iterator was first instantiated.
template<typename T, order_t O>
class const_dfs_iterator {
protected:
    template<typename T2, order_t O2>
    friend class const_dfs_iterator;

    template<typename T2, order_t O2, bool Is_const>
    friend class dfs_proxy;

    using container_t           = unsafe_tree<T>;

public:
    using value_type          = typename container_t::value_type;
    using difference_type     = typename container_t::difference_type;
    using size_type           = typename container_t::size_type;
    using const_reference     = typename container_t::const_reference;
    using const_pointer       = typename container_t::const_pointer;
    using reference           = const_reference;
    using pointer             = const_pointer;
    using iterator_category   = std::bidirectional_iterator_tag;

protected:
    using node_t              = const typename container_t::_node_t;
    using order_tag_t         = order_tag<O>;
    using const_node_handle_t = const_node_handle<value_type>;

    /// @brief Pointer to the tree being traversed
    const container_t* _tree;

    /// @brief Pointer to the tree node currently being iterated over
    /// @note If _node is nullptr, then this iterator is a past-the-end iterator.
    node_t* _node;

    const_dfs_iterator(const container_t* tree, node_t* node) noexcept :
        _tree(tree),
        _node(node)
    {

    }

public:
    const_dfs_iterator() noexcept :
        _tree(nullptr),
        _node(nullptr)
    {

    }

    ~const_dfs_iterator() = default;

    template<order_t O2>
    const_dfs_iterator(const const_dfs_iterator<T, O2>& other) CPPTOOLS_NOEXCEPT_RELEASE :
        _tree(other._tree),
        _node(other._node)
    {

    }

    template<order_t O2>
    friend void swap(const_dfs_iterator& lhs, const_dfs_iterator<T, O2>& rhs) noexcept {
        std::swap(lhs._tree,  rhs._tree);
        std::swap(lhs._node,  rhs._node);
    }

    template<order_t O2>
    const_dfs_iterator& operator=(const_dfs_iterator<T, O2> other) noexcept {
        swap(*this, other);

        return *this;
    }

    const_dfs_iterator& operator++() CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_tree), "tree", critical, "cannot prefix-increment a default-constructed iterator", exception::iterator::incremented_past_end_error);
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "tree", critical, "cannot prefix-increment a past-the-end iterator",        exception::iterator::incremented_past_end_error);

        _node = dfs_next<T>(_node, order_tag_t{});

        return *this;
    }

    const_dfs_iterator operator++(int) CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_tree), "tree", critical, "cannot postfix-increment a default-constructed iterator", exception::iterator::incremented_past_end_error);
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "tree", critical, "cannot postfix-increment a past-the-end iterator",        exception::iterator::incremented_past_end_error);

        const_dfs_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    const_dfs_iterator& operator--() CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_tree),                              "tree", critical, "cannot prefix-decrement a default-constructed iterator", exception::iterator::decremented_past_begin_error);
        CPPTOOLS_DEBUG_ASSERT(_node != dfs_begin<T>(*_tree, order_tag_t{}), "tree", critical, "cannot prefix-decrement begin iterator",                 exception::iterator::decremented_past_begin_error);

        if (_node) [[likely]] {
            _node = dfs_previous<T>(_node, order_tag_t{});
        } else {
            _node = dfs_last<T>(*_tree, order_tag_t{});
        }

        return *this;
    }

    const_dfs_iterator operator--(int) CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_tree),                              "tree", critical, "cannot postfix-decrement a default-constructed iterator", exception::iterator::decremented_past_begin_error);
        CPPTOOLS_DEBUG_ASSERT(_node != dfs_begin<T>(*_tree, order_tag_t{}), "tree", critical, "cannot postfix-decrement begin iterator",                 exception::iterator::decremented_past_begin_error);

        const_dfs_iterator tmp = *this;
        --*this;
        return tmp;
    }

    reference operator*() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "tree", critical, "cannot dereference an iterator pointing at no node", exception::iterator::illegal_dereference_error);

        return _node->value;
    }

    pointer operator->() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "tree", critical, "cannot perform member access on an iterator pointing at no node", exception::iterator::illegal_dereference_error);

        return &(_node->value);
    }

    bool operator==(const const_dfs_iterator& rhs) const CPPTOOLS_NOEXCEPT_RELEASE {
        return (_node == rhs._node)
            && (_tree == rhs._tree);

        // comparing tree pointers ensures that t1.end() != t2.end()
    }

    const_node_handle_t as_node() const CPPTOOLS_NOEXCEPT_RELEASE {
        return { _node };
    }
};

/// @brief Generic iterator to an element in a tree. Allows bidirectional
/// DFS traversal of the tree.
/// @tparam T Type of value to be iterated upon by the iterator
/// @tparam O Order of traversal to be implemented by the iterator
/// @note Iterators are invalidated only when the node they point to is no
/// longer part of the tree which it originally belonged to when the 
/// iterator was first instantiated.
template<typename T, order_t O>
class dfs_iterator : private const_dfs_iterator<T, O> {

    template<typename T2, order_t O2>
    friend class dfs_iterator;

    template<typename T2, order_t O2, bool Is_const>
    friend class dfs_proxy;

    using base          = const_dfs_iterator<T, O>;
    using container_t   = typename base::container_t;
    using node_t        = typename base::node_t;
    using node_handle_t = node_handle<T>;

    template<order_t O2>
    dfs_iterator(const const_dfs_iterator<T, O2>& other) = delete;

    dfs_iterator(container_t* tree, node_t* node) noexcept :
        base(tree, node)
    {

    }

public:
    using value_type        = typename base::value_type;
    using difference_type   = typename base::difference_type;
    using size_type         = typename base::size_type;
    using const_reference   = typename base::const_reference;
    using const_pointer     = typename base::const_pointer;
    using reference         = typename container_t::reference;
    using pointer           = typename container_t::pointer;
    using iterator_category = typename base::iterator_category;

    /// @brief Default constructor
    dfs_iterator() noexcept :
        base(nullptr, nullptr)
    {

    }

    ~dfs_iterator() = default;

    template<order_t O2>
    dfs_iterator(const dfs_iterator<T, O2>& other) CPPTOOLS_NOEXCEPT_RELEASE :
        base(other._tree, other._node) {
    }

    template<order_t O2>
    friend void swap(dfs_iterator& lhs, dfs_iterator<T, O2>& rhs) noexcept {
        swap(
            static_cast<const_dfs_iterator<T, O>>(lhs),
            static_cast<const_dfs_iterator<T, O2>>(rhs)
        );
    }

    template<order_t O2>
    dfs_iterator& operator=(dfs_iterator<T, O2> other) noexcept {
        swap(*this, other);

        return *this;
    }

    dfs_iterator& operator++() CPPTOOLS_NOEXCEPT_RELEASE {
        base::operator++();

        return *this;
    }

    dfs_iterator operator++(int) CPPTOOLS_NOEXCEPT_RELEASE {
        dfs_iterator tmp = *this;
        base::operator++(0);

        return tmp;
    }

    dfs_iterator& operator--() CPPTOOLS_NOEXCEPT_RELEASE {
        base::operator--();

        return *this;
    }

    dfs_iterator operator--(int) CPPTOOLS_NOEXCEPT_RELEASE {
        dfs_iterator tmp = *this;
        base::operator--(0);

        return tmp;
    }

    reference operator*() const CPPTOOLS_NOEXCEPT_RELEASE {
        return const_cast<reference>(base::operator*());
    }

    pointer operator->() const CPPTOOLS_NOEXCEPT_RELEASE {
        return const_cast<pointer>(base::operator->());
    }

    bool operator==(const dfs_iterator& rhs) const CPPTOOLS_NOEXCEPT_RELEASE {
        return base::operator==(rhs);
    }

    node_handle_t as_node() CPPTOOLS_NOEXCEPT_RELEASE {
        return { this->_node };
    }
};

template<typename T, any_cvref<unsafe_tree<T>> Tree>
auto dfs_begin(Tree& t, pre_order_tag) noexcept
{
    return t.root();
}

template<typename T, any_cvref<unsafe_tree<T>> Tree>
auto dfs_last(Tree& t, pre_order_tag) noexcept
{
    return t.rightmost();
}

template<typename T, any_cvref<unsafe_tree<T>> Tree>
auto dfs_end(Tree& t, pre_order_tag) noexcept
{
    using node_t = std::conditional_t<std::is_const_v<Tree>, const typename Tree::node_t, typename Tree::node_t>;
    return static_cast<node_t*>(nullptr);
}

template<typename T, any_cvref<unsafe_tree<T>> Tree>
auto dfs_begin(Tree& t, post_order_tag) noexcept
{
    return t.leftmost();
}

template<typename T, any_cvref<unsafe_tree<T>> Tree>
auto dfs_last(Tree& t, post_order_tag) noexcept
{
    return t.root();
}

template<typename T, any_cvref<unsafe_tree<T>> Tree>
auto dfs_end(Tree& t, post_order_tag) noexcept
{
    using node_t = std::conditional_t<std::is_const_v<Tree>, const typename Tree::node_t, typename Tree::node_t>;
    return static_cast<node_t*>(nullptr);
}

template<typename T, order_t O, bool Is_const>
class dfs_proxy {
    using container_t = std::conditional_t<Is_const, const unsafe_tree<T>, unsafe_tree<T>>;
    using order_tag_t = order_tag<O>;

    container_t& _tree;

    dfs_proxy(const dfs_proxy& other)            = default;
    dfs_proxy(dfs_proxy&& other)                 = default;
    dfs_proxy& operator=(const dfs_proxy& other) = delete;
    dfs_proxy& operator=(dfs_proxy&& other)      = delete;

public:
    dfs_proxy(container_t& tree) noexcept : _tree(tree) {}

    using iterator = std::conditional_t<Is_const,
        const_dfs_iterator<T, O>, dfs_iterator<T, O>
    >;

    iterator begin() const noexcept {
        return iterator(&_tree, dfs_begin<T>(_tree, order_tag_t{}));
    }

    iterator end() const noexcept {
        return iterator(&_tree, dfs_end<T>(_tree, order_tag_t{}));
    }
};

template<typename T, order_t O, bool Is_const>
class reverse_dfs_proxy {
    using container_t = std::conditional_t<Is_const, const unsafe_tree<T>, unsafe_tree<T>>;

    dfs_proxy<T, O, Is_const> _forward_dfs;

    reverse_dfs_proxy(const reverse_dfs_proxy& other)            = default;
    reverse_dfs_proxy(reverse_dfs_proxy&& other)                 = default;
    reverse_dfs_proxy& operator=(const reverse_dfs_proxy& other) = delete;
    reverse_dfs_proxy& operator=(reverse_dfs_proxy&& other)      = delete;

public:
    reverse_dfs_proxy(container_t& tree) noexcept : _forward_dfs(tree) {}

    using iterator = std::reverse_iterator<
        std::conditional_t<Is_const,
            const_dfs_iterator<T, O>, dfs_iterator<T, O>
    >>;

    iterator begin() const noexcept {
        return iterator(_forward_dfs.end());
    }

    iterator end() const noexcept {
        return iterator(_forward_dfs.begin());
    }
};

template<order_t O, typename T>
detail::dfs_proxy<T, O, true> dfs(const unsafe_tree<T>& t) {
    return { t };
}

template<order_t O, typename T>
detail::dfs_proxy<T, O, false> dfs(unsafe_tree<T>& t) {
    return { t };
}

template<order_t O, typename T>
detail::reverse_dfs_proxy<T, O, true> reverse_dfs(const unsafe_tree<T>& t) {
    return { t };
}

template<order_t O, typename T>
detail::reverse_dfs_proxy<T, O, false> reverse_dfs(unsafe_tree<T>& t) {
    return { t };
}

} // namespace detail

namespace traversal {

using order = detail::order_t;

inline constexpr order pre_order  = order::pre;
inline constexpr order post_order = order::post;

} // namespace traversal

} // namespace tools

#include <cpptools/_internal/undef_debug_macros.hpp>

#endif//CPPTOOLS_CONTAINER_TREE_TRAVERSAL_HPP