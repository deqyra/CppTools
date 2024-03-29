#ifndef CPPTOOLS__CONTAINER__TREE__UNSAFE_TREE_HPP
#define CPPTOOLS__CONTAINER__TREE__UNSAFE_TREE_HPP

#include <algorithm>
#include <memory>
#include <ranges>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cpptools/exception/exception.hpp>
#include <cpptools/exception/internal_exception.hpp>
#include <cpptools/exception/parameter_exception.hpp>
#include <cpptools/exception/iterator_exception.hpp>
#include <cpptools/utility/merging_strategy.hpp>
#include <cpptools/utility/type_utils.hpp>

#include "node.hpp"

#ifndef CPPTOOLS_DEBUG_UNSAFE_TREE
# if CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH != 0
#   define CPPTOOLS_DEBUG_UNSAFE_TREE 1
# else
#   define CPPTOOLS_DEBUG_UNSAFE_TREE 0
# endif
#endif

#define CPPTOOLS_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE
#define CPPTOOLS_LOCAL_DEBUG_MACRO CPPTOOLS_DEBUG_UNSAFE_TREE
#include <cpptools/_internal/debug_macros.hpp>

namespace tools::detail
{

/// @brief Unsafe arbitrary tree.
///
/// @tparam T Type of values to be stored
///
/// @note Enable debug assertions with #define CPPTOOLS_DEBUG_TREE 1
template<typename T>
class unsafe_tree {
protected:
    using value_type        = T;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using node_t            = node<value_type>;
    using node_ptr          = std::unique_ptr<node_t>;
    using key_type          = const node_t*;

    using storage_t         = std::unordered_map<key_type, node_ptr>;

    using size_type         = typename storage_t::size_type;
    using pointer           = typename storage_t::pointer;
    using const_pointer     = typename storage_t::const_pointer;
    using difference_type   = typename storage_t::difference_type;
    using key_equal         = typename storage_t::key_equal;
    using allocator_type    = typename storage_t::allocator_type;

    static constexpr bool NoExceptErasure = noexcept(std::declval<storage_t>().erase(std::declval<key_type>()));

private:
    static const_reference _value_const_ref_from_storage_value(const typename storage_t::value_type& pair) {
        return (pair.second)->value;
    }

    static reference _value_ref_from_storage_value(typename storage_t::value_type& pair) {
        return (pair.second)->value;
    }

protected:
    using const_value_view_t = decltype(std::declval<const storage_t&>() | std::views::transform(_value_const_ref_from_storage_value));
    using       value_view_t = decltype(std::declval<      storage_t&>() | std::views::transform(_value_ref_from_storage_value));

    const const_value_view_t& const_values() const {
        return _const_values;
    }

    value_view_t& values() {
        return _values;
    }

    using const_iterator = decltype(std::declval<const const_value_view_t&>().begin());
    using       iterator = decltype(std::declval<            value_view_t&>().begin());

    struct initializer {
        T value;
        std::vector<initializer> child_initializers;

        initializer(const T& value, std::vector<initializer> child_init = {}) :
            value(value),
            child_initializers(std::move(child_init))
        {
        }
    };

private:
    /// @brief Structure mapping node pointers to their node IDs
    storage_t _nodes;

    /// @brief Pointer to the root node of the tree
    node_t* _root;

    /// @brief Pointer to the leftmost node of the tree
    node_t* _leftmost;

    /// @brief Pointer to the rightmost node of the tree
    node_t* _rightmost;

    /// @brief View over the values of the nodes
    const_value_view_t _const_values;

    /// @brief View over the const values of the nodes
    value_view_t _values;

    void _copy_fill_from_init(node_t* dest, const std::vector<initializer>& child_initializers) {
        for (const auto& init : child_initializers) {
            auto new_node = emplace_node(dest, init.value);

            if (init.child_initializers.size() != 0) {
                _copy_fill_from_init(new_node, init.child_initializers);
            }
        }
    }

    void _move_fill_from_init(node_t* dest, std::vector<initializer>& child_initializers) {
        for (auto& init : child_initializers) {
            auto new_node = emplace_node(dest, std::move(init.value));

            if (init.child_initializers.size() != 0) {
                _move_fill_from_init(new_node, init.child_initializers);
            }
        }
    }

