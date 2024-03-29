#ifndef CPPTOOLS__CONTAINER__TREE__NODE_HPP
#define CPPTOOLS__CONTAINER__TREE__NODE_HPP

#include <algorithm>
#include <iterator>
#include <numeric>
#include <type_traits>
#include <vector>

#include <cpptools/exception/exception.hpp>
#include <cpptools/exception/internal_exception.hpp>
#include <cpptools/exception/parameter_exception.hpp>
#include <cpptools/utility/merging_strategy.hpp>

#ifndef CPPTOOLS_DEBUG_NODE
# define CPPTOOLS_DEBUG_NODE CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH
#endif

#define CPPTOOLS_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE
#define CPPTOOLS_LOCAL_DEBUG_MACRO CPPTOOLS_DEBUG_NODE
#include <cpptools/_internal/debug_macros.hpp>

namespace tools::detail {

/// @brief A node in an arbitrary tree, to be used with unsafe_tree<T>.
///
/// @note Enable debug assertions with #define CPPTOOLS_DEBUG_NODE 1
template<typename T>
class node {
public:
    using value_type = T;
    using storage_type = std::vector<node*>;
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

    /// @brief Pointers to successive parent nodes
    storage_type _parent_chain;


public:
    template<typename ...ArgTypes>
    node(ArgTypes&&... args) noexcept(std::is_nothrow_constructible_v<T, ArgTypes&&...>):
        value{std::forward<ArgTypes&&>(args)...},
        _parent(nullptr),
        _sibling_index(),
        _children(),
        _parent_chain() {

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
        CPPTOOLS_DEBUG_ASSERT(not_null(_parent),      "node", critical, "node has no parent",                    exception::internal::precondition_failure_error);
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
    ///
    /// @param child Pointer to the node to insert as a new child
    ///
    /// @pre \c child must not be null.
    /// @pre \c child must point to an orphaned node.
    ///
    /// @post The parent metadata of \c child was updated and has become
    /// valid again, as well as that of all of its own children.
    void insert_child(node* child) {
        CPPTOOLS_DEBUG_ASSERT(not_null(child), "node", critical, "child to be inserted is null", exception::parameter::null_parameter_error, "child");

        _children.push_back(child);
        child->_parent = this;
        child->_sibling_index = _children.size() - 1;
        child->update_parent_chain_and_propagate();
    }

    /// @brief Remove a child from this node
    ///
    /// @param index Index of the child node to be removed
    ///
    /// @pre \c child must not be a valid index in this node's vector of
    /// children nodes.
    ///
    /// @note The parent metadata of \c child is left untouched and thus
    /// becomes invalid, as well as that of all of its children nodes.The parent
    /// metadata may become valid again upon insertion of the node as a child to
    /// another node.
    ///
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
    /// 
    /// @tparam merge_t Default-constructible, invocable type which merges
    /// its second argument into the first.
    /// 
    /// @param index The index of the child to merge with this node.
    /// 
    /// @pre \c index must be a valid index into this node's vector of
    /// children.
    /// 
    /// @note The child node is NOT deleted from the tree as part of this
    /// operation.
    template<merging_strategy<T> merge_t>
    void merge_child(size_t index) {
        CPPTOOLS_DEBUG_ASSERT(index < _children.size(), "node", critical, "index out of bounds", exception::parameter::invalid_value_error);

        node* to_merge = _children[index];
        std::vector<node*> to_adopt = to_merge->release_children();

        if (!to_adopt.empty()) {
            size_t child_count         = _children.size();
            size_t right_sibling_count = child_count - 1 - to_merge->sibling_index();
            size_t insert_count        = to_merge->_children.size() - 1; // -1: the node to merge will be overwritten and this does not count as an insertion
            _children.resize(child_count + insert_count, nullptr);

            auto to_merge_it = _children.begin() + to_merge->sibling_index();
            if (right_sibling_count > 0) {
                // move right siblings of the node to merge at the end of the resized children vector
                auto it_right_sibling = to_merge_it + 1;
                std::copy_backward(it_right_sibling, it_right_sibling + right_sibling_count, _children.end());
            }

            // adopt grand-children
            for (auto new_child : to_adopt) {
                new_child->_parent = this;
                new_child->update_parent_chain_and_propagate();

                *(to_merge_it++) = new_child;
            }
        }

        // merge the node value into this node's value
        merge_t{}(value, to_merge->value);
    }

    bool has_parent(const node* parent) const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(parent), "node", pedantic, "node cannot have nullptr parent", exception::parameter::null_parameter_error, "parent", nullptr);

        auto it = std::find(_parent_chain.begin(), _parent_chain.end(), parent);

        return it != _parent_chain.end();
    }

    void update_parent_chain() {
        CPPTOOLS_DEBUG_ASSERT(not_null(_parent), "node", critical, "node has no parent", exception::internal::precondition_failure_error);

        _parent_chain.clear();

        _parent_chain.reserve(_parent->_parent_chain.size() + 1);
        _parent_chain.push_back(_parent);
        std::copy(
            _parent->_parent_chain.begin(),
            _parent->_parent_chain.end(),
            std::back_inserter(_parent_chain)
        );
    }

    void propagate_parent_chain_update() {
        for (auto child : _children) {
            child->update_parent_chain_and_propagate();
        }
    }

    void update_parent_chain_and_propagate() {
        CPPTOOLS_DEBUG_ASSERT(_parent != nullptr, "node", critical, "node has no parent", exception::internal::precondition_failure_error);

        update_parent_chain();
        propagate_parent_chain_update();
    }

    void clear_parent_metadata() noexcept {
        _parent = nullptr;
        _sibling_index = 0;
        _parent_chain.clear();
    }

    const std::vector<node*>& children() const noexcept {
        return _children;
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

    const std::vector<node*>& siblings() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_parent), "node", critical, "node has no parent", exception::internal::precondition_failure_error);

        return _parent->_children;
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

    size_t sibling_count() const CPPTOOLS_NOEXCEPT_RELEASE {
        CPPTOOLS_DEBUG_ASSERT(not_null(_parent), "node", critical, "node has no parent", exception::internal::precondition_failure_error);

        return _parent->child_count();
    }
};

} // namespace tools::detail

#include <cpptools/_internal/undef_debug_macros.hpp>

#endif//CPPTOOLS__CONTAINER__TREE__NODE_HPP