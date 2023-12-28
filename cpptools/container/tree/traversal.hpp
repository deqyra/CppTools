#ifndef CPPTOOLS__CONTAINER__TREE__TRAVERSAL_HPP
#define CPPTOOLS__CONTAINER__TREE__TRAVERSAL_HPP

#include <type_traits>
#include <vector>

#include <cpptools/container/tree/node.hpp>
#include <cpptools/container/tree/unsafe_tree.hpp>

#include <cpptools/exception/iterator_exception.hpp>

#ifndef CPPTOOLS_DEBUG_TRAVERSAL
# if CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH != 0
#   define CPPTOOLS_DEBUG_TRAVERSAL 1
# else
#   define CPPTOOLS_DEBUG_TRAVERSAL 0
# endif
#endif

#define CPPTOOLS_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE
#define CPPTOOLS_LOCAL_DEBUG_MACRO CPPTOOLS_DEBUG_TRAVERSAL
#include <cpptools/_internal/debug_macros.hpp>

namespace tools::detail::tree::traversal
{

enum class order {
    pre_order,  // N L R
    post_order, // L R N
};

struct  pre_order_tag { static constexpr order order = order::pre_order; };
struct post_order_tag { static constexpr order order = order::post_order; };

template<order O>
using order_tag = std::conditional_t<
    O == order::pre_order,  pre_order_tag,  std::conditional_t<
    O == order::post_order, post_order_tag,
    void>
>;

/// @brief Get a pointer to the next node from this one, as in a
/// pre-order traversal of the tree
///
/// @return A pointer as described above
template<typename T>
node<T>* dfs_next(const node<T>& n, pre_order_tag) CPPTOOLS_NOEXCEPT_RELEASE {
    CPPTOOLS_DEBUG_ASSERT(!is_orphaned(), "tree", critical, "cannot iterate over an orphaned node", exception::internal::precondition_failure_error);

    // Structure:        Traversal:                     //
    //        N                --- N (begin)            //
    //      /   \             /                         //
    //     L     R           L   ------ R               //
    //    / \   / \         /   /      /                //
    //   L1 L2 R1 R2      L1 - L2    R1 - R2 - (end)    //

    const auto&  children = n.children();
    const node<T>* parent = n.parent();

    // first child
    if (!children.empty()) {
        return children.front();
    } else if (parent) [[likely]] {
        if (!is_rightmost_sibling()) {
            return right_sibling();
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
///
/// @return A pointer as described above
///
/// @pre \c this node must not be orphaned.
template<typename T>
node<T>* dfs_previous(const node<T>& n, pre_order_tag) CPPTOOLS_NOEXCEPT_RELEASE {
    CPPTOOLS_DEBUG_ASSERT(!is_orphaned(), "tree", critical, "cannot iterate over an orphaned node", exception::internal::precondition_failure_error);

    // Structure:        Traversal (reversed):          //
    //        N                --- N - (end)            //
    //      /   \             /                         //
    //     L     R           L   ------ R               //
    //    / \   / \         /   /      /                //
    //   L1 L2 R1 R2      L1 - L2    R1 - R2 (begin)    //

    const node<T>* parent = n.parent();

    return (parent && !is_leftmost_sibling())
        ? left_sibling()->rightmost_child_or_this()
        : parent;
}

/// @brief Get a pointer to the next node from this one, as in a post-
/// order traversal of the tree
///
/// @return A pointer as described above
///
/// @pre \c this node must not be orphaned.
template<typename T>
node<T>* dfs_next(const node<T>& n, post_order_tag) CPPTOOLS_NOEXCEPT_RELEASE {
    CPPTOOLS_DEBUG_ASSERT(!is_orphaned(), "tree", critical, "cannot iterate over an orphaned node", exception::internal::precondition_failure_error);

    // Structure:                Traversal:             //
    //        N                   (end) - N --          //
    //      /   \                             \         //
    //     L     R                  L -----    R        //
    //    / \   / \                  \     \    \       //
    //   L1 L2 R1 R2     (begin) L1 - L2    R1 - R2     //

    const node<T>* parent = n.parent();

    return (parent && !is_rightmost_sibling())
        ? right_sibling()->leftmost_child_or_this()
        : parent;
}

/// @brief Get a pointer to the previous node from this one, as in a
/// post-order traversal of the tree
///
/// @return A pointer as described above
///
/// @pre \c this node must not be orphaned.
template<typename T>
node<T>* dfs_previous(const node<T>& n, post_order_tag) CPPTOOLS_NOEXCEPT_RELEASE {
    CPPTOOLS_DEBUG_ASSERT(!is_orphaned(), "tree", critical, "cannot iterate over an orphaned node", exception::internal::precondition_failure_error);

    // Structure:                Traversal (reversed):  //
    //        N                   (begin) N --          //
    //      /   \                             \         //
    //     L     R                  L -----    R        //
    //    / \   / \                  \     \    \       //
    //   L1 L2 R1 R2     (end) - L1 - L2    R1 - R2     //

    const auto&  children = n.children();
    const node<T>* parent = n.parent();

    // last child
    if (!children.empty()) {
        return children.back();
    } else if (parent) {
        if (!is_leftmost_sibling()) {
            return left_sibling();
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
/// 
/// @tparam T Type of value to be iterated upon by the iterator
/// @tparam O Order of traversal to be implemented by the iterator
///
/// @note Iterators are invalidated only when the node they point to is no
/// longer part of the tree which it originally belonged to when the 
/// iterator was first instantiated.
template<typename T, order O>
class const_dfs_iterator {
protected:
    template<order O2>
    friend class const_dfs_iterator;

    using container         = unsafe_tree<T>;
    using const_node_handle = const_node_handle<T, container>;

public:
    using value_type        = typename container::value_type;
    using difference_type   = typename container::difference_type;
    using size_type         = typename container::size_type;
    using const_reference   = typename container::const_reference;
    using const_pointer     = typename container::const_pointer;
    using reference         = const_reference;
    using pointer           = const_pointer;
    using iterator_category = std::bidirectional_iterator_tag;

protected:
    using tree_ptr = const container*;
    using node_ptr = const typename container::node*;
    using order_tag = order_tag<O>;

    /// @brief Pointer to the tree being traversed
    tree_ptr _tree;

    /// @brief Pointer to the tree node currently being iterated over
    ///
    /// @note If _node is nullptr, then this iterator is a past-the-end iterator.
    node_ptr _node;

    const_dfs_iterator(tree_ptr tree, node_ptr node) noexcept :
        _tree(tree),
        _node(node) {
    }
public:

    const_dfs_iterator() noexcept :
        _tree(nullptr),
        _node(nullptr) {

    }

    ~const_dfs_iterator() = default;

    template<order O2>
    const_dfs_iterator(const const_dfs_iterator<T, O2>& other) CPPTOOLS_NOEXCEPT_RELEASE :
        _tree(other._tree),
        _node(other._node) {
    }

    template<order O2>
    friend void swap(const_dfs_iterator& lhs, const_dfs_iterator<T, O2>& rhs) noexcept {
        std::swap(lhs._tree,  rhs._tree);
        std::swap(lhs._node,  rhs._node);
    }

    template<order O2>
    const_dfs_iterator& operator=(const_dfs_iterator<T, O2> other) noexcept {
        swap(*this, other);

        return *this;
    }

    const_dfs_iterator& operator++() CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "tree", critical, "cannot prefix-increment a past-the-end iterator", exception::iterator::incremented_past_end_error);

        _node = dfs_next(*_node, order_tag{});

        return *this;
    }

    const_dfs_iterator operator++(int) CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_node), "tree", critical, "cannot postfix-increment a past-the-end iterator", exception::iterator::incremented_past_end_error);

        const_dfs_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    const_dfs_iterator& operator--() CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(*this != _tree->template begin<O>(), "tree", critical, "cannot prefix-decrement begin iterator",      exception::iterator::decremented_past_begin_error);

        if (_node) [[likely]] {
            _node = dfs_previous(*_node, order_tag{});
        } else {
            _node = end_node_for(_tree, order_tag{});
        }

        return *this;
    }

    const_dfs_iterator operator--(int) CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(*this != _tree->template begin<O>(), "tree", critical, "cannot postfix-decrement begin iterator",      exception::iterator::decremented_past_begin_error);

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
        CPPTOOLS_DEBUG_ASSERT(rhs._is_valid(), "tree", critical, "cannot compare iterator with an invalid iterator", exception::parameter::invalid_value_error, "rhs");

        return (_node == rhs._node)
            || (_tree == rhs._tree);

        // comparing tree pointers ensures that t1.end() != t2.end()

        // tree comparison can be short-circuited by nodes being equal,
        // since it's been asserted that both iterators are valid
        // (in a case where both iterators point to the same node but different
        // trees, at least one iterator has to be invalid)
    }

    bool operator!=(const const_dfs_iterator& rhs) const CPPTOOLS_NOEXCEPT_RELEASE {
        return !(*this == rhs);
    }

    const_node_handle as_node() const CPPTOOLS_NOEXCEPT_RELEASE {
        return const_node_handle(_node);
    }
};

/// @brief Generic iterator to an element in a tree. Allows bidirectional
/// DFS traversal of the tree.
/// 
/// @tparam T Type of value to be iterated upon by the iterator
/// @tparam O Order of traversal to be implemented by the iterator
///
/// @note Iterators are invalidated only when the node they point to is no
/// longer part of the tree which it originally belonged to when the 
/// iterator was first instantiated.
template<typename T, order O>
class dfs_iterator : private const_dfs_iterator<T, O> {

