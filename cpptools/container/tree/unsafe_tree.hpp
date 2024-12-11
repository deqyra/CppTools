#ifndef CPPTOOLS_CONTAINER_TREE_UNSAFE_TREE_HPP
#define CPPTOOLS_CONTAINER_TREE_UNSAFE_TREE_HPP

#include <algorithm>
#include <memory>
#include <ranges>
#include <stack>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

#include <cpptools/exception/exception.hpp>
#include <cpptools/exception/internal_exception.hpp>
#include <cpptools/exception/parameter_exception.hpp>
#include <cpptools/exception/iterator_exception.hpp>
#include <cpptools/utility/attributes.hpp>
#include <cpptools/utility/heterogenous_lookup.hpp>
#include <cpptools/utility/merge_strategy.hpp>
#include <cpptools/utility/detail/allocator.hpp>

#include "node.hpp"

#ifndef CPPTOOLS_DEBUG_UNSAFE_TREE
# define CPPTOOLS_DEBUG_UNSAFE_TREE CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH
#endif

#define CPPTOOLS_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE
#define CPPTOOLS_LOCAL_DEBUG_MACRO CPPTOOLS_DEBUG_UNSAFE_TREE
#include <cpptools/_internal/debug_macros.hpp>

namespace tools::detail
{

/// @brief Unsafe arbitrary tree.
/// @tparam T Type of values to be stored
/// @note Enable debug assertions with #define CPPTOOLS_DEBUG_TREE 1
template<typename T, typename A = std::allocator<T>>
class unsafe_tree {
public:
    using value_type        = T;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using allocator_type    = A;
    using node_t            = node<value_type, allocator_type>;

private:
    using _key_type         = node_t*;
    using _al_node          = rebind_alloc_t<A, node_t>;
    using _al_traits        = std::allocator_traits<_al_node>;

    struct _deleter_t {
        /// @brief Allocator
        NO_UNIQUE_ADDR mutable _al_node _alloc;

        void operator()(node_t* node) const {
            _al_traits::destroy(_alloc, node);
            _al_traits::deallocate(_alloc, node, 1);
        }

        _deleter_t() = default;
        _deleter_t(const _deleter_t &) = default;
        _deleter_t(_deleter_t &&) = default;
    };

    using _node_ptr         = std::unique_ptr<node_t, _deleter_t&>;

    static constexpr bool _pocca = _al_traits::propagate_on_container_copy_assignment::value;
    static constexpr bool _pocma = _al_traits::propagate_on_container_move_assignment::value;
    static constexpr bool _pocs  = _al_traits::propagate_on_container_swap::value;

public: 
    using storage_t = std::unordered_set<
        _node_ptr,
        unique_ptr_transparent_hash_t<_node_ptr>,
        unique_ptr_transparent_equality_t<_node_ptr>,
        rebind_alloc_t<_al_node, _node_ptr>
    >;

    using size_type         = typename _al_traits::size_type;
    using pointer           = typename _al_traits::pointer;
    using const_pointer     = typename _al_traits::const_pointer;
    using difference_type   = typename _al_traits::difference_type;

protected:
    static constexpr bool NoExceptErasure = noexcept(std::declval<storage_t>().erase(std::declval<_key_type>()));
    static constexpr bool NoExceptSwap    = noexcept(std::swap(std::declval<storage_t&>(), std::declval<storage_t&>()));

private:
    static const_reference _value_const_ref_from_storage_value(const typename storage_t::value_type& ptr) {
        return ptr->value;
    }

    static reference _value_ref_from_storage_value(const typename storage_t::value_type& ptr) {
        return ptr->value;
    }

 public:
    using const_value_view_t = decltype(std::declval<const storage_t&>() | std::views::transform(_value_const_ref_from_storage_value));
    using       value_view_t = decltype(std::declval<      storage_t&>() | std::views::transform(_value_ref_from_storage_value));

    const const_value_view_t& const_values() const {
        return _const_values;
    }

    value_view_t& values() {
        return _values;
    }

