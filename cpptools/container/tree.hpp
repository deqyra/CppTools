#ifndef CPPTOOLS__CONTAINER__TREE_HPP
#define CPPTOOLS__CONTAINER__TREE_HPP

#include <algorithm>
#include <utility>

#include <cpptools/container/tree/node.hpp>
#include <cpptools/container/tree/traversal.hpp>
#include <cpptools/container/tree/unsafe_tree.hpp>

#include <cpptools/exception/exception.hpp>
#include <cpptools/exception/internal_exception.hpp>
#include <cpptools/exception/parameter_exception.hpp>
#include <cpptools/exception/iterator_exception.hpp>
#include <cpptools/utility/merging_strategy.hpp>
#include <cpptools/utility/type_utils.hpp>

#ifndef CPPTOOLS_DEBUG_TREE
# if CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH != 0
#   define CPPTOOLS_DEBUG_TREE 1
# else
#   define CPPTOOLS_DEBUG_TREE 0
# endif
#endif

#define CPPTOOLS_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE
#define CPPTOOLS_LOCAL_DEBUG_MACRO CPPTOOLS_DEBUG_TREE
#include <cpptools/_internal/debug_macros.hpp>

namespace tools {

/// @brief An arbitrary tree. STL-compatible.
///
/// @tparam T Type of values to be stored
///
/// @note Enable debug assertions with #define CPPTOOLS_DEBUG_TREE 1
template<typename T>
class tree : private detail::tree::unsafe_tree<T> {
/// @note On debug assertions:
/// Some assertions enforce that compound statements hold true, and it can be 
/// hard to make sense of those compound statements. Easier understanding can be
/// achieved by taking short-circuiting into account, and so an assertion that
/// verifies "A || B" can (and should) be read as "if not A, then B".

    using base              = detail::tree::unsafe_tree<T>;

public:
    using value_type        = typename base::value_type;
    using reference         = typename base::reference;
    using const_reference   = typename base::const_reference;

private:
    using node              = base::node;
    static constexpr bool NoExceptErasure = base::NoExceptErasure;

public:
    using size_type         = typename base::size_type;
    using pointer           = typename base::pointer;
    using const_pointer     = typename base::const_pointer;
    using difference_type   = typename base::difference_type;
    using key_equal         = typename base::key_equal;
    using allocator_type    = typename base::allocator_type;

    using const_node_handle = base::const_node_handle;
    using node_handle       = base::node_handle;

    using const_iterator    = base::const_iterator;
    using iterator          = base::iterator;

    using initializer       = base::initializer;

private:
    tree(node* copy_from) :
        base(copy_from)
    {

    }

    // tree(const base& other) :
    //     base(other)
    // {
    // }

    tree(base&& other) :
        base(std::move(other))
    {
    }

public:
    tree() :
        base()
    {
    }

    /// @param other Tree to copy-construct from
    ///
    /// @exception Any exception thrown in a constructor of the value type
    /// will be forwarded to the caller
    tree(const tree& other) :
        base(static_cast<const base&>(other))
    {
    }

    /// @param other Tree to move-construct from
    tree(tree&& other) :
        base(static_cast<base&&>(std::move(other)))
    {
    }

    tree(const initializer& init) :
        base(init)
    {
    }

    /// @param other Tree to copy-assign contents from
    tree& operator=(const tree& other) {
        *this = tree{other};

        return *this;
    }

    /// @param other Tree to move-assign contents from
    tree& operator=(tree&& other) {
        static_cast<base&>(*this) = static_cast<base&&>(std::move(other));

        return *this;
    }

    /// @param init Tree-like initializer list
    tree& operator=(initializer&& init) {
        tree temp(std::move(init));
        *this = std::move(temp);

        return *this;
    }

    /// @brief Get a copy of a subtree
    ///
    /// @param subtree_root Root of the subtree to be copied
    ///
    /// @return A new tree whose root is the copied subtree
    tree copy_subtree(const const_node_handle& subtree_root) const {
        return tree(
            base::copy_subtree(base::node_from(subtree_root))
        );
    }

