#ifndef CPPTOOLS_CONTAINER_TREE_NODE_HPP
#define CPPTOOLS_CONTAINER_TREE_NODE_HPP

#include <algorithm>
#include <memory>
#include <numeric>
#include <type_traits>
#include <vector>

#include <cpptools/exception/exception.hpp>
#include <cpptools/exception/internal_exception.hpp>
#include <cpptools/exception/parameter_exception.hpp>
#include <cpptools/utility/allocator.hpp>
#include <cpptools/utility/merge_strategy.hpp>

#ifndef CPPTOOLS_DEBUG_NODE
# define CPPTOOLS_DEBUG_NODE CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH
#endif

#define CPPTOOLS_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE
#define CPPTOOLS_LOCAL_DEBUG_MACRO CPPTOOLS_DEBUG_NODE
#include <cpptools/_internal/debug_macros.hpp>

namespace tools::detail {

/// @brief A node in an arbitrary tree, to be used with unsafe_tree<T>.
/// @note Enable debug assertions with #define CPPTOOLS_DEBUG_NODE 1
template<typename T, typename A = std::allocator<T>>
class node {
public:
    using value_type = T;
    using allocator_type = A;

private:
    using _al_type   = rebind_alloc_t<A, T>;
    using _al_traits = std::allocator_traits<allocator_type>;

public:
    using storage_type = std::vector<node*, rebind_alloc_t<_al_type, node*>>;
    using size_type = typename storage_type::size_type;

    /// @brief Value attached to this node
    value_type value;

private:
    node(const node&  other) = delete;
    node(      node&& other) = delete;
    node& operator=(const node&  other) = delete;
    node& operator=(      node&& other) = delete;

    /// @brief Pointer to the parent of this node
    node* _parent;

    /// @brief Index of this node in its parent's vector of children nodes
    size_type _sibling_index;

    /// @brief Pointers to the children of this node
    storage_type _children;


public:
    template<typename ...ArgTypes>
    node(ArgTypes&&... args) noexcept(std::is_nothrow_constructible_v<T, ArgTypes&&...>):
        value{std::forward<ArgTypes&&>(args)...},
        _parent(nullptr),
        _sibling_index(),
        _children()
    {

    }

    node* leftmost_child_or_this() noexcept {
        node* leftmost = this;
        while (!leftmost->_children.empty()) {
            leftmost = leftmost->_children.front();
        }

        return leftmost;
    }

    node* rightmost_child_or_this() noexcept {
        node* rightmost = this;
        while (!rightmost->_children.empty()) {
            rightmost = rightmost->_children.back();
        }

        return rightmost;
    }