    const_value_view_t _make_const_value_view() {
        return static_cast<const storage_t&>(_nodes) | std::views::transform(_value_const_ref_from_storage_value);
    }

    value_view_t _make_value_view() {
        return static_cast<      storage_t&>(_nodes) | std::views::transform(_value_ref_from_storage_value);
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

    NO_UNIQUE_ADDR _al_node _alloc;

    /// @brief Deleter
    _deleter_t _deleter;

    void _copy_fill_from_init(node_t* dest, const std::vector<initializer>& child_initializers) {
        for (const auto& init : child_initializers) {
            auto new_node = emplace_node(dest, init.value);

            if (init.child_initializers.size() != 0) {
                _copy_fill_from_init(new_node, init.child_initializers);
            }
        }
    }

    void _move_fill_from_init(node_t* dest, std::vector<initializer>&& child_initializers) {
        for (auto& init : child_initializers) {
            auto new_node = emplace_node(dest, std::move(init.value));

            if (init.child_initializers.size() != 0) {
                _move_fill_from_init(new_node, std::move(init.child_initializers));
            }
        }
    }

    /// @brief Transfer ownership of nodes of a subtree from this tree's storage
    /// to another storage
    /// @param subtree_root Root of the subtree whose nodes should be transferred
    /// @param storage Storage where the nodes should be transferred to
    void _move_node_ptrs_recursive(node_t* subtree_root, storage_t& storage) {
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, subtree_root), "unsafe_tree", critical, "cannot move nodes not part of tree", exception::parameter::invalid_value_error, "subtree_root", subtree_root);

        auto map_node = _nodes.extract(subtree_root);
        storage.insert(std::move(map_node));

        for (auto child : subtree_root->children()) {
            _move_node_ptrs_recursive(child, storage);
        }
    }