    template<order O2>
    friend class dfs_iterator;

    using base       = const_dfs_iterator<T, O>;
    using container  = base::container;
    using node_handle = node_handle<T, container>;
    friend container;


    template<order O2>
    dfs_iterator(const const_dfs_iterator<T, O2>& other) = delete;

    using base::const_dfs_iterator;

public:
    using value_type        = typename base::value_type;
    using difference_type   = typename base::difference_type;
    using size_type         = typename base::size_type;
    using const_reference   = typename base::const_reference;
    using const_pointer     = typename base::const_pointer;
    using reference         = typename container::reference;
    using pointer           = typename container::pointer;
    using iterator_category = typename base::iterator_category;

    /// @brief Default constructor
    dfs_iterator() noexcept :
        base(nullptr, nullptr) {

    }

    ~dfs_iterator() = default;

    template<order O2>
    dfs_iterator(const dfs_iterator<T, O2>& other) CPPTOOLS_NOEXCEPT_RELEASE :
        base(other._tree, other._node) {
    }

    template<order O2>
    friend void swap(dfs_iterator& lhs, dfs_iterator<T, O2>& rhs) noexcept {
        std::swap(
            static_cast<const_dfs_iterator<T, O>>(lhs),
            static_cast<const_dfs_iterator<T, O2>>(rhs)
        );
    }