    node* left_sibling() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_parent),      "node", critical, "node has no parent",                   exception::internal::precondition_failure_error);
        CPPTOOLS_DEBUG_ASSERT(!is_leftmost_sibling(), "node", critical, "leftmost sibling has no left sibling", exception::internal::precondition_failure_error);

        return _parent->_children[_sibling_index - 1];
    }

    node* right_sibling() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_parent),       "node", critical, "node has no parent",                    exception::internal::precondition_failure_error);
        CPPTOOLS_DEBUG_ASSERT(!is_rightmost_sibling(), "node", critical, "rightmost sibling has no right sibling",exception::internal::precondition_failure_error);

        return _parent->_children[_sibling_index + 1];
    }

    bool is_leftmost_sibling() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_parent),  "node", critical, "node has no parent", exception::internal::precondition_failure_error);

        return _sibling_index == 0;
    }

    bool is_rightmost_sibling() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_parent),  "node", critical, "node has no parent", exception::internal::precondition_failure_error);

        return _sibling_index == (_parent->children().size() - 1);
    }

    /// @brief Append a new child node to this node, updating the \c _parent
    /// pointer and \c _sibling_index in the child node
    /// @param child Pointer to the node to insert as a new child
    /// @pre \c child must not be null.
    /// @pre \c child must point to an orphaned node.
    /// @post The parent metadata of \c child was updated and has become
    /// valid again, as well as that of all of its own children.
    void insert_child(node* child) {
        CPPTOOLS_DEBUG_ASSERT(not_null(child), "node", critical, "child to be inserted is null", exception::parameter::null_parameter_error, "child");

        _children.push_back(child);
        child->_parent = this;
        child->_sibling_index = _children.size() - 1;
    }

    /// @brief Remove a child from this node
    /// @param index Index of the child node to be removed
    /// @pre \c child must not be a valid index in this node's vector of
    /// children nodes.
    /// @note The parent metadata of \c child is left untouched and thus
    /// becomes invalid, as well as that of all of its children nodes.The parent
    /// metadata may become valid again upon insertion of the node as a child to
    /// another node.
    /// @see template<typename T> tree::node::insert_child(node* child)
    node* remove_child(size_type index) CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(index < _children.size(), "node", critical, "index out of bounds", exception::parameter::null_parameter_error, "child", nullptr);

        node* removed_child = _children[index];
        auto it = _children.erase(_children.begin() + index);

        // decrement parent index of right siblings of removed child
        auto end = _children.end();
        for (; it != end; it++) {
            --((*it)->_sibling_index);
        }

        return removed_child;
    }

    /// @brief Merge the child node at the specified index: its value is
    /// merged with this node's value according to the provided merging
    /// strategy, and its children are adopted in its place. The child node
    /// is then dropped.
    /// @tparam merge_t Default-constructible, invocable type which merges
    /// its second argument into the first.
    /// @param index The index of the child to merge with this node.
    /// @pre \c index must be a valid index into this node's vector of children.
    /// @note The child node is NOT deleted from the tree as part of this
    /// operation. However since the child node is going to be deleted soon 
    /// afterwards anyway, the value to be merged is passed by move to the merge
    /// strategy.
    template<merge_strategy<T> merge_t>
    void merge_child(std::size_t merge_index) {
        CPPTOOLS_DEBUG_ASSERT(merge_index < _children.size(), "node", critical, "index out of bounds", exception::parameter::invalid_value_error, "index", merge_index);

        node* to_merge = _children[merge_index];
        std::vector<node*> to_adopt = to_merge->release_children();
        std::size_t adopted_count = to_adopt.size();

        if (adopted_count != 0) {
            std::size_t child_count           = _children.size();
            std::size_t right_sibling_count   = child_count - 1 - to_merge->sibling_index();
            std::size_t additional_node_count = adopted_count - 1; // -1: the node to merge will be overwritten and this does not count as an insertion
            _children.resize(child_count + additional_node_count, nullptr);
            auto to_merge_it = _children.begin() + merge_index;

            if (right_sibling_count > 0 && adopted_count > 1) {
                // move right siblings of the node to merge at the end of the resized children vector
                auto it_right_sibling = to_merge_it + 1;
                std::copy_backward(it_right_sibling, it_right_sibling + right_sibling_count, _children.end());
            }

            // adopt grand-children
            for (auto new_child : to_adopt) {
                new_child->_parent = this;
                *(to_merge_it++) = new_child;
            }
        } else {
            auto to_merge_it = _children.begin() + merge_index;
            _children.erase(to_merge_it);
        }

        // sibling indices of nodes that were moved around may need to be updated
        // compute the range of such nodes

        // start of the range: decide whether the adopted nodes need updating, or not (in which case they can be skipped)
        std::size_t start_offset = (merge_index == 0)
                ? adopted_count     // adopted nodes inserted as first children, sibling index remained up-to-date => skip them
                : merge_index;      // adopted nodes inserted in the middle, sibling index needs update => include them

        auto outdated_it = _children.begin() + start_offset;

        // end of the range: decide whether the right sibling nodes need updating, or not (in which case they can be omitted)
        auto outdated_index_end = (adopted_count == 1)
            ? _children.begin() + (merge_index + adopted_count) // right siblings were not moved, sibling index remained up-to-date => omit them
            : _children.end();                                  // right siblings were moved, sibling index needs update => include them

        // update the sibling index of the nodes in the computed range
        for (; outdated_it != outdated_index_end; ++outdated_it) {
            (*outdated_it)->_sibling_index = start_offset++;
        }

        // merge the node value into this node's value
        merge_t{}(value, std::move(to_merge->value));
    }

    bool has_parent(const node* n) const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(n), "node", pedantic, "node cannot have nullptr parent", exception::parameter::null_parameter_error, "n", nullptr);

        auto parent = _parent;
        while (parent != nullptr) {
            if (parent == n) {
                return true;
            }

            parent = parent->_parent;
        }

        return false;
    }

    void clear_parent_metadata() noexcept {
        _parent = nullptr;
        _sibling_index = 0;
    }

    const std::vector<node*>& children() const noexcept {
        return _children;
    }

    node* child(size_t index) const noexcept {
        return _children[index];
    }

    size_type child_count() const noexcept {
        return _children.size();
    }

    size_type descendant_count() const noexcept {
        return std::accumulate(
            _children.begin(), _children.end(), size_type{},
            [](size_type result, const node* n) {
                return result + 1 + n->descendant_count();
            }
        );
    }

    node* parent() const noexcept {
        return _parent;
    }

    [[nodiscard]] std::vector<node*> release_children() noexcept {
        std::vector<node*> result = std::move(_children);
        _children.clear();

        return result;
    }

    size_t sibling_index() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_parent), "node", critical, "node has no parent", exception::internal::precondition_failure_error);

        return _sibling_index;
    }

    void reserve(size_t capacity) {
        _children.reserve(capacity);
    }
};

} // namespace tools::detail

#include <cpptools/_internal/undef_debug_macros.hpp>

#endif//CPPTOOLS_CONTAINER_TREE_NODE_HPP