    /// @brief Given a source root node and a destination root node, recursively
    /// copy all source children nodes as children of the destination node
    /// @param from Node whose chidlren should be copied
    /// @param to Node which should adopt the copied nodes
    void _copy_children_recursive(const node_t* from, node_t* to) {
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, to), "unsafe_tree", critical, "destination not in tree",               exception::parameter::invalid_value_error, "to", to);
        CPPTOOLS_DEBUG_ASSERT(!to->has_parent(from),     "unsafe_tree", critical, "destination is part of copied subtree", exception::parameter::invalid_value_error, "to", to);

        for (auto child : from->children()) {
            // make new child node holding a copy of the source value
            node_t* copied = make_node(child->value);

            // insert copied child into destination
            to->insert_child(copied);

            // recurse copy
            if (child->child_count() > 0) {
                _copy_children_recursive(child, copied);
            }
        }
    }

    /// @brief Given another tree, use its nodes to copy-construct this tree's
    /// nodes, replicating its structure
    void _copy_assign_contents(const node_t *copy_from) {
        clear();

        if (copy_from != nullptr) {
            _root = make_node(copy_from->value);
            if (copy_from->child_count() > 0) {
                _copy_children_recursive(copy_from, _root);
            }

            _leftmost = _root->leftmost_child_or_this();
            _rightmost = _root->rightmost_child_or_this();
        }
    }

    /// @brief Given another tree, use its nodes to move-construct this tree's
    /// nodes, replicating its structure
    /// @note This function does not steal the node storage but copies and 
    /// recreates the node structure of the other tree by move-constructing 
    /// nodes. This is useful when the tree being moved-from uses a different
    /// allocator.
    void _move_assign_contents(unsafe_tree &&other) {
        using node_pair = std::pair<node_t *, node_t *>;

        // move-construct reallocated root node
        node_t* this_node = make_node(std::move(other._root->value));
        node_t* other_node = other._root;

        _root = this_node;

        auto root_child_count = other_node->child_count();
        if (root_child_count == 0) {
            _leftmost  = _root;
            _rightmost = _root;

            other._nodes.clear();
            return;
        }

        // iterative pre-order DFS traversal to move-construct reallocated nodes
        // and replicate node structure
        this_node->reserve(other_node->child_count());

        std::stack<node_pair, std::vector<node_pair>> node_stack;
        node_stack.push({this_node, other_node});

        node_t* parent_node = this_node;
        other_node = other_node->child(0);
        do {
            // move-construct node
            this_node = make_node(std::move(other_node->value));
            this_node->reserve(other_node->child_count());

            parent_node->insert_child(this_node);
            parent_node = this_node;

            // find next node
            if (other_node->child_count() != 0) {
                // visit leftmost children first
                node_stack.push({this_node, other_node});

                this_node  = this_node->child(0);
                other_node = other_node->child(0);
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
                    this_node  = this_node->right_sibling();
                    other_node = other_node->right_sibling();
                }
            }
        } while (!node_stack.empty());

        _leftmost  = _root->child(0)->leftmost_child_or_this();
        _rightmost = _root->child(root_child_count)->rightmost_child_or_this();

        other._nodes.clear();
    }

    /// @brief Delete an entire subtree from the node storage, including the
    /// values held by its nodes.
    /// @param branch_root Root of the subtree whose nodes must be deleted
    void _delete_subtree_nodes(node_t* subtree_root) CPPTOOLS_NOEXCEPT_RELEASE_AND(NoExceptErasure) {
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, subtree_root), "unsafe_tree", critical, "subtree root not in tree", exception::parameter::invalid_value_error, "subtree_root", subtree_root);

        for (auto child : subtree_root->release_children()) {
            _delete_subtree_nodes(child);
        }

        _delete_node(subtree_root);
    }

    /// @brief Delete a node and the value it holds
    /// @param n Node to be deleted
    void _delete_node(node_t* n) CPPTOOLS_NOEXCEPT_RELEASE_AND(NoExceptErasure) {
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, n), "unsafe_tree", critical, "node not in tree",                           exception::parameter::invalid_value_error,"n", n);
        CPPTOOLS_DEBUG_ASSERT(n->children().empty(),    "unsafe_tree", critical, "node to be deleted shall not have children", exception::parameter::invalid_value_error,"n", n);

        _nodes.erase(n);
    }

    /// @brief Compare whether two subtrees are equal both in structure and value
    /// @param left First subtree to compare
    /// @param right Second subtree to compare
    /// @return Whether the two subtrees are equal both in structure and value
    static bool _subtrees_equal(node_t* left, node_t* right) {
        CPPTOOLS_DEBUG_ASSERT(both_not_null(left, right), "unsafe_tree", critical, "cannot compare null subtrees",                    exception::parameter::null_parameter_error, "left or right");
        CPPTOOLS_DEBUG_ASSERT(!left->has_parent(right),   "unsafe_tree", critical, "compared subtrees shall not contain one another", exception::parameter::invalid_value_error,  "left", left);
        CPPTOOLS_DEBUG_ASSERT(!right->has_parent(left),   "unsafe_tree", critical, "compared subtrees shall not contain one another", exception::parameter::invalid_value_error,  "right", right);

        using node_pair = std::pair<node_t*, node_t*>;

        // This function performs a pre-order DFS traversal of both trees and
        // compares both structure and value along the way

        // iterative version: use a stack
        std::stack<node_pair, std::vector<node_pair>> node_stack;
        auto this_node = left;
        auto other_node = right;

        do {
            // compare structure
            if (this_node->child_count() != other_node->child_count()) {
                return false;
            }

            // compare value
            if (this_node->value != other_node->value) {
                return false;
            }

            // find next node
            if (this_node->child_count() != 0) {
                // visit leftmost children first
                node_stack.push({this_node, other_node});
                this_node  =  this_node->child(0);
                other_node = other_node->child(0);
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

    /// @brief Construct a tree from a detached subtree
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
        _values(_make_value_view()),
        _alloc(),
        _deleter()
    {
        _root->clear_parent_metadata();
    }

public:
    unsafe_tree(allocator_type alloc = {}) :
        _nodes(),
        _root(nullptr),
        _leftmost(nullptr),
        _rightmost(nullptr),
        _const_values(_make_const_value_view()),
        _values(_make_value_view()),
        _alloc(std::move(alloc)),
        _deleter()
    {

    }

    /// @param copy_from Pointer to the root node of a branch to copy contents
    /// from
    /// @exception Any exception thrown in a constructor of the value type
    /// will be forwarded to the caller
    unsafe_tree(const node_t* copy_from, allocator_type alloc = {}) :
        unsafe_tree(alloc)
    {
        _copy_assign_contents(copy_from);
    }

    /// @param other Tree to copy-construct from
    /// @exception Any exception thrown in a constructor of the value type
    /// will be forwarded to the caller
    unsafe_tree(const unsafe_tree& other, allocator_type alloc = {}) :
        unsafe_tree(other._root, alloc)
    {
    }

    /// @param other Tree to move-construct from
    unsafe_tree(unsafe_tree&& other) :
        _nodes(std::move(other._nodes)),
        _root(other._root),
        _leftmost(other._leftmost),
        _rightmost(other._rightmost),
        _const_values(_make_const_value_view()),
        _values(_make_value_view()),
        _alloc(std::move(other._alloc)),
        _deleter()
    {
        other._root = nullptr;
        other._leftmost = nullptr;
        other._rightmost = nullptr;
    }

    /// @param other Tree to move-construct from
    unsafe_tree(unsafe_tree&& other, allocator_type alloc) :
        _nodes(),
        _root(nullptr),
        _leftmost(nullptr),
        _rightmost(nullptr),
        _const_values(_make_const_value_view()),
        _values(_make_value_view()),
        _alloc(std::move(alloc)),
        _deleter()
    {
        if (other._alloc != alloc) {
            if (!other.empty()) {
                _move_assign_contents(std::move(other));
            }
        } else {
            _nodes = std::move(other._nodes);
            _root = other._root;
            _leftmost = other._leftmost;
            _rightmost = other._rightmost;
        }

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
        _move_fill_from_init(emplaced_root, std::move(init.child_initializers));
    }

    /// @param other Tree to copy-assign contents from
    unsafe_tree& operator=(const unsafe_tree& other) {
        if constexpr (_pocca) {
            _alloc = other._alloc;
        }
        _copy_assign_contents(other._root);

        return *this;
    }

    /// @param other Tree to move-assign contents from
    unsafe_tree& operator=(unsafe_tree&& other) {
        bool steal = (_alloc == other._alloc);

        if constexpr (_pocma) {
            _alloc = std::move(other._alloc);
            steal = true;
        }

        if (steal) {
            _nodes = std::move(other._nodes);
            _root = other._root;
            _leftmost = other._leftmost;
            _rightmost = other._rightmost;
        } else {
            _move_assign_contents(std::move(other));
        }
        other.clear();

        return *this;
    }

    /// @param Init Tree-like initializer list
    unsafe_tree& operator=(initializer&& init) {
        unsafe_tree temp(std::move(init));
        *this = std::move(temp);

        return *this;
    }

    /// @brief Get the allocator instance for this tree
    /// @return The allocator instance for this tree
    allocator_type get_allocator() const {
        return static_cast<allocator_type>(_alloc);
    }

    /// @brief Create and register a new node out of a value, and get a pointer
    /// to it
    /// @tparam ArgTypes Types of the arguments to be forwarded to the 
    /// constructor of the new value
    /// @param args Values of the arguments to be forwarded to the constructor
    /// of the new value
    /// @return A pointer to the newly created node
    template<typename... ArgTypes>
    [[nodiscard]] node_t* make_node(ArgTypes&&... args) {
        node_t *raw_ptr = _al_traits::allocate(_alloc, 1);
        _al_traits::construct(_alloc, raw_ptr, std::forward<ArgTypes>(args)...);

        _nodes.insert(_node_ptr(raw_ptr, _deleter));
        return raw_ptr;
    }

    /// @brief Tells whether emplacing a node somewhere would change the 
    /// leftmost node of the tree
    /// @param dest_node Pointer to the node where the supposed emplacement
    /// would take place; can be null to indicate root emplacement
    /// @return Whether emplacing a node as a new child to \c dest_node would 
    /// change the leftmost node of the tree
    bool emplacing_there_would_change_leftmost(node_t* where) const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(both_null(where, _root) || in_range_keys(_nodes, where), "unsafe_tree", critical, "destination not in tree", exception::parameter::invalid_value_error, "where", where);

        return where == _leftmost;
    }

    /// @brief Tells whether emplacing a node somewhere would change the 
    /// rightmost node of the tree
    /// @param dest_node Pointer to the node where the supposed emplacement
    /// would take place
    /// @return Whether emplacing a node as a new child to \c dest_node would 
    /// change the rightmost node of the tree
    bool emplacing_there_would_change_rightmost(node_t* where) const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(both_null(where, _root) || in_range_keys(_nodes, where), "unsafe_tree", critical, "destination not in tree", exception::parameter::invalid_value_error, "where", where);

        return where == _rightmost
            || _rightmost->has_parent(where);
    }

    /// @brief Detach a subtree
    /// @param subtree_root Root of the subtree to be detached
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

        if (dropping_leftmost)  { _leftmost  = parent; }
        if (dropping_rightmost) { _rightmost = parent; }

        return unsafe_tree(std::move(intermediate_storage), subtree_root, chopped_leftmost, chopped_rightmost);
    }

    /// @brief Acquire the nodes of another tree, making it a subtree of this
    /// tree.
    /// @param destination Node where that should be parent to the stolen nodes
    /// @param other Tree to steal nodes from
    /// @return Pointer to the newly adopted subtree
    node_t* adopt_subtree(node_t* destination, unsafe_tree&& other) {
        CPPTOOLS_DEBUG_ASSERT(not_empty(other),                   "unsafe_tree", critical, "cannot adopt empty tree", exception::parameter::invalid_value_error, "destination", destination);
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, destination), "unsafe_tree", critical, "destination not in tree", exception::parameter::invalid_value_error, "destination", destination);

        bool updating_leftmost  = emplacing_there_would_change_leftmost(destination);
        bool updating_rightmost = emplacing_there_would_change_rightmost(destination);

        node_t* new_subtree = other._root;
        // steal node ownership
        other._move_node_ptrs_recursive(new_subtree, _nodes);
        // attach subtree
        destination->insert_child(new_subtree);

        if (updating_leftmost)  { _leftmost  = other._leftmost;  }
        if (updating_rightmost) { _rightmost = other._rightmost; }

        other.clear();

        return new_subtree;
    }

    /// @brief Get a mutable handle to the root node of the tree
    /// @return A mutable handle to the root node of the tree
    node_t* root() {
        return _root;
    }

    /// @brief Get a const handle to the root ndoe of the tree
    /// @return A const handle to the root ndoe of the tree
    const node_t* root() const {
        return _root;
    }

    /// @brief Get a mutable handle to the leftmost node of the tree
    /// @return A mutable handle to the leftmost node of the tree
    node_t* leftmost() {
        return _leftmost;
    }

    /// @brief Get a const handle to the leftmost node of the tree
    /// @return A const handle to the leftmost node of the tree
    const node_t* leftmost() const {
        return _leftmost;
    }

    /// @brief Get a mutable handle to the rightmost node of the tree
    /// @return A mutable handle to the rightmost node of the tree
    node_t* rightmost() {
        return _rightmost;
    }

    /// @brief Get a const handle to the rightmost node of the tree
    /// @return A const handle to the rightmost node of the tree
    const node_t* rightmost() const {
        return _rightmost;
    }

    /// @brief Move a subtree from somewhere in this tree to somewhere else in
    /// this tree
    /// @param destination The node which the moved subtree should be 
    /// @param root The root node of the subtree to move
    void move_subtree(node_t* destination, node_t* subtree_root) {
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, subtree_root),    "unsafe_tree", critical, "subtree root not in tree",             exception::parameter::invalid_value_error, "subtree_root", subtree_root);
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, destination),     "unsafe_tree", critical, "destination not in tree",              exception::parameter::invalid_value_error, "destination", destination);
        CPPTOOLS_DEBUG_ASSERT(subtree_root != _root,                  "unsafe_tree", critical, "cannot move the root of the tree",     exception::parameter::invalid_value_error, "subtree_root", subtree_root);
        CPPTOOLS_DEBUG_ASSERT(!destination->has_parent(subtree_root), "unsafe_tree", critical, "destination is part of moved subtree", exception::parameter::invalid_value_error, "destination", destination);

        bool dropping_leftmost  = (subtree_root == _leftmost)  || (_leftmost->has_parent(subtree_root));
        bool dropping_rightmost = (subtree_root == _rightmost) || (_rightmost->has_parent(subtree_root));

        bool updating_leftmost  = emplacing_there_would_change_leftmost(destination);
        bool updating_rightmost = emplacing_there_would_change_rightmost(destination);

        node_t* parent = subtree_root->parent();
        parent->remove_child(subtree_root->sibling_index());
        destination->insert_child(subtree_root);

        // For each extremum node, there are four cases to consider:
        //
        //                    | not updating_*most | updating_*most |
        // -------------------|--------------------|----------------|
        // not dropping_*most |          1         |        2       |
        // -------------------|--------------------|----------------|
        //     dropping_*most |          3         |        4       |

        // Case 1: nothing to do
        
        // Case 2:
        if (!dropping_leftmost  && updating_leftmost)  { _leftmost  = destination->leftmost_child_or_this(); }
        if (!dropping_rightmost && updating_rightmost) { _rightmost = destination->rightmost_child_or_this(); }

        // Case 3:
        if (dropping_leftmost  && !updating_leftmost)  { _leftmost  = parent->leftmost_child_or_this(); }
        if (dropping_rightmost && !updating_rightmost) { _rightmost = parent->rightmost_child_or_this(); }

        // Case 4: this situation only arises when a subtree which includes an
        // extremum node is moved somewhere further up its parent branch. In 
        // this case, the extremum node remains an extremum node: no special
        // handling required.

        // Note: due to how the tree is implemented as of writing this comment,
        // case 4 can never happen for the leftmost node of the tree, only for
        // the rightmost node.
    }

    /// @brief Erase an entire subtree and its values
    /// @param to_erase The root node of the subtree to erase
    void erase_subtree(node_t* subtree_root) {
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, subtree_root), "unsafe_tree", critical, "subtree root not in tree", exception::parameter::invalid_value_error, "subtree_root", subtree_root);

        if (subtree_root == _root) {
            clear();
            return;
        }

        bool dropping_leftmost  = (subtree_root == _leftmost)  || (_leftmost->has_parent(subtree_root));
        bool dropping_rightmost = (subtree_root == _rightmost) || (_rightmost->has_parent(subtree_root));

        // the actual node can be deleted afterwards
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
    /// @tparam ArgTypes Types of the arguments to be forwarded to a
    /// constructor of the value to emplace
    /// @param where Handle to the node which is to be the parent of the newly 
    /// emplaced node
    /// @param args Arguments to be forwarded to a constructor of the value to
    /// construct in the newly emplaced node
    /// @return A pointer to the newly emplaced child node
    /// @exception Any exception thrown in the constructor of the emplaced value
    /// will be forwarded to the caller
    template<typename ...ArgTypes>
    node_t* emplace_node(node_t* where, ArgTypes&&... args) CPPTOOLS_NOEXCEPT_RELEASE_AND((std::is_nothrow_constructible_v<value_type, ArgTypes...>)) {
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

    /// @brief Merge the provided node into its parent node:
    /// - the value of the provided node is merged into the value of the parent
    /// node using the merge strategy provided as a template parameter
    /// - the children of the provided node are all inserted in-between this
    /// node's left and right siblings, their relative order is preserved, and
    /// their new parent is this node's parent
    /// - the provided node is deleted from the tree
    /// @tparam merge_t A function-like type satisfying the \c merge_strategy
    /// concept for this tree's value type. The default merge strategy keeps the
    /// parent node's value, discarding that of the provided node.
    template<merge_strategy<T> merge_t = merge::keep>
    void merge_with_parent(node_t* n) {
        CPPTOOLS_DEBUG_ASSERT(not_null(n),              "unsafe_tree", critical, "cannot merge null node with parent", exception::parameter::null_parameter_error, "n");
        CPPTOOLS_DEBUG_ASSERT(in_range_keys(_nodes, n), "unsafe_tree", critical, "node not in tree",                   exception::parameter::invalid_value_error,  "n", n);

        node_t* parent = n->parent();

        CPPTOOLS_DEBUG_ASSERT(not_null(parent),         "unsafe_tree", critical, "cannot merge node with null parent", exception::parameter::invalid_value_error,  "n", n);

        parent->template merge_child<merge_t>(n->sibling_index());

        if (_leftmost == n) {
            _leftmost = parent->leftmost_child_or_this();
        }
        if (_rightmost == n) {
            _rightmost = parent->rightmost_child_or_this();
        }

        _delete_node(n);
    }

    /// @brief Check that this tree's structure and values are the same as that
    /// of another tree
    /// @param other Other tree to compare this tree to
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

    /// @brief Swap around the contents of two trees
    /// @param lhs First tree
    /// @param rhs Second tree
    friend void swap(unsafe_tree& lhs, unsafe_tree& rhs) noexcept(NoExceptSwap) {
        std::swap(lhs._nodes, rhs._nodes);
        std::swap(lhs._root, rhs._root);
        std::swap(lhs._leftmost, rhs._leftmost);
        std::swap(lhs._rightmost, rhs._rightmost);
    }

    /// @brief Get the size of the tree
    size_type size() const {
        return _nodes.size();
    }

    /// @brief Get the maximum size the tree can have
    size_type max_size() const {
        return _nodes.max_size();
    }

    /// @brief Get whether the tree is empty
    bool empty() const {
        return _nodes.empty();
    }

    /// @brief Clear the tree
    void clear() {
        _nodes.clear();
        _root = nullptr;
        _leftmost = nullptr;
        _rightmost = nullptr;
    }

    /// @brief Get the begin iterator for a fast traversal of the tree (no order
    /// guarantee)
    /// @return The begin iterator for a fast traversal of the tree
    iterator begin() {
        return values().begin();
    }

    /// @brief Get the end iterator for a fast traversal of the tree (no order
    /// guarantee)
    /// @return The end iterator for a fast traversal of the tree
    iterator end() {
        return values().end();
    }

    /// @brief Get the begin iterator for a fast const traversal of the tree (no
    /// order guarantee)
    /// @return The begin iterator for a fast const traversal of the tree
    const_iterator cbegin() const {
        return const_values().begin();
    }

    /// @brief Get the end iterator for a fast const traversal of the tree (no
    /// order guarantee)
    /// @return The end iterator for a fast const traversal of the tree
    const_iterator cend() const {
        return const_values().end();
    }

    /// @copydoc unsafe_tree<T>::cbegin
    const_iterator begin() const {
        return const_values().begin();
    }

    /// @copydoc unsafe_tree<T>::cend
    const_iterator end() const {
        return const_values().end();
    }
};

} // namespace tools::detail

#include <cpptools/_internal/undef_debug_macros.hpp>

#endif//CPPTOOLS_CONTAINER_TREE_UNSAFE_TREE_HPP

//////////
// TODO //
//////////

/*
 - profile different impls for _subtrees_equal:
    - compare structure when stacking, and then values only when unstacking
    - recursive version
 - _copy_assign_contents should be iterative (reuse _move_assign_contents)
 - reverberate allocator awareness in tree.hpp
 - swap
 - investigate reusing nodes instead of clearing and then copying during copy-assignment
 */