    /// @brief Transfer ownership of nodes of a subtree from this tree's storage
    /// to another storage
    ///
    /// @param branch_root Root of the subtree whose nodes should be transferred
    /// @param storage Storage where the nodes should be transferred to
    void _move_node_ptrs_recursive(node_t* subtree_root, storage_t& storage) {
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, subtree_root), "unsafe_tree", critical, "cannot move nodes of null subtree", exception::parameter::null_parameter_error, "subtree_root", subtree_root);

        auto map_node = _nodes.extract(subtree_root);
        storage.insert(std::move(map_node));

        for (auto child : subtree_root->children()) {
            _move_node_ptrs_recursive(child, storage);
        }
    }

    /// @brief Given a source root node and a destination root node, recursively
    /// copy all source children nodes as children of the destination node
    ///
    /// @param from Node whose chidlren should be copied
    /// @param to Node which should adopt the copied nodes
    void _copy_children_recursive(const node_t* from, node_t* to) {
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, to), "unsafe_tree", critical, "destination not in tree",               exception::parameter::invalid_value_error, "to", to);
        CPPTOOLS_DEBUG_ASSERT(!to->has_parent(from),     "unsafe_tree", critical, "destination is part of copied subtree", exception::parameter::invalid_value_error, "to", to);

        for (auto child : from->children()) {
            // make new child node_t holding a copy of the source value
            node_t* copied = make_node(child->value);

            // insert copied child into destination
            to->insert_child(copied);

            // recurse copy
            if (child->child_count() > 0) {
                _copy_children_recursive(child, copied);
            }
        }
    }

    /// @brief Delete an entire subtree from the node storage, including the
    /// values held by its nodes.
    ///
    /// @param branch_root Root of the subtree whose nodes must be deleted
    void _delete_subtree_nodes(node_t* subtree_root) CPPTOOLS_NOEXCEPT_RELEASE_AND(NoExceptErasure) {
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, subtree_root), "unsafe_tree", critical, "subtree root not in tree", exception::parameter::invalid_value_error, "subtree_root", subtree_root);

        for (auto child : subtree_root->release_children()) {
            _delete_subtree_nodes(child);
        }

        _delete_node(subtree_root);
    }

    /// @brief Delete a node and the value it holds
    ///
    /// @param n Node to be deleted
    void _delete_node(node_t* n) CPPTOOLS_NOEXCEPT_RELEASE_AND(NoExceptErasure) {
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, n), "unsafe_tree", critical, "node_t not in tree",                           exception::parameter::invalid_value_error,"n", n);
        CPPTOOLS_DEBUG_ASSERT(n->children().empty(),    "unsafe_tree", critical, "node_t to be deleted shall not have children", exception::parameter::invalid_value_error,"n", n);

        _nodes.erase(n);
    }

    /// @brief Compare whether two subtrees are equal both in structure and value
    ///
    /// @param left First subtree to compare
    /// @param right Second subtree to compare
    ///
    /// @return Whether the two subtrees are equal both in structure and value
    static bool _subtrees_equal(node_t* left, node_t* right) {
        CPPTOOLS_DEBUG_ASSERT(both_not_null(left, right), "unsafe_tree", critical, "cannot compare null subtrees",                    exception::parameter::null_parameter_error, "left or right");
        CPPTOOLS_DEBUG_ASSERT(!left->has_parent(right),   "unsafe_tree", critical, "compared subtrees shall not contain one another", exception::parameter::invalid_value_error,  "left", left);
        CPPTOOLS_DEBUG_ASSERT(!right->has_parent(left),   "unsafe_tree", critical, "compared subtrees shall not contain one another", exception::parameter::invalid_value_error,  "right", right);

        using node_pair = std::pair<node_t*, node_t*>;

        // This function performs a DFS traversal of both trees and compares
        // both structure and value along the way

        // iterative version: use a stack
        std::stack<node_pair, std::vector<node_pair>> node_stack;
        auto this_node = left;
        auto other_node = right;

        do {
            // compare structure
            if (this_node->child_count() != other_node->child_count()) return false;

            // compare value
            if (this_node->value != other_node->value) return false;

            // find next node
            if (this_node->child_count() != 0) {
                // visit leftmost children first
                node_stack.push({this_node, other_node});
                this_node  =  this_node->children()[0];
                other_node = other_node->children()[0];
            } else {
                // unstack to a non-rightmost node
                while (!node_stack.empty() && this_node->parent() != nullptr && this_node->is_rightmost_sibling()) {
                    std::tie(this_node, other_node) = node_stack.top();
                    node_stack.pop();
                }
                // stack exhausted == tree fully visited
                // stack not exhausted (== non-null parents) == more right branches to visit
                if (!node_stack.empty()) {
                    // visit right siblings
                    this_node = this_node->right_sibling();
                    other_node = other_node->right_sibling();
                }
            }
        } while (!node_stack.empty());

        return true;
    }

    const_value_view_t _make_const_value_view() {
        return static_cast<const storage_t&>(_nodes) | std::views::transform(_value_const_ref_from_storage_value);
    }

    value_view_t _make_value_view() {
        return static_cast<storage_t&>(_nodes) | std::views::transform(_value_ref_from_storage_value);
    }

    /// @brief Construct a tree from a detached subtree
    ///
    /// @param chopped_nodes Storage to steal nodes from
    /// @param chopped_root Root node of this tree
    /// @param chopped_leftmost Leftmost node of this tree
    /// @param chopped_rightmost Rightmost node of this tree
    unsafe_tree(storage_t&& chopped_nodes, node_t* chopped_root, node_t* chopped_leftmost, node_t* chopped_rightmost) :
        _nodes(std::move(chopped_nodes)),
        _root(chopped_root),
        _leftmost(chopped_leftmost),
        _rightmost(chopped_rightmost),
        _const_values(_make_const_value_view()),
        _values(_make_value_view())
    {
        _root->clear_parent_metadata();
        _root->propagate_parent_chain_update();
    }