    /// @brief Detach a subtree
    ///
    /// @param subtree_root Root of the subtree to be detached
    ///
    /// @return A new tree whose root is the detached subtree
    tree chop_subtree(const node_handle& subtree_root) {
        return tree(
            base::chop_subtree(base::node_from(subtree_root))
        );
    }

    /// @brief Acquire the nodes of another tree, making it a subtree of this
    /// tree.
    ///
    /// @param other Tree to steal nodes from
    /// @param destination Node where that should be parent to the stolen nodes
    ///
    /// @return Pointer to the newly adopted subtree
    node_handle adopt_subtree(tree&& other, const node_handle& destination) {
        return base::handle_from(
            base::adopt_subtree(
                static_cast<base&&>(std::move(other)),
                base::node_from(destination)
            )
        );
    }

    node_handle root() {
        return base::handle_from(base::root());
    }

    const_node_handle root() const {
        return base::const_handle_from(base::root());
    }

    /// @brief Move a subtree from somewhere in this tree to somewhere else in
    /// this tree
    ///
    /// @param root The root node of the subtree to move
    /// @param destination The node which the moved subtree should be 
    void move_subtree(const node_handle& subtree_root, const node_handle& destination) {
        base::move_subtree(
            base::node_from(subtree_root),
            base::node_from(destination)
        );
    }

    /// @brief Erase an entire subtree and its values
    ///
    /// @param to_erase The root node of the subtree to erase
    void erase_subtree(const node_handle& subtree_root) {
        base::erase_subtree(
            base::node_from(subtree_root)
        );
    }

    /// @brief Emplace a new value in the tree, as a new child node to the
    /// provided node handle
    ///
    /// @tparam ArgTypes Types of the arguments to be forwarded to a
    /// constructor of the value to emplace
    ///
    /// @param where Handle to the node which is to be the parent of the newly 
    /// emplaced node
    /// @param args Arguments to be forwarded to a constructor of the value to
    /// emplace
    ///
    /// @return A pointer to the newly emplaced child node
    ///
    /// @exception Any exception thrown in the resulting constructor call of
    /// the value type will be let through to the caller
    template<typename ...ArgTypes>
    node_handle emplace_node(const node_handle& where, ArgTypes&&... args) {
        return base::handle_from(
            base::emplace_node(
                base::node_from(where),
                std::forward<ArgTypes&&>(args)...
            )
        );
    }

    /// @brief Merge this node into its parent node:
    /// - the value of this node is merged into the value of the parent node
    /// using the merging strategy provided as a template parameter
    /// - the children of this node are all inserted in-between this node's
    /// left and right siblings, their relative order is preserved, and
    /// their new parent is this node's parent
    /// - this node is deleted from the tree, and this node handle immediately
    /// becomes invalid as a result
    ///
    /// @tparam merger_t A function-like type satisfying the
    /// merging_strategy concept for this tree's value type. The default
    /// merging strategy discards this node's value and keeps the parent
    /// node's value.
    template<merging_strategy<T> merge_t = merging_strategies::keep_original>
    void merge_with_parent(const node_handle& n) {
        base::template merge_with_parent<T>(base::node_from(n));
    }

    /// @brief Check that this tree's structure and its ordered values are the
    /// same as that of another tree
    ///
    /// @param other Other tree to compare this tree to
    ///
    /// @return Whether this tree and other are equal
    bool operator==(const tree& other) const {
        return static_cast<const base&>(*this) == static_cast<const base&>(other);
    }

    friend void swap(tree& lhs, tree& rhs) {
        swap(static_cast<base&>(lhs), static_cast<base&>(rhs));
    }

    using base::size;
    using base::max_size;
    using base::empty;
    using base::clear;
    using base::begin;
    using base::end;
    using base::cbegin;
    using base::cend;
};

} // namespace tools

#include <cpptools/_internal/undef_debug_macros.hpp>

//////////////////
////// TODO //////
//////////////////

// - do likeliness annotations really make a difference? if not a ternary would read better in some places
// - custom allocators (and then scary iterators)
// - store values contiguously instead of within a node map. std::hive comes to mind

// TODO: Ctrl+F TODO

#endif//CPPTOOLS__CONTAINER__TREE_HPP