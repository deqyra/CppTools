#ifndef TOOLS__CONTAINER__TREE_HPP
#define TOOLS__CONTAINER__TREE_HPP

#include <algorithm>
#include <utility>

#include "tree/traversal.hpp"
#include "tree/unsafe_tree.hpp"

#include <cpptools/utility/merge_strategy.hpp>

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
    /// @brief Default constructor
    tree() :
        base()
    {
    }

    /// @brief Copy constructor
    ///
    /// @param other Tree to copy-construct from
    tree(const tree& other) :
        base(other)
    {
    }

    /// @brief Move constructor
    ///
    /// @param other Tree to move-construct from
    tree(tree&& other) :
        base(std::move(other))
    {
    }

    /// @brief Tree-like initializer list copy constructor
    ///
    /// @param init Initializer to copy-construct from
    tree(const initializer& init) :
        base(init)
    {
    }

    /// @brief Tree-like initializer list move constructor
    ///
    /// @param init Initializer to move-construct from
    tree(initializer&& init) :
        base(std::move(init))
    {
    }

    /// @brief Subtree copy constructor
    ///
    /// @param subtree_root Handle to the root node of the subtree from which
    /// this tree should be copy-constructed
    explicit tree(const const_node_handle_t& subtree_root) :
        base(subtree_root.ptr())
    {
    }

    /// @brief Copy assignment
    ///
    /// @param other Tree to copy-assign contents from
    tree& operator=(const tree& other) {
        *this = tree(other);

        return *this;
    }

    /// @brief Move assignment
    ///
    /// @param other Tree to move-assign contents from
    tree& operator=(tree&& other) {
        base::operator=(std::move(other));

        return *this;
    }

    /// @brief Tree-like initializer list copy assignment
    ///
    /// @param init Tree-like initializer list to copy-assign contents from
    tree& operator=(const initializer& init) {
        tree temp(init);
        *this = std::move(temp);

        return *this;
    }

    /// @brief Tree-like initializer list move assignment
    ///
    /// @param init Tree-like initializer list to move-assign contents from
    tree& operator=(initializer&& init) {
        tree temp(std::move(init));
        *this = std::move(temp);

        return *this;
    }

    /// @brief Subtree copy assignment
    ///
    /// @param subtree_root Root of the subtree to copy contents from
    tree& operator=(const const_node_handle_t& subtree_root)
    {
        *this = tree(subtree_root);

        return *this;
    }

    /// @copydoc base::chop_subtree
    tree chop_subtree(const node_handle_t& subtree_root) {
        return tree(
            base::chop_subtree(subtree_root.ptr())
        );
    }

    /// @copydoc base::adopt_subtree
    node_handle_t adopt_subtree(const node_handle_t& destination, tree other) {
        return {
            base::adopt_subtree(destination.ptr(), std::move(other))
        };
    }

    /// @copydoc base::root
    node_handle_t root() {
        return { base::root() };
    }

    /// @copydoc base::root
    const_node_handle_t root() const {
        return { base::root() };
    }

    /// @copydoc base::leftmost
    node_handle_t leftmost() {
        return { base::leftmost() };
    }

    /// @copydoc base::leftmost
    const_node_handle_t leftmost() const {
        return { base::leftmost() };
    }

    /// @copydoc base::rightmost
    node_handle_t rightmost() {
        return { base::rightmost() };
    }

    /// @copydoc base::rightmost
    const_node_handle_t rightmost() const {
        return { base::rightmost() };
    }

    /// @copydoc base::move_subtree
    void move_subtree(const node_handle_t& destination, const node_handle_t& subtree_root) {
        base::move_subtree(destination.ptr(), subtree_root.ptr());
    }

    /// @copydoc base::erase_subtree
    void erase_subtree(const node_handle_t& subtree_root) {
        base::erase_subtree(subtree_root.ptr());
    }

    /// @copydoc base::emplace_node
    template<typename ...ArgTypes>
    node_handle_t emplace_node(const node_handle_t& where, ArgTypes&&... args) {
        return {
            base::emplace_node(where.ptr(), std::forward<ArgTypes>(args)...)
        };
    }

    /// @copydoc base::merge_with_parent
    template<merge_strategy<T> merge_t = merge::keep>
    void merge_with_parent(const node_handle_t& n) {
        base::template merge_with_parent<merge_t>(n.ptr());
    }

    /// @copydoc base::operator==
    bool operator==(const tree& other) const {
        return base::operator==(other);
    }

    /// @copydoc base::swap
    friend void swap(tree& lhs, tree& rhs) {
        swap(static_cast<base&>(lhs), static_cast<base&>(rhs));
    }

    /// @copydoc base::size
    using base::size;
    /// @copydoc base::max_size
    using base::max_size;
    /// @copydoc base::empty
    using base::empty;
    /// @copydoc base::clear
    using base::clear;
    /// @copydoc base::begin
    using base::begin;
    /// @copydoc base::end
    using base::end;
    /// @copydoc base::cbegin
    using base::cbegin;
    /// @copydoc base::cend
    using base::cend;

    /// @brief Get a proxy range-like object which implements DFS const
    /// traversal of the tree. To be used with range-based \c for loops.
    template<traversal::order O>
    friend detail::dfs_proxy<T, O, true> dfs(const tree<T>& t) {
        return dfs<O>(static_cast<const base&>(t));
    }

    /// @brief Get a proxy range-like object which implements DFS traversal of
    /// the tree. To be used with range-based \c for loops.
    template<traversal::order O>
    friend detail::dfs_proxy<T, O, false> dfs(tree<T>& t) {
        return dfs<O>(static_cast<base&>(t));
    }

    /// @brief Get a proxy range-like object which implements reverse DFS const
    /// traversal of the tree. To be used with range-based \c for loops.
    template<traversal::order O>
    friend detail::reverse_dfs_proxy<T, O, true> reverse_dfs(const tree<T>& t) {
        return reverse_dfs<O>(static_cast<const base&>(t));
    }

    /// @brief Get a proxy range-like object which implements reverse DFS
    /// traversal of the tree. To be used with range-based \c for loops.
    template<traversal::order O>
    friend detail::reverse_dfs_proxy<T, O, false> reverse_dfs(tree<T>& t) {
        return reverse_dfs<O>(static_cast<base&>(t));
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

#endif//TOOLS__CONTAINER__TREE_HPP