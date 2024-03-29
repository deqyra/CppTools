#ifndef CPPTOOLS__CONTAINER__TREE_HPP
#define CPPTOOLS__CONTAINER__TREE_HPP

#include <algorithm>
#include <utility>

#include "tree/traversal.hpp"
#include "tree/unsafe_tree.hpp"

#include <cpptools/utility/merging_strategy.hpp>
#include <cpptools/utility/type_utils.hpp>

#ifndef CPPTOOLS_DEBUG_TREE
# define CPPTOOLS_DEBUG_TREE CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH
#endif

#define CPPTOOLS_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE
#define CPPTOOLS_LOCAL_DEBUG_MACRO CPPTOOLS_DEBUG_TREE
#include <cpptools/_internal/debug_macros.hpp>

namespace tools {

/// @brief An arbitrary tree. STL-compatible.
///
/// @tparam T Type of values to be stored
template<typename T>
class tree : private detail::unsafe_tree<T> {
    using base                = detail::unsafe_tree<T>;
    using node_t              = base::node_t;

    static constexpr bool NoExceptErasure = base::NoExceptErasure;

public:
    using value_type          = typename base::value_type;
    using reference           = typename base::reference;
    using const_reference     = typename base::const_reference;
    using size_type           = typename base::size_type;
    using pointer             = typename base::pointer;
    using const_pointer       = typename base::const_pointer;
    using difference_type     = typename base::difference_type;
    using key_equal           = typename base::key_equal;
    using allocator_type      = typename base::allocator_type;

    using const_node_handle_t = const_node_handle<T>;
    using node_handle_t       = node_handle<T>;

    using const_iterator      = typename base::const_iterator;
    using iterator            = typename base::iterator;

    using initializer         = typename base::initializer;

private:
    tree(const node_t* copy_from) :
        base(copy_from)
    {
    }

    tree(const base& other) :
        base(other)
    {
    }

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
    tree(const tree& other) :
        base(other)
    {
    }

    /// @param other Tree to move-construct from
    tree(tree&& other) :
        base(std::move(other))
    {
    }

    tree(const initializer& init) :
        base(init)
    {
    }

    explicit tree(const const_node_handle_t& subtree_root) :
        base(subtree_root.ptr())
    {
    }

    /// @param other Tree to copy-assign contents from
    tree& operator=(const tree& other) {
        *this = tree(other);

        return *this;
    }

    /// @param other Tree to move-assign contents from
    tree& operator=(tree&& other) {
        base::operator=(std::move(other));

        return *this;
    }

    /// @param init Tree-like initializer list
    tree& operator=(initializer&& init) {
        tree temp(std::move(init));
        *this = std::move(temp);

        return *this;
    }

    /// @param subtree_root Root of the subtree to copy contents from
    tree& operator=(const const_node_handle_t& subtree_root)
    {
        *this = tree(subtree_root);

        return *this;
    }

    /// @brief Detach a subtree
    ///
    /// @param subtree_root Root of the subtree to be detached
    ///
    /// @return A new tree whose root is the detached subtree
    tree chop_subtree(const node_handle_t& subtree_root) {
        return tree(
            base::chop_subtree(subtree_root.ptr())
        );
    }

    /// @brief Acquire the nodes of another tree, making it a subtree of this
    /// tree.
    ///
    /// @param destination Node where that should be parent to the stolen nodes
    /// @param other Tree to steal nodes from
    ///
    /// @return Handle to the newly adopted subtree
    node_handle_t adopt_subtree(const node_handle_t& destination, tree&& other) {
        return {
            base::adopt_subtree(destination.ptr(), std::move(other))
        };
    }

    node_handle_t root() {
        return { base::root() };
    }

    const_node_handle_t root() const {
        return { base::root() };
    }

    /// @brief Move a subtree from somewhere in this tree to somewhere else in
    /// this tree
    ///
    /// @param destination The node which the moved subtree should be 
    /// @param root The root node of the subtree to move
    void move_subtree(const node_handle_t& destination, const node_handle_t& subtree_root) {
        base::move_subtree(subtree_root.ptr(), destination.ptr());
    }

    /// @brief Erase an entire subtree and its values
    ///
    /// @param to_erase The root node of the subtree to erase
    void erase_subtree(const node_handle_t& subtree_root) {
        base::erase_subtree(subtree_root.ptr());
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
    node_handle_t emplace_node(const node_handle_t& where, ArgTypes&&... args) {
        return {
            base::emplace_node(where.ptr(), std::forward<ArgTypes>(args)...)
        };
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
    template<merging_strategy<T> merge_t = merge::keep_original>
    void merge_with_parent(const node_handle_t& n) {
        base::template merge_with_parent<T>(n.ptr());
    }

    /// @brief Check that this tree's structure and its ordered values are the
    /// same as that of another tree
    ///
    /// @param other Other tree to compare this tree to
    ///
    /// @return Whether this tree and other are equal
    bool operator==(const tree& other) const {
        return base::operator==(other);
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

    template<traversal::order O>
    friend detail::dfs_proxy<T, O, true> dfs(const tree<T>& t) {
        return dfs(static_cast<const base&>(t));
    }

    template<traversal::order O>
    friend detail::dfs_proxy<T, O, false> dfs(tree<T>& t) {
        return dfs(static_cast<base&>(t));
    }
};

} // namespace tools

#include <cpptools/_internal/undef_debug_macros.hpp>

//////////////////
////// TODO //////
//////////////////

// - do likeliness annotations really make a difference?
// - custom allocators (and then scary iterators)
// - store values contiguously instead of within a node map. std::hive comes to mind

// TODO: Ctrl+F TODO

#endif//CPPTOOLS__CONTAINER__TREE_HPP