protected:
    unsafe_tree() :
        _nodes(),
        _root(nullptr),
        _leftmost(nullptr),
        _rightmost(nullptr),
        _const_values(_make_const_value_view()),
        _values(_make_value_view())
    {

    }

    /// @param copy_from Pointer to the root node of a branch to copy contents
    /// from
    ///
    /// @exception Any exception thrown in a constructor of the value type
    /// will be forwarded to the caller
    unsafe_tree(const node_t* copy_from) :
        unsafe_tree()
    {
        if (copy_from != nullptr) {
            _root = make_node(copy_from->value);
            if (copy_from->child_count() > 0) {
                _copy_children_recursive(copy_from, _root);
            }

            _leftmost = _root->leftmost_child_or_this();
            _rightmost = _root->rightmost_child_or_this();
        }
    }

    /// @param other Tree to copy-construct from
    ///
    /// @exception Any exception thrown in a constructor of the value type
    /// will be forwarded to the caller
    unsafe_tree(const unsafe_tree& other) :
        unsafe_tree(other._root)
    {
    }

    /// @param other Tree to move-construct from
    unsafe_tree(unsafe_tree&& other) :
        _nodes(std::move(other._nodes)),
        _root(other._root),
        _leftmost(other._leftmost),
        _rightmost(other._rightmost),
        _const_values(_make_const_value_view()),
        _values(_make_value_view())
    {
        other._root = nullptr;
        other._leftmost = nullptr;
        other._rightmost = nullptr;
    }

    unsafe_tree(const initializer& init) :
        unsafe_tree()
    {
        auto emplaced_root = emplace_node(root(), init.value);
        _copy_fill_from_init(emplaced_root, init.child_initializers);
    }

    unsafe_tree(initializer&& init) :
        unsafe_tree()
    {
        auto emplaced_root = emplace_node(root(), std::move(init.value));
        _move_fill_from_init(emplaced_root, init.child_initializers);
    }

    /// @param other Tree to copy-assign contents from
    unsafe_tree& operator=(const unsafe_tree& other) {
        *this = unsafe_tree{other};

        return *this;
    }

    /// @param other Tree to move-assign contents from
    unsafe_tree& operator=(unsafe_tree&& other) {
        swap(*this, other);
        other.clear();

        return *this;
    }

    /// @param Init Tree-like initializer list
    unsafe_tree& operator=(initializer&& init) {
        unsafe_tree temp(std::move(init));
        *this = std::move(temp);

        return *this;
    }

    /// @brief Create and register a new node out of a value, and get a pointer
    /// to it
    ///
    /// @tparam ArgTypes Types of the arguments to be forwarded to the 
    /// constructor of the new value
    ///
    /// @param args Values of the arguments to be forwarded to the constructor
    /// of the new value
    ///
    /// @return A pointer to the newly created node
    template<typename... ArgTypes>
    [[nodiscard]] node_t* make_node(ArgTypes&&... args) {
        auto new_node = std::make_unique<node_t>(std::forward<ArgTypes>(args)...);
        auto raw_ptr = new_node.get();

        _nodes[raw_ptr] = std::move(new_node);
        return raw_ptr;
    }

    /// @brief Tells whether emplacing a node somewhere would change the 
    /// leftmost node of the tree
    ///
    /// @param dest_node Pointer to the node where the supposed emplacement
    /// would take place; can be null to indicate root emplacement
    ///
    /// @return Whether emplacing a node as a new child to \c dest_node would 
    /// change the leftmost node of the tree
    bool emplacing_there_would_change_leftmost(node_t* where) const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(both_null(where, _root) || in_range_keys(_nodes, where), "unsafe_tree", critical, "destination not in tree", exception::parameter::invalid_value_error, "where", where);

        return where == _leftmost;
    }

    /// @brief Tells whether emplacing a node somewhere would change the 
    /// rightmost node of the tree
    ///
    /// @param dest_node Pointer to the node where the supposed emplacement
    /// would take place
    ///
    /// @return Whether emplacing a node as a new child to \c dest_node would 
    /// change the rightmost node of the tree
    bool emplacing_there_would_change_rightmost(node_t* where) const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(both_null(where, _root) || in_range_keys(_nodes, where), "unsafe_tree", critical, "destination not in tree", exception::parameter::invalid_value_error, "where", where);

        return where == _rightmost
            || _rightmost->has_parent(where);
    }

    /// @brief Detach a subtree
    ///
    /// @param subtree_root Root of the subtree to be detached
    ///
    /// @return A new tree whose root is the detached subtree
    unsafe_tree chop_subtree(node_t* subtree_root) {
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, subtree_root), "unsafe_tree", critical, "subtree root not in tree", exception::parameter::invalid_value_error, "subtree_root", subtree_root);

        if (subtree_root == _root) {
            return unsafe_tree(std::move(*this));
        }

        const bool dropping_leftmost  = (subtree_root == _leftmost)  || (_leftmost->has_parent(subtree_root));
        const bool dropping_rightmost = (subtree_root == _rightmost) || (_rightmost->has_parent(subtree_root));

        node_t* chopped_leftmost = dropping_leftmost
            ? _leftmost
            : subtree_root->leftmost_child_or_this();

        node_t* chopped_rightmost = dropping_rightmost
            ? _rightmost
            : subtree_root->rightmost_child_or_this();

        storage_t intermediate_storage;

        node_t* parent = subtree_root->parent(); // not null since root case was taken care of already
        _move_node_ptrs_recursive(subtree_root, intermediate_storage);

        parent->remove_child(subtree_root->sibling_index());

        if (dropping_leftmost) {
            _leftmost = parent;
        }
        if (dropping_rightmost) {
            _rightmost = parent;
        }

        return unsafe_tree(std::move(intermediate_storage), subtree_root, chopped_leftmost, chopped_rightmost);
    }

    /// @brief Acquire the nodes of another tree, making it a subtree of this
    /// tree.
    ///
    /// @param destination Node where that should be parent to the stolen nodes
    /// @param other Tree to steal nodes from
    ///
    /// @return Pointer to the newly adopted subtree
    node_t* adopt_subtree(node_t* destination, unsafe_tree&& other) {
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, destination), "unsafe_tree", critical, "destination not in tree", exception::parameter::invalid_value_error, "destination", destination);

        bool updating_leftmost  = emplacing_there_would_change_leftmost(destination);
        bool updating_rightmost = emplacing_there_would_change_rightmost(destination);

        node_t* new_subtree = other._root;
        // steal node ownership
        other._move_node_ptrs_recursive(new_subtree, _nodes);
        // attach subtree
        destination->insert_child(new_subtree);

        if (updating_leftmost)  { _leftmost  = other._leftmost; }
        if (updating_rightmost) { _rightmost = other._rightmost; }

        other.clear();

        return new_subtree;
    }

    node_t* root() {
        return _root;
    }

    const node_t* root() const {
        return _root;
    }

    /// @brief Move a subtree from somewhere in this tree to somewhere else in
    /// this tree
    ///
    /// @param destination The node which the moved subtree should be 
    /// @param root The root node of the subtree to move
    void move_subtree(node_t* destination, node_t* subtree_root) {
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, subtree_root),    "unsafe_tree", critical, "subtree root not in tree",          exception::parameter::invalid_value_error, "subtree_root", subtree_root);
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, destination),     "unsafe_tree", critical, "destination not in tree",           exception::parameter::invalid_value_error, "destination", destination);
        CPPTOOLS_DEBUG_ASSERT(subtree_root != _root,                  "unsafe_tree", critical, "cannot move the root of the tree",  exception::parameter::invalid_value_error, "subtree_root", subtree_root);
        CPPTOOLS_DEBUG_ASSERT(!destination->has_parent(subtree_root), "unsafe_tree", critical, "destination is part of moved tree", exception::parameter::invalid_value_error, "destination", destination);

        bool dropping_leftmost  = (subtree_root == _leftmost)  || (_leftmost->has_parent(subtree_root));
        bool dropping_rightmost = (subtree_root == _rightmost) || (_rightmost->has_parent(subtree_root));

        bool updating_leftmost  = emplacing_there_would_change_leftmost(destination);
        bool updating_rightmost = emplacing_there_would_change_rightmost(destination);

        node_t* parent = subtree_root->parent();
        parent->remove_child(subtree_root->sibling_index());
        destination->insert_child(subtree_root);

        // It can be the case that (dropping_rightmost && update_rightmost) == true,
        // when a subtree which includes the rightmost node_t is moved somewhere
        // further up the rightmost branch. In this case, the rightmost node_t of
        // the tree does not change.
        // Due to the tree semantics as implemented at the time of writing this
        // comment, this situation currently cannot happen for the leftmost node_t
        // of the tree. Similar handling is still performed as a form of
        // future-proofing.

        if (dropping_leftmost  && !updating_leftmost)  { _leftmost  = parent; }
        if (dropping_rightmost && !updating_rightmost) { _rightmost = parent; }

        if (updating_leftmost  && !dropping_leftmost)  { _leftmost  = destination->leftmost_child_or_this(); }
        if (updating_rightmost && !dropping_rightmost) { _rightmost = destination->rightmost_child_or_this(); }
    }

    /// @brief Erase an entire subtree and its values
    ///
    /// @param to_erase The root node of the subtree to erase
    void erase_subtree(node_t* subtree_root) {
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, subtree_root), "unsafe_tree", critical, "subtree root not in tree", exception::parameter::invalid_value_error, "subtree_root", subtree_root);

        if (subtree_root == _root) {
            clear();
            return;
        }

        bool dropping_leftmost  = (subtree_root == _leftmost)  || (_leftmost->has_parent(subtree_root));
        bool dropping_rightmost = (subtree_root == _rightmost) || (_rightmost->has_parent(subtree_root));

        // the actual node_t can be deleted afterwards
        node_t* parent = subtree_root->parent();
        if (parent) {
            parent->remove_child(subtree_root->sibling_index());
        } 

        _delete_subtree_nodes(subtree_root);

        if (dropping_leftmost)  _leftmost  = parent;
        if (dropping_rightmost) _rightmost = parent;
    }

    /// @brief Emplace a new value in the tree, as a new child node to the
    /// provided node
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
    node_t* emplace_node(node_t* where, ArgTypes&&... args) {
        CPPTOOLS_DEBUG_ASSERT(null(where) || in_range_keys(_nodes, where), "unsafe_tree", critical, "destination not in tree", exception::parameter::invalid_value_error, "where", where);

        node_t* child = make_node(std::forward<ArgTypes>(args)...);

        if (where == nullptr) [[unlikely]] {
            // insert tree root
            node_t* old_root = _root;
            _root = child;
            if (old_root != nullptr) {
                child->insert_child(old_root);
            } else {
                _leftmost = child;
                _rightmost = child;
            }
        } else {
            bool update_leftmost  = emplacing_there_would_change_leftmost(where);
            bool update_rightmost = emplacing_there_would_change_rightmost(where);

            where->insert_child(child);

            if (update_leftmost)  _leftmost  = child;
            if (update_rightmost) _rightmost = child;
        }

        return child;
    }

    /// @brief Merge this node into its parent node:
    /// - the value of this node is merged into the value of the parent node
    /// using the merging strategy provided as a template parameter
    /// - the children of this node are all inserted in-between this node's
    /// left and right siblings, their relative order is preserved, and
    /// their new parent is this node's parent
    /// - this node is deleted from the tree
    ///
    /// @tparam merger_t A function-like type satisfying the
    /// merging_strategy concept for this tree's value type. The default
    /// merging strategy discards this node's value and keeps the parent
    /// node's value.
    template<merging_strategy<T> merge_t = merge::keep_original>
    void merge_with_parent(node_t* n) {
        CPPTOOLS_DEBUG_ASSERT(not_null(n),              "unsafe_tree", critical, "cannot merge node_t with parent using an iterator pointing at no node_t", exception::internal::precondition_failure_error);
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, n), "unsafe_tree", critical, "node_t not in tree",                                                    exception::parameter::invalid_value_error, "n", n);

        node_t* parent = n->parent();

        CPPTOOLS_DEBUG_ASSERT(not_null(parent), "unsafe_tree", critical, "cannot merge node_t with null parent",                                          exception::internal::precondition_failure_error);

        parent->template merge_child<merge_t>(n->sibling_index());

        _delete_node(n);
    }

    /// @brief Check that this tree's structure and its ordered values are the
    /// same as that of another tree
    ///
    /// @param other Other tree to compare this tree to
    ///
    /// @return Whether this tree and other are equal
    bool operator==(const unsafe_tree& other) const {
        if (&other == this) {
            return true;
        }

        if (size() != other.size()) {
            return false;
        }

        // at this point, both trees have the same size
        if (empty() /* && other.empty()*/) {
            return true;
        }

        // at this point, both trees have equal size and are non-empty
        return _subtrees_equal(_root, other._root);
    }

    friend void swap(unsafe_tree& lhs, unsafe_tree& rhs) {
        std::swap(lhs._nodes, rhs._nodes);
        std::swap(lhs._root, rhs._root);
        std::swap(lhs._leftmost, rhs._leftmost);
        std::swap(lhs._rightmost, rhs._rightmost);
    }

    size_type size() const {
        return _nodes.size();
    }

    size_type max_size() const {
        return _nodes.max_size();
    }

    bool empty() const {
        return _nodes.empty();
    }

    void clear() {
        _nodes.clear();
        _root = nullptr;
        _leftmost = nullptr;
        _rightmost = nullptr;
    }

    iterator begin() {
        return values().begin();
    }

    iterator end() {
        return values().end();
    }

    const_iterator begin() const {
        return const_values().begin();
    }

    const_iterator end() const {
        return const_values().end();
    }

    const_iterator cbegin() const {
        return const_values().begin();
    }

    const_iterator cend() const {
        return const_values().end();
    }
};

} // namespace tools::detail

#include <cpptools/_internal/undef_debug_macros.hpp>

#endif//CPPTOOLS__CONTAINER__TREE__UNSAFE_TREE_HPP

//////////
// TODO //
//////////

/*
 - figure out better access modifiers for node
 - profile different impls for _subtrees_equal:
    - compare structure when stacking, and then values only when unstacking
    - recursive version
 */