    template<order O2>
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
        return static_cast<base>(*this) == static_cast<base>(rhs);
    }

    bool operator!=(const dfs_iterator& rhs) const CPPTOOLS_NOEXCEPT_RELEASE {
        return !(*this == rhs);
    }

    node_handle as_node() CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(_is_valid(),     "tree", critical, "cannot make node handle from invalid iterator", exception::iterator::operation_while_invalid_error);

        return node_handle(this->_node);
    }
};

template<typename T>
node<T>* dfs_begin(const unsafe_tree<T>& t, pre_order_tag) noexcept
{
    return t.root();
}
template<typename T>
node<T>* dfs_end(const unsafe_tree<T>& t, pre_order_tag) noexcept
{
    return t.rightmost();
}

template<typename T>
node<T>* dfs_begin(const unsafe_tree<T>& t, post_order_tag) noexcept
{
    return t.leftmost();
}

template<typename T>
node<T>* dfs_end(const unsafe_tree<T>& t, post_order_tag) noexcept
{
    return t.root();
}

template<typename T, order O, bool Is_const>
class dfs_proxy {
    using tree = std::conditional_t<Is_const, const unsafe_tree<T>, unsafe_tree<T>>;
    using order_tag = order_tag<order::pre_order>;

    tree& _tree;

    dfs_proxy(const dfs_proxy& other)            = delete;
    dfs_proxy(dfs_proxy&& other)                 = delete;
    dfs_proxy& operator=(const dfs_proxy& other) = delete;
    dfs_proxy& operator=(dfs_proxy&& other)      = delete;

public:
    dfs_proxy(tree& tree) noexcept : _tree(tree) {}

    using iterator = std::conditional_t<Is_const,
        const_dfs_iterator<T, O>, dfs_iterator<T, O>
    >;

    iterator begin() const noexcept {
        return iterator(_tree, dfs_begin(_tree, order_tag{}));
    }

    iterator end() const noexcept {
        return iterator(_tree, dfs_end(_tree, order_tag{}));
    }
};

} // namespace tools::detail::tree::traversal

//////////
// TODO //
//////////

/*
 - reassess iterator invalidation
 */

#include <cpptools/_internal/undef_debug_macros.hpp>

#endif//CPPTOOLS__CONTAINER__TREE__TRAVERSAL_HPP