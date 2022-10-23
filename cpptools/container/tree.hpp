#ifndef TOOLS__CONTAINER__TREE_HPP
#define TOOLS__CONTAINER__TREE_HPP

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <cpptools/exception/exception.hpp>
#include <cpptools/exception/internal_exception.hpp>
#include <cpptools/exception/parameter_exception.hpp>
#include <cpptools/exception/iterator_exception.hpp>

#if CPPTOOLS_DEBUG_TREE != 0
#   if not defined CPPTOOLS_DEBUG_ENABLED or CPPTOOLS_DEBUG_ENABLED == 0
#       warning CPPTOOLS_DEBUG_TREE is defined and non-zero but CPPTOOLS_DEBUG_ENABLED is not
#   endif
#endif
#include <cpptools/_internal/debug.hpp>

namespace tools::container
{

enum class tree_traversal
{
    post_order, // L R N
    pre_order   // N L R
};

/// @brief An arbitrary tree. STL-compatible.
///
/// @tparam T Type of values to be stored
///
/// @note Enable debug assertions with #define CPPTOOLS_DEBUG_TREE 1
template<typename T>
class tree
{
public:
    using traversal = tree_traversal;

private:
    static constexpr traversal default_traversal = traversal::pre_order;

public:
    using value_type        = T;
    using size_type         = std::size_t;
    using reference         = value_type&;
    using const_reference   = const value_type&;

private:
    template<traversal t = default_traversal>
    class iterator_impl;

    template<traversal>
    friend class iterator_impl;

    /// @brief A node in an arbitrary tree
    class node
    {
    public:
        using size_type = typename std::vector<node*>::size_type;

        /// @brief Value attached to this node
        T value;

    private:
        node(const node& other) = delete;
        node& operator=(const node& other) = delete;

        /// @brief Pointer to the parent of this node
        node* _parent;

        /// @brief Index of this node in its parent's vector of children nodes
        size_type _parent_index;

        /// @brief Pointers to the children of this node
        std::vector<node*> _children;

        /// @brief Pointers to successive parent nodes
        std::vector<node*> _parent_chain;


    public:
#if CPPTOOLS_DEBUG_TREE != 0
        /// @brief The tree this node belongs to.
        ///
        /// @note THIS MEMBER IS TO BE USED FOR DEBUGGING PURPOSES ONLY.
        tree* tree;
#endif

        /// @param args Arguments to construct the node value from
        template<typename ...ArgTypes>
        node(ArgTypes&&... args) noexcept(std::is_nothrow_constructible_v<T, ArgTypes&&...>):
            value(std::forward<ArgTypes&&>(args)...),
            _parent(nullptr),
            _children(),
            _parent_index(),
            _parent_chain()
        {

        }

        ~node() = default;

        /// @brief Get a pointer to the leftmost child node of this node, or to
        /// this node if it has no children
        ///
        /// @return A pointer as described above
        node* leftmost_child_or_this() const noexcept
        {
            node* root = this;
            while (!root->_children.empty())
            {
                root = root->_children.front();
            }

            return root;
        }

        /// @brief Get a pointer to the rightmost child node of this node, or to
        /// this node if it has no children
        ///
        /// @return A pointer as described above
        node* rightmost_child_or_this() const noexcept
        {
            node* root = this;
            while (!root->_children.empty())
            {
                root = root->_children.back();
            }

            return root;
        }

        /// @brief Tells whether or not this node is the left most sibling in
        /// its parent's collection of children
        ///
        /// @return Whether or not this node is the left most sibling in
        /// its parent's collection of children
        ///
        /// @note It is a precondition that this node must have a non-null
        /// parent when calling this function.
        bool is_leftmost_sibling() const
        {
            CPPTOOLS_DEBUG_ASSERT(_parent != nullptr, "tree", critical, "called is_leftmost_sibling on node with no parent", exception::internal::precondition_failure_error);
            return _parent_index == 0;
        }

        /// @brief Tells whether or not this node is the right most sibling in
        /// its parent's collection of children
        ///
        /// @return Whether or not this node is the right most sibling in
        /// its parent's collection of children
        ///
        /// @note It is a precondition that this node must have a non-null
        /// parent when calling this function.
        bool is_rightmost_sibling() const
        {
            CPPTOOLS_DEBUG_ASSERT(_parent != nullptr, "tree", critical, "called is_leftmost_sibling on node with no parent", exception::internal::precondition_failure_error);
            return _parent_index == (_parent->children().size() - 1);
        }

        /// @brief Get a pointer to the next node from this one, as in a
        /// pre-order traversal of the tree
        ///
        /// @return A pointer as described above
        node* next_pre_order() const
        {
            // Structure:        Traversal:                     //
            //        N                --- N (begin)            //
            //      /   \             /                         //
            //     L     R           L   ------ R               //
            //    / \   / \         /   /      /                //
            //   L1 L2 R1 R2      L1 - L2    R1 - R2 (end)      //

            if (!_children.empty())
            {
                // first child
                return _children.front();
            }
            else if (_parent)
            {        
                if (!is_rightmost_sibling())
                {
                    // immediate right sibling
                    return _parent->_children[_parent_index + 1];;
                }
                else
                {
                    // find the topmost parent node whose rightmost child is this node
                    node* top_parent = _parent;
                    while (top_parent->_parent && top_parent->is_rightmost_sibling())
                    {
                        top_parent = top_parent->_parent;
                    }

                    node* next_parent = top_parent->_parent;
                    if (next_parent)
                    {
                        // immediate right sibling of this parent
                        return next_parent->_children[top_parent->_parent_index + 1];
                    }
                }
            }
            return nullptr;
        }

        /// @brief Get a pointer to the previous sibling node from this one, as
        /// in a pre-order traversal of the tree
        ///
        /// @return A pointer as described above
        node* previous_pre_order() const
        {
            // Structure:        Traversal:                     //
            //        N                --- N (end)              //
            //      /   \             /                         //
            //     L     R           L   ------ R               //
            //    / \   / \         /   /      /                //
            //   L1 L2 R1 R2      L1 - L2    R1 - R2 (begin)    //

            return (_parent && !is_leftmost_sibling())
                ? _parent->_children[_parent_index - 1]->rightmost_child_or_this()
                : _parent;
        }

        /// @brief Get a pointer to the next sibling node from this one, as in
        /// a post-order traversal of the tree
        ///
        /// @return A pointer as described above
        node* next_post_order() const
        {
            // Structure:                Traversal:             //
            //        N                     (end) N --          //
            //      /   \                             \         //
            //     L     R                  L -----    R        //
            //    / \   / \                  \     \    \       //
            //   L1 L2 R1 R2     (begin) L1 - L2    R1 - R2     //

            return (_parent && !is_rightmost_sibling())
                ? _parent->_children[_parent_index + 1]->leftmost_child_or_this()
                : _parent;
        }

        /// @brief Get a pointer to the previous sibling node from this one, as
        /// in a post-order traversal of the tree
        ///
        /// @return A pointer as described above
        node* previous_post_order() const
        {
            // Structure:                Traversal:             //
            //        N                   (begin) N --          //
            //      /   \                             \         //
            //     L     R                  L -----    R        //
            //    / \   / \                  \     \    \       //
            //   L1 L2 R1 R2       (end) L1 - L2    R1 - R2     //

            if (!_children.empty())
            {
                // last child
                return _children.back();
            }
            else if (_parent)
            {
                if (!is_leftmost_sibling())
                {
                    // immediate left sibling
                    return _parent->_children[_parent_index - 1];;
                }
                else
                {
                    // find the topmost parent node whose leftmost child is this node
                    node* top_parent = _parent;
                    while (top_parent->_parent && top_parent->is_leftmost_sibling())
                    {
                        top_parent = top_parent->_parent;
                    }

                    node* next_parent = top_parent->_parent;
                    if (next_parent)
                    {
                        // immediate left sibling of this parent
                        return next_parent->_children[top_parent->_parent_index - 1];
                    }
                }
            }
            return nullptr;
        }

        /// @brief Append a new child node to this node
        ///
        /// @param child Pointer to the child node to be removed
        ///
        /// @note As part of this operation, the internal book-keeping of the
        /// child node, as well as that of its own children, will be entirely
        /// taken care of.
        void insert_child(node* child)
        {
            CPPTOOLS_DEBUG_ASSERT(child != nullptr, "tree", critical, "cannot insert null child", exception::parameter::null_parameter_error, "child", nullptr);

            _children.push_back(child);

            // update child book-keeping
            child->_parent_index = _children.size() - 1;
            child->_parent = this;
            child->update_parent_chain();
            child->propagate_parent_chain_update();
        }

        /// @brief Update this node's parent chain
        ///
        /// @note This function will dereference this node's parent without
        /// checking
        void update_parent_chain()
        {
            CPPTOOLS_DEBUG_ASSERT(_parent != nullptr, "tree", critical, "called update_parent_chain on node with no parent", exception::internal::precondition_failure_error);

            _parent_chain.clear();

            _parent_chain.reserve(_parent->_parent_chain.size() + 1);
            _parent_chain.push_back(_parent);
            std::copy(
                _parent->_parent_chain.begin(),
                _parent->_parent_chain.end(),
                std::back_inserter(_parent_chain)
            );
        }

        /// @brief Recursively update _parent_chain in all of the children nodes
        /// of this node
        void propagate_parent_chain_update()
        {
            for (auto child : _children)
            {
                child->update_parent_chain();
                child->propagate_parent_chain_update();
            }
        }

        /// @brief Remove a child from this node
        ///
        /// @param child Pointer to the child node to be removed
        ///
        /// @note As part of this operation, the internal book-keeping of the
        /// child node will be taken care of, as well as its siblings'.
        void remove_child(node* child)
        {
            CPPTOOLS_DEBUG_ASSERT(child != nullptr, "tree", critical, "cannot remove null child", exception::parameter::null_parameter_error, "child", nullptr);
            CPPTOOLS_DEBUG_ASSERT(_children.find(child) != _children.end(), "tree", critical, "node to be removed is not a child of this node", exception::parameter::invalid_value_error, "child", nullptr);

            auto it = _children.erase(_children.begin() + child->_parent_index);

            // decrement parent index of right siblings of removed child
            auto end = _children.end();
            for (; it != end; it++)
            {
                --((*it)->_parent_index);
            }

            // clear parent metadata in removed child            
            child->_parent_index = 0;
            child->_parent_chain.clear();
            child->_parent = nullptr;
        }

        /// @brief Whether this node is a child (to any degree) of a certain
        /// node
        bool has_parent(node* parent) const
        {
            CPPTOOLS_DEBUG_ASSERT(parent != nullptr, "tree", pedantic, "has_parent called with nullptr", exception::parameter::null_parameter_error, "parent", nullptr);

            auto it = std::find(_parent_chain.begin(), _parent_chain.end(), parent);

            return it != _parent_chain.end();
        }

        /// @brief Get pointers to the children of this node
        ///
        /// @return A vector of pointers to children nodes
        std::vector<node*>& children()
        {
            return _children;
        }

        /// @brief Get a pointer to the parent of this node
        ///
        /// @return A pointer to the parent node
        node* parent()
        {
            return _parent;
        }
    };
    using node_ptr          = std::unique_ptr<node>;

public:
    using key_type          = node*;
    using storage_type      = std::map<key_type, node_ptr>;
    using size_type         = typename storage_type::size_type;
    using pointer           = typename storage_type::pointer;
    using const_pointer     = typename storage_type::const_pointer;
    using difference_type   = typename storage_type::difference_type;
    using key_compare       = typename storage_type::key_compare;
    using value_compare     = typename storage_type::value_compare;
    using allocator_type    = typename storage_type::allocator_type;

private:
    /// @brief Constant iterator to an element in the tree. Allows bidirectional
    /// DFS traversal of the tree.
    /// 
    /// @tparam t Type of traversal to be implemented by the iterator
    ///
    /// @note Iterators are invalidated only when the node they point to is no
    /// longer part of the tree which it originally belonged to when the 
    /// iterator was first instantiated.
    template<traversal t>
    class iterator_impl
    {
    friend class tree;

    public:
        using value_type        = tree::value_type;
        using reference         = tree::const_reference;

        using iterator_category = std::bidirectional_iterator_tag;
        using const_reference   = tree::const_reference;
        using pointer           = tree::pointer;
        using difference_type   = tree::difference_type;
        using size_type         = tree::size_type;

    protected:
        /// @brief Pointer to the tree being traversed
        tree* _tree;

        /// @brief Pointer to the tree node currently being iterated over
        ///
        /// @note If _node == nullptr, then *this == _tree->end().
        node* _node;

        /// @brief Pointer to the value of the current node
        value_type* _value;

        /// @param tree Pointer to the tree being traversed
        /// @param node Pointer to the node to start iterating with
        iterator_impl(tree* tree, node* node) :
            _tree(tree),
            _node(node),
            _value(node ? &(node->value) : nullptr)
        {
        }

#if CPPTOOLS_DEBUG_TREE != 0
        /// @brief Whether or not this iterator is valid
        ///
        /// @note THIS MEMBER IS TO BE USED FOR DEBUGGING PURPOSES ONLY.
        bool _is_valid() const
        {
            return (_node == nullptr) || (_tree == _node->tree);
        }
#endif

    public:
        iterator_impl() :
            _tree(nullptr),
            _node(nullptr),
            _value(nullptr)
        {

        }

        ~iterator_impl() = default;

        /// @param other Iterator to copy contents from
        ///
        /// @tparam t2 Type of traversal implemented by the provided iterator
        template<traversal t2>
        iterator_impl(const iterator_impl<t2>& other) :
            _tree(_tree_from_iterator(other)),
            _node(_node_from_iterator(other)),
            _value(_value_from_iterator(other))
        {
            CPPTOOLS_DEBUG_ASSERT(other._is_valid(), "iterator", critical, "copy-constructing from invalid iterator", exception::iterator::operation_while_invalid_error);
        }

        /// @brief Standard swap between two iterators
        ///
        /// @tparam t2 Type of traversal implemented by the provided iterator
        /// 
        /// @param lhs First iterator to swap
        /// @param rhs Second iterator to swap
        template<traversal t2>
        friend void swap(iterator_impl& lhs, iterator_impl<t2>& rhs)
        {
            CPPTOOLS_DEBUG_ASSERT(lhs._is_valid(), "iterator", critical, "swap involving an invalid iterator", exception::parameter::invalid_value_error, "lhs");
            CPPTOOLS_DEBUG_ASSERT(rhs._is_valid(), "iterator", critical, "swap involving an invalid iterator", exception::parameter::invalid_value_error, "rhs");

            std::ranges::swap(lhs._tree,  tree<T>::_tree_from_iterator(rhs));
            std::ranges::swap(lhs._node,  tree<T>::_node_from_iterator(rhs));
            std::ranges::swap(lhs._value, tree<T>::_value_from_iterator(rhs));
        }

        /// @param other Iterator to copy-assign contents from
        ///
        /// @tparam t2 Type of traversal implemented by the provided iterator
        /// 
        /// @return This iterator
        template<traversal t2>
        iterator_impl& operator=(iterator_impl<t2> other)
        {
            swap(*this, other);

            return *this;
        }

        iterator_impl& operator++()
        {
            CPPTOOLS_DEBUG_ASSERT(_node != nullptr, "iterator", critical, "iterator incremented past end", exception::iterator::incremented_past_end_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "operator++() called on invalid iterator", exception::iterator::operation_while_invalid_error);

            if constexpr (t == traversal::pre_order)
            {
                _node = _node->next_pre_order();
            }
            else // if constexpr (t == traversal::post_order)
            {
                _node = _node->next_post_order();
            }

            _value = nullptr;
            if (_node) [[likely]]
                _value = &(_node->value);

            return *this;
        }

        iterator_impl operator++(int)
        {
            iterator_impl tmp = *this;
            ++*this;
            return tmp;
        }

        iterator_impl& operator--()
        {
            CPPTOOLS_DEBUG_ASSERT(*this != _tree->begin<t>(), "iterator", critical, "iterator decremented past begin", exception::iterator::decremented_past_begin_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "operator--() called on invalid iterator", exception::iterator::operation_while_invalid_error);

            if constexpr (t == traversal::pre_order)
            {
                if (_node) [[likely]]
                    _node = _node->previous_pre_order();
                else
                    _node = _tree->_rightmost;
            }
            else // if constexpr (t == traversal::post_order)
            {
                if (_node) [[likely]]
                    _node = _node->previous_post_order();
                else
                    _node = _tree->_root;
            }

            _value = nullptr;
            if (_node) [[likely]]
                _value = &(_node->value);

            return *this;
        }

        iterator_impl operator--(int)
        {
            iterator_impl tmp = *this;
            --*this;
            return tmp;
        }

        const_reference operator*() const
        {
            CPPTOOLS_DEBUG_ASSERT(_value != nullptr, "iterator", critical, "dereferenced while pointing at no node", exception::iterator::illegal_dereference_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "dereference on invalid iterator", exception::iterator::operation_while_invalid_error);

            return *_value;
        }

        const_pointer operator->() const
        {
            CPPTOOLS_DEBUG_ASSERT(_value != nullptr, "iterator", critical, "member accessed while pointing at no node", exception::iterator::illegal_dereference_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "member access on invalid iterator", exception::iterator::operation_while_invalid_error);

            return _value;
        }

        bool operator==(const iterator_impl& rhs) const
        {
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "operator==() called on invalid iterator", exception::iterator::operation_while_invalid_error);

            return (_node && (_node == rhs._node))
                || (_tree == rhs._tree);

            // comparing tree pointers ensures that t1.end() != t2.end()
        }

        bool operator!=(const iterator_impl& rhs) const
        {
            return !(operator==(rhs));
        }

        /// @brief Get an iterator to the parent node of this iterator's node,
        /// or the past-the-end iterator if this iterator's node has no parent
        ///
        /// @return An iterator as described above
        iterator_impl parent() const
        {
            CPPTOOLS_DEBUG_ASSERT(_node != nullptr, "iterator", critical, "parent() called while pointing at no node", exception::internal::precondition_failure_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "parent() called on invalid iterator", exception::iterator::operation_while_invalid_error);

            return iterator_impl(_tree, _node->parent());
        }

        /// @brief Get an array of iterators to the immediate child nodes of
        /// this iterator's node
        ///
        /// @return An std::vector filled with iterators as described above
        std::vector<iterator_impl> children() const
        {
            CPPTOOLS_DEBUG_ASSERT(_node != nullptr, "iterator", critical, "children() called while pointing at no node", exception::internal::precondition_failure_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "children() called on invalid iterator", exception::iterator::operation_while_invalid_error);

            std::vector<iterator_impl> children_it;
            auto& children = _node->children();
            children_it.reserve(children.size());

            auto to_iterator = [this](node* n)
            {
                return iterator_impl(_tree, n);
            };

            // make an iterator out of every child node pointer
            std::transform(
                children.begin(), children.end(),
                std::back_inserter(children_it),
                to_iterator
            );

            return children_it;
        }

        /// @brief Get the count of immediate child nodes in this iterator's
        /// node
        ///
        /// @return The count as described above
        size_type children_count() const
        {
            CPPTOOLS_DEBUG_ASSERT(_node != nullptr, "iterator", critical, "children_count() called while pointing at no node", exception::internal::precondition_failure_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "children_count() called on invalid iterator", exception::iterator::operation_while_invalid_error);

            return _node->children().size();
        }

        /// @brief Tells whether the provided iterator's node is a parent (to
        /// any degree) of this iterator's node
        ///
        /// @tparam t2 Type of traversal implemented by the provided iterator
        ///
        /// @param node_it Iterator to the node to look for in this iterator's
        /// node's parent chain
        ///
        /// @return Whether the provided iterator's node is a parent of this
        /// iterator's node
        ///
        /// @note It is a precondition that both this iterator and the provided
        /// iterator point to non-null nodes when this function is called.
        template<traversal t2>
        bool has_parent(const iterator_impl<t2>& node_it) const
        {
            CPPTOOLS_DEBUG_ASSERT(_node != nullptr, "iterator", critical, "has_parent() called while pointing at no node", exception::internal::precondition_failure_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "has_parent() called on invalid iterator", exception::iterator::operation_while_invalid_error);
            CPPTOOLS_DEBUG_ASSERT(tree<T>::_node_from_iterator(node_it) != nullptr, "iterator", critical, "provided iterator points no node", exception::parameter::invalid_value_error, node_it);
            CPPTOOLS_DEBUG_ASSERT(node_it._is_valid(), "iterator", critical, "provided iterator is invalid", exception::iterator::operation_while_invalid_error);

            return _node->has_parent(tree<T>::_node_from_iterator(node_it));
        }

        /// @brief Tells whether this iterator's node is a parent (to any 
        /// degree) of the provided iterator's node
        ///
        /// @tparam t2 Type of traversal implemented by the provided iterator
        ///
        /// @param node_it Iterator to the node whose parent chain is to be
        /// checked
        ///
        /// @return Whether this iterator's node is a parent of the provided
        /// iterator's node
        ///
        /// @note It is a precondition that both this iterator and the provided
        /// iterator point to non-null nodes when this function is called.
        template<traversal t2>
        bool is_parent_of(const iterator_impl<t2>& node_it) const
        {
            CPPTOOLS_DEBUG_ASSERT(_node != nullptr, "iterator", critical, "is_parent_of() called while pointing at no node", exception::internal::precondition_failure_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "is_parent_of() called on invalid iterator", exception::iterator::operation_while_invalid_error);
            CPPTOOLS_DEBUG_ASSERT(tree<T>::_node_from_iterator(node_it) != nullptr, "iterator", critical, "provided iterator points no node", exception::parameter::invalid_value_error, node_it);
            CPPTOOLS_DEBUG_ASSERT(node_it._is_valid(), "iterator", critical, "provided iterator is invalid", exception::iterator::operation_while_invalid_error);

            return tree<T>::_node_from_iterator(node_it)->has_parent(_node);
        }
    };

    /// @brief Mutable iterator to an element in the tree. Allows bidirectional
    /// DFS traversal of the tree and insertion, copy and move operations on 
    /// node and branches.
    /// 
    /// @tparam t Type of traversal to be implemented by the iterator
    ///
    /// @note Invalidation rules are the same as that of const iterators.
    template<traversal t>
    class iterator : public iterator_impl<t>
    {
        friend tree;
        using this_base = iterator_impl<t>;

        /// @param tree Pointer to the tree being traversed
        /// @param node Pointer to the node on which the iteration starts
        iterator(tree* tree, node* node) : this_base(tree, node) {}

        // Forbid assignment from any const iterator
        template<traversal t2>
        this_base& operator=(iterator_impl<t2> other) = delete;

    public:
        using reference = tree::reference;

        iterator() : this_base() {}

        ~iterator() = default;

        /// @tparam t2 Type of traversal implemented by the provided iterator
        template<traversal t2>
        iterator(const iterator<t2>& other) : this_base(other) {}

        /// @tparam t2 Type of traversal implemented by the provided iterator
        template<traversal t2>
        iterator& operator=(iterator<t2> other)
        {
            swap(*this, other); // will use swap(iterator_impl&, iterator_impl&)

            return *this;
        }

        iterator& operator++()
        {
            this_base::operator++();
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp = *this;
            this_base::operator++();
            return tmp;
        }

        iterator& operator--()
        {
            this_base::operator--();
            return *this;
        }

        iterator operator--(int)
        {
            iterator tmp = *this;
            this_base::operator--();
            return tmp;
        }

        reference operator*() const
        {
            CPPTOOLS_DEBUG_ASSERT(this->_value != nullptr, "iterator", critical, "dereferenced while pointing at no node", exception::iterator::illegal_dereference_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "dereference on invalid iterator", exception::iterator::operation_while_invalid_error);

            return *(this->value);
        }

        pointer operator->() const
        {
            CPPTOOLS_DEBUG_ASSERT(this->_value != nullptr, "iterator", critical, "member accessed while pointing at no node", exception::iterator::illegal_dereference_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "member access on invalid iterator", exception::iterator::operation_while_invalid_error);

            return this->value;
        }

        /// @see iterator_impl::parent()
        iterator parent() const
        {
            CPPTOOLS_DEBUG_ASSERT(this->_node != nullptr, "iterator", critical, "parent() called while pointing at no node", exception::internal::precondition_failure_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "parent() called on invalid iterator", exception::iterator::operation_while_invalid_error);

            return iterator(this->_tree, this->_node->parent());
        }

        /// @see iterator_impl::children()
        std::vector<iterator> children() const
        {
            CPPTOOLS_DEBUG_ASSERT(this->_node != nullptr, "iterator", critical, "parent() called while pointing at no node", exception::internal::precondition_failure_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "parent() called on invalid iterator", exception::iterator::operation_while_invalid_error);

            std::vector<iterator> children_it;
            auto& children = this->_node->children();
            children_it.reserve(children.size());

            auto to_iterator = [this](node* n)
            {
                return iterator(this->_tree, n);
            };

            // make an iterator out of every child node pointer
            std::transform(
                children.begin(), children.end(),
                std::back_inserter(children_it),
                to_iterator
            );

            return children_it;
        }

        /// @brief Emplace a new value in the tree, as a new child node to the
        /// this iterator's node
        ///
        /// @tparam ArgTypes Types of the arguments to be forwarded to a
        /// constructor of the new value
        ///
        /// @param args Values of the arguments to be forwarded to a constructor
        /// of the new value
        ///
        /// @return An iterator to the newly inserted child node
        ///
        /// @exception Any exception thrown in the resulting constructor call of
        /// the value type will be let through to the caller
        ///
        /// @note If this iterator's node is null and ONLY IF this iterator's
        /// tree has no root, the newly created node will be emplaced as the 
        /// root of the tree.
        template<typename ...ArgTypes>
        iterator emplace_child(ArgTypes&&... args)
        {
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "emplace_child() called on invalid iterator", exception::iterator::operation_while_invalid_error);

            node* child = this->_tree->_make_node(std::forward<ArgTypes&&>(args)...);
            this->_tree->_perform_child_insertion(child, this->_node);

            return iterator(this->_tree, child);
        }

        /// @brief Move the branch whose root is this iterator's node, making it
        /// a child of the provided iterator's node
        ///
        /// @tparam t2 Type of traversal implemented by the provided iterator
        ///
        /// @param destination Iterator to the node where this iterator's branch
        /// should be moved to
        ///
        /// @note It is valid for the destination iterator's node to be located
        /// another tree instance which holds the same value type, in which case
        /// the branch will be moved across the trees.
        ///
        /// @note This iterator remains valid after the operation completes, and
        /// still points to the root node of the moved branch. All other 
        /// iterators to the node however become invalid.
        template<traversal t2>
        void move_as_child_to(const iterator<t2>& destination)
        {
            CPPTOOLS_DEBUG_ASSERT(this->_node != nullptr, "iterator", critical, "move_as_child_to() called while pointing at no node", exception::internal::precondition_failure_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "move_as_child_to() called on invalid iterator", exception::iterator::operation_while_invalid_error);
            CPPTOOLS_DEBUG_ASSERT(destination._is_valid(), "iterator", critical, "destination iterator is invalid", exception::parameter::invalid_value_error, "destination");

            tree* other  = tree<T>::_tree_from_iterator(destination);
            node* dest   = tree<T>::_node_from_iterator(destination);

            CPPTOOLS_DEBUG_ASSERT(dest != nullptr, "iterator", critical, "destination iterator points to no node", exception::parameter::invalid_value_error, "destination");

            this->_tree->_move_branch_across(this->_node, dest, other);
            this->_tree = other;
        }

        /// @brief Replicate the branch whose root is this iterator's node,
        /// copying the branch as a child of the provided iterator's node
        ///
        /// @tparam t2 Type of traversal implemented by the provided iterator
        ///
        /// @param destination Iterator to the node where this iterator's branch
        /// should be copied to
        ///
        /// @return An iterator to the root node of the copied branch
        ///
        /// @note It is valid for the destination iterator's node to be located
        /// another tree instance which holds the same value type, in which case
        /// the branch will be copied across the trees.
        template<traversal t2>
        iterator copy_as_child_to(const iterator<t2>& destination) const
        {
            CPPTOOLS_DEBUG_ASSERT(this->_node != nullptr, "iterator", critical, "move_as_child_to() called while pointing at no node", exception::internal::precondition_failure_error);
            CPPTOOLS_DEBUG_ASSERT(_is_valid(), "iterator", critical, "move_as_child_to() called on invalid iterator", exception::iterator::operation_while_invalid_error);
            CPPTOOLS_DEBUG_ASSERT(destination._is_valid(), "iterator", critical, "destination iterator is invalid", exception::parameter::invalid_value_error, "destination");

            tree* other  = tree<T>::_tree_from_iterator(destination);
            node* dest   = tree<T>::_node_from_iterator(destination);

            CPPTOOLS_DEBUG_ASSERT(dest != nullptr, "iterator", critical, "destination iterator points to no node", exception::parameter::invalid_value_error, "destination");

            node* new_branch = this->_tree->_copy_branch_across(this->_node, dest, other);
            return iterator(other, new_branch);
        }
    };

    /// @brief Constant iterator to an element in the tree. Allows bidirectional
    /// DFS traversal of the tree.
    /// 
    /// @tparam t Type of traversal to be implemented by the iterator
    ///
    /// @note Iterators are invalidated only when the node they point to is no
    /// longer part of the tree which it originally belonged to when the 
    /// iterator was first instantiated.
    template<traversal t>
    class const_iterator : public iterator_impl<t>
    {
        template<traversal t2>
        const_iterator(const iterator<t2>& other) : 
            iterator_impl<t2>(other)
        {
        }
    };

    template<traversal t = default_traversal>
    class reverse_iterator : public std::reverse_iterator<iterator<t>> {};

    template<traversal t = default_traversal>
    class const_reverse_iterator : public std::reverse_iterator<const_iterator<t>> {};

private:
    /// @brief Structure mapping node pointers to their node IDs
    storage_type _nodes;

    /// @brief Pointer to the root node of the tree
    node* _root;

    /// @brief Pointer to the leftmost node of the tree
    node* _leftmost;

    /// @brief Pointer to the rightmost node of the tree
    node* _rightmost;

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
    [[nodiscard]] node* _make_node(ArgTypes&&... args)
    {
        node* new_node = new node(std::forward<ArgTypes&&>(args)...);
#if CPPTOOLS_DEBUG_TREE != 0
        new_node->tree = this;
#endif
        _nodes[new_node] = node_ptr(new_node);

        return new_node;
    }

    /// @brief Correctly inserts a node as a child to a destination node
    ///
    /// @param child Pointer to the node to insert
    /// @param destination Pointer to the node where child should be inserted
    ///
    /// @note If destination is null and ONLY IF this tree's root is null,
    /// the inserted node will be made the root of this tree.
    void _perform_child_insertion(node* child, node* destination)
    {
        CPPTOOLS_DEBUG_ASSERT(child != nullptr, "tree", critical, "cannot insert null child", exception::parameter::null_parameter_error, "child", child);
        CPPTOOLS_DEBUG_ASSERT(destination != nullptr, "tree", critical, "cannot insert child at null destination", exception::parameter::null_parameter_error, "destination", destination);
        CPPTOOLS_DEBUG_ASSERT(_nodes.find(destination) != _nodes.end(), "tree", critical, "cannot insert child in destination not part of tree", exception::parameter::invalid_value_error, "destination", destination);

        if (!destination && !_root) [[unlikely]]
        {
            // insert tree root
            _root = child;
            _root->propagate_parent_chain_update();
        }
        else
        {
            // insert child node
            destination->insert_child(child);
        }
    }

    /// @brief Destruct a node as well as all of its children (recursive)
    ///
    /// @param root Pointer to the root node of the branch
    void _destruct_branch(node* root)
    {
        CPPTOOLS_DEBUG_ASSERT(root != nullptr, "tree", critical, "branch to destruct is null", exception::parameter::null_parameter_error, "root", root);
        CPPTOOLS_DEBUG_ASSERT(_nodes.find(root) != _nodes.end(), "tree", critical, "branch to destruct is not part of tree", exception::parameter::invalid_value_error, "root", root);

        // recursively destruct branches from children nodes
        for (auto child : root->children())
        {
            _destruct_branch(child);
        }

        // delete current node
        _nodes.erase(root);
    }

    /// @brief Move the set of nodes that make up a branch to another tree
    /// instance
    ///
    /// @tparam t Type of traversal implemented by the provided iterator
    ///
    /// @param root Pointer to the root of the branch whose nodes should be
    /// copied
    /// @param destination Pointer to a node in the tree where the branch
    /// should be moved
    /// @param other Pointer to the tree that owns the destination node
    ///
    /// @return A pointer to the root of the newly copied branch
    node* _copy_branch_across(node* root, node* destination, tree* other)
    {
        CPPTOOLS_DEBUG_ASSERT(root != nullptr, "tree", critical, "cannot copy null branch", exception::parameter::null_parameter_error, "root", root);
        CPPTOOLS_DEBUG_ASSERT(destination != nullptr, "tree", critical, "cannot copy branch at null destination", exception::parameter::null_parameter_error, "destination", destination);
        CPPTOOLS_DEBUG_ASSERT(other != nullptr, "tree", critical, "cannot copy branch to null tree", exception::parameter::null_parameter_error, "other", other);

        // copy root node
        node* new_root = other->_make_node(root->value);
        other->_perform_child_insertion(new_root, destination);

        // init recursion
        if (destination->has_parent(root)) [[unlikely]]
            other->_copy_branch_recursion_guard(root, new_root, root);
        else
            other->_copy_branch_recursive(root, new_root);

        // update book-keeping
        if (destination == other->_leftmost) other->_leftmost = new_root->leftmost_child_or_this();
        if (destination == other->_rightmost) other->_rightmost = new_root->rightmost_child_or_this();

        return new_root;
    }

    /// @brief Given a source root node and a destination root node, recursively
    /// copy all source children nodes as children of the destination node
    ///
    /// @param source Pointer to the node from which to start copying
    /// @param destination Pointer to the node in which the copied nodes should
    /// be inserted
    void _copy_branch_recursive(node* source, node* destination)
    {
        CPPTOOLS_DEBUG_ASSERT(source != nullptr, "tree", critical, "cannot recurse-copy null branch", exception::parameter::null_parameter_error, "source", source);
        CPPTOOLS_DEBUG_ASSERT(destination != nullptr, "tree", critical, "cannot recurse-copy branch at null destination", exception::parameter::null_parameter_error, "destination", destination);
        CPPTOOLS_DEBUG_ASSERT(!destination->has_parent(source), "tree", critical, "recursive copy initiated on destination contained by source", exception::parameter::invalid_value_error, "destination", destination);

        for (auto child : source->children())
        {
            // make new child node holding a copy of the source value
            node* copied = _make_node(child->value);

            // insert copied child into destination
            destination->insert_child(copied);

            // recurse copy
            if (!child->children().empty())
                _copy_branch_recursive(child, copied);
        }
    }

    /// @brief Given a source root node and a destination root node, recursively
    /// copy all source children nodes as children of the destination node,
    /// making sure not to fall into an infinite recursion of copying the newly
    /// copied nodes, which can happen when the destination node is a child of
    /// the source node
    ///
    /// @param source Pointer to the node from which to start copying
    /// @param destination Pointer to the node to which to append the copied
    /// nodes
    /// @param exclude Pointer to a node that should be excluded from the 
    /// copying process
    bool _copy_branch_recursion_guard(node* source, node* destination, node* exclude)
    {
        CPPTOOLS_DEBUG_ASSERT(source != nullptr, "tree", critical, "cannot recurse-copy null branch", exception::parameter::null_parameter_error, "source", source);
        CPPTOOLS_DEBUG_ASSERT(destination != nullptr, "tree", critical, "cannot recurse-copy branch at null destination", exception::parameter::null_parameter_error, "destination", destination);
        CPPTOOLS_DEBUG_ASSERT(exclude != nullptr, "tree", pedantic, "excluded node is null, use _copy_branch_recursive instead", exception::parameter::null_parameter_error, "exclude", exclude);

        bool excluded_node_traversed = false;

        for (auto child : source->children())
        {
            if (child == exclude)
            {
                excluded_node_traversed = true;
                continue;
            }

            node* copied = _make_node(source->value);
            destination->insert_child(copied);

            // recursively copy source children into copied node
            if (excluded_node_traversed)
                _copy_branch_recursive(child, copied);
            else
                excluded_node_traversed = _copy_branch_recursion_guard(child, copied, exclude);
        }

        return excluded_node_traversed;
    }

    /// @brief Attach a branch to a new parent node
    ///
    /// @tparam t Type of traversal implemented by the provided iterator
    ///
    /// @param root Pointer to the root of the branch whose nodes should be
    /// moved
    /// @param destination Pointer to a node in the tree where the branch
    /// should be moved
    /// @param other Pointer to the tree that owns the destination node
    void _move_branch_across(node* root, node* destination, tree* other)
    {
        CPPTOOLS_DEBUG_ASSERT(root != nullptr, "tree", critical, "cannot move null branch", exception::parameter::null_parameter_error, "root", root);
        CPPTOOLS_DEBUG_ASSERT(destination != nullptr, "tree", critical, "cannot move branch at null destination", exception::parameter::null_parameter_error, "destination", destination);
        CPPTOOLS_DEBUG_ASSERT(other != nullptr, "tree", critical, "cannot move branch to null tree", exception::parameter::null_parameter_error, "other", other);

        node* parent = root->parent();

        if (parent == destination) return;

        // to be used for book-keeping update
        bool dropping_leftmost  = (root == _leftmost)  || (_leftmost->has_parent(root));
        bool dropping_rightmost = (root == _rightmost) || (_rightmost->has_parent(root));

        // to be used for book-keeping update
        bool moving_as_new_rightmost = (destination == other->_rightmost);
        bool moving_as_new_leftmost  = (destination == other->_leftmost)
            && (!destination || destination->children().empty());

        if (parent) parent->remove_child(root);
        _perform_child_insertion(root, destination);

        if (this != other)
        {
            // move node across different storages
            _move_node_ptrs_recursive(root, other);

            // conditionally update book-keeping
            if (moving_as_new_leftmost)
            {
                other->_leftmost = dropping_leftmost
                    ? _leftmost
                    : root->leftmost_child_or_this();
            }
            if (moving_as_new_rightmost)
            {
                other->_rightmost = dropping_rightmost
                    ? _rightmost
                    : root->rightmost_child_or_this();
            }
        }
        else
        {
            // conditionally update book-keeping
            if (moving_as_new_leftmost)  _leftmost  = root->leftmost_child_or_this();
            if (moving_as_new_rightmost) _rightmost = root->rightmost_child_or_this();
        }

        // conditionally update book-keeping
        if (dropping_leftmost) _leftmost = parent->leftmost_child_or_this();
        if (dropping_rightmost) _rightmost = parent;
    }

    /// @brief Move the pointers to all nodes of a branch from this tree to
    /// another tree
    ///
    /// @param root Pointer to the root of the branch whose node should be moved
    /// @param destination Pointer to the tree where the branch should be moved
    void _move_node_ptrs_recursive(node* root, tree* destination)
    {
        CPPTOOLS_DEBUG_ASSERT(root != nullptr, "tree", critical, "cannot move nodes of null branch", exception::parameter::null_parameter_error, "root", root);
        CPPTOOLS_DEBUG_ASSERT(destination != nullptr, "tree", critical, "cannot move nodes of branch to null tree", exception::parameter::null_parameter_error, "destination", destination);

        auto map_node = _nodes.extract(root);
        destination->_nodes.insert(map_node);

        for (auto child : root->children())
        {
            _move_node_ptrs_recursive(child, destination);
        }
    }

    /// @brief Returns the tree pointer from an iterator
    ///
    /// @tparam t Type of traversal implemented by the provided iterator
    ///
    /// @param it Iterator whose tree pointer to get
    ///
    /// @return The tree pointer as described above
    template<traversal t>
    static tree*& _tree_from_iterator(const iterator_impl<t>& it)
    {
        return it._tree;
    }

#if CPPTOOLS_DEBUG_TREE != 0
    /// @brief Returns the original tree pointer from an iterator
    template<traversal t>
    static tree*& _original_tree_from_iterator(const iterator_impl<t>& it)
    {
        return it._original_tree;
    }
#endif

    /// @brief Returns the node pointer from an iterator
    ///
    /// @param it Iterator whose node pointer to get
    ///
    /// @return The node pointer as described above
    template<traversal t>
    static node*& _node_from_iterator(const iterator_impl<t>& it)
    {
        return it._node;
    }

    /// @brief Returns the value pointer from an iterator
    ///
    /// @param it Iterator whose value pointer to get
    ///
    /// @return The value pointer as described above
    template<traversal t>
    static value_type*& _value_from_iterator(const iterator_impl<t>& it)
    {
        return it._value;
    }

public:
    tree() :
        _nodes(),
        _root(nullptr),
        _leftmost(nullptr),
        _rightmost(nullptr)
    {

    }

    /// @brief Constructs a tree containing a single value emplaced in a root
    /// node, constructed from the provided arguments
    ///
    /// @tparam ArgTypes Types of the arguments to be forwarded to a constructor
    /// of the new value
    ///
    /// @param args Values of the arguments to be forwarded to a constructor of
    /// the new value
    ///
    /// @exception Any exception thrown in a constructor of the value type
    /// will be forwarded to the caller
    template<typename ...ArgTypes>
    tree(ArgTypes&&... args) :
        _nodes(),
        _root(nullptr),
        _leftmost(nullptr),
        _rightmost(nullptr)
    {
        _root = _make_node(std::forward<ArgTypes&&>(args)...);
        _leftmost = _root;
        _rightmost = _root;
    }

    /// @param other Tree to copy-construct contents from
    ///
    /// @exception Any exception thrown in a constructor of the value type
    /// will be forwarded to the caller
    tree(const tree<T>& other) :
        _nodes(),
        _root(nullptr),
        _leftmost(nullptr),
        _rightmost(nullptr)
    {
        if (other._root)
        {
            // copy-construct root node value from other root node value
            _root = _make_node(other._root->value);

            // copy node structure
            if (!other._root->children().empty())
                _copy_branch_recursive(other._root, _root);

            _leftmost = _root->leftmost_child_or_this();
            _rightmost = _root->rightmost_child_or_this();
        }
    }

    /// @param other Tree to move-construct contents from
    tree(tree<T>&& other) :
        _nodes(std::move(other._nodes)),
        _root(other._root),
        _leftmost(other._leftmost),
        _rightmost(other._rightmost)
    {
        other._root = nullptr;
        other._leftmost = nullptr;
        other._rightmost = nullptr;
    }

    struct initializer
    {
        T value;
        std::initializer_list<initializer> child_initializers;

        initializer(T&& value, std::initializer_list<initializer> list = {}) :
            value(std::forward<T&&>(value)),
            child_initializers(list)
        {

        }
    };

private:
    void _fill_from_init_list(iterator<traversal::pre_order> dest, std::initializer_list<initializer> init_list)
    {
        for (auto init : init_list)
        {
            auto new_node_it = dest.emplace_child(std::move(init.value));

            if (init.child_initializers.size())
                _fill_from_init_list(new_node_it, init.child_initializers);
        }
    }

public:
    tree(initializer&& init) :
        tree(std::move(init.value))
    {
        _fill_from_init_list(begin<traversal::pre_order>(), init.child_initializers);
    }

    /// @param other Tree to copy-assign contents from
    tree<T>& operator=(const tree<T>& other)
    {
        tree<T> tmp{other};
        swap(*this, tmp);
    }

    /// @param other Tree to move-assign contents from
    tree<T>& operator=(tree<T>&& other)
    {
        _nodes.clear();

        _nodes = std::move(other._nodes);
        _root = other._root;
        _leftmost = other._leftmost;
        _rightmost = other._rightmost;
    }

    tree<T>& operator=(initializer&& init)
    {
        tree<T> temp(std::move(init));
        *this = std::move(temp);
    }

    /// @brief Get an iterator to the root node of the tree
    ///
    /// @return An iterator as described above
    /// 
    /// @note When the tree is empty and has no root, a call to the
    /// emplace_child member function of the returned iterator will emplace a
    /// value in the tree, as its root node. After such a first call to
    /// emplace_child, the iterator points to the newly created root node, and 
    /// subsequent calls to emplace_child will function normally, emplacing
    /// values as children to the root node.
    ///
    /// @note The returned iterator implements pre-order traversal of the tree
    /// in the forward direction.
    /// 
    /// @see [c][r]begin() template member functions for other kinds of 
    /// traversal through iteration
    iterator<traversal::pre_order> root()
    {
        return begin<traversal::pre_order>();
    }

    const_iterator<traversal::pre_order> root() const
    {
        return cbegin<traversal::pre_order>();
    }

    /// @brief Search the tree for a value in a given traversal order, and get
    /// an iterator to the first encountered node that contains the value, or a
    /// past-the-end iterator if the value could not be found
    ///
    /// @tparam t Type of traversal to be implemented by the returned iterator
    ///
    /// @param value Value to find within the tree
    ///
    /// @return An iterator as described above
    ///
    /// @note The search will start from the first node in the traversal order
    /// as indicated by the template parameters 
    /// @note The returned iterator implements traversal in the same order as
    /// that which was used for the search
    template<traversal t = default_traversal>
    iterator<t> find(const_reference value)
    {
        return find(value, begin<t>());
    }

    template<traversal t = default_traversal>
    const_iterator<t> find(const_reference value) const
    {
        return find(value, cbegin<t>());
    }

    /// @brief Search the tree for a value in a given traversal order and from a
    /// given starting node, and get an iterator to the first encountered node
    /// that contains the value, or a past-the-end iterator if the value could 
    /// not be found
    ///
    /// @tparam t Type of traversal to use for the search
    ///
    /// @param value Value to find within the tree
    /// @param start Iterator implementing traversal type t, from which the 
    /// search should start
    ///
    /// @return An iterator as described above
    ///
    /// @note The returned iterator implements traversal in the same order as
    /// that which was used for the search.
    template<traversal t>
    iterator<t> find(const_reference value, iterator<t> start)
    {
        CPPTOOLS_DEBUG_ASSERT(start._is_valid(), "tree", critical, "provided iterator is invalid", exception::parameter::invalid_value_error, "start");
        CPPTOOLS_DEBUG_ASSERT(start._tree == this, "tree", critical, "provided iterator points to different tree", exception::parameter::invalid_value_error, "start");
        CPPTOOLS_DEBUG_ASSERT(start._node != nullptr, "tree", pedantic, "provided iterator points to no node", exception::parameter::invalid_value_error, "start");

        auto end = this->end<t>();
        for (; start != end; ++start)
        {
            if (*start == value) return start;
        }
        return start;
    }

    template<traversal t>
    const_iterator<t> find(const_reference value, const_iterator<t> start) const
    {
        CPPTOOLS_DEBUG_ASSERT(start._is_valid(), "tree", critical, "provided iterator is invalid", exception::parameter::invalid_value_error, "start");
        CPPTOOLS_DEBUG_ASSERT(start._tree == this, "tree", critical, "provided iterator points to different tree", exception::parameter::invalid_value_error, "start");
        CPPTOOLS_DEBUG_ASSERT(start._node != nullptr, "tree", pedantic, "provided iterator points to no node", exception::parameter::invalid_value_error, "start");

        auto end = this->cend<t>();
        for (; start != end; ++start)
        {
            if (*start == value) return start;
        }
        return start;
    }

    /// @brief Destroys the node pointed to by an iterator, as well as all
    /// of its children (recursive)
    ///
    /// @tparam t Type of traversal to be implemented by the returned iterator
    ///
    /// @param to_erase Iterator to the node to erase
    ///
    /// @return An iterator to the next node in the traversal order indicated by
    /// template parameters
    template<traversal t>
    iterator<t> erase(iterator<t> to_erase)
    {
        CPPTOOLS_DEBUG_ASSERT(to_erase._is_valid(), "tree", critical, "provided iterator is invalid", exception::parameter::invalid_value_error, "to_erase");
        CPPTOOLS_DEBUG_ASSERT(to_erase._tree == this, "tree", critical, "provided iterator points to different tree", exception::parameter::invalid_value_error, "to_erase");
        CPPTOOLS_DEBUG_ASSERT(to_erase._node != nullptr, "tree", critical, "provided iterator points to no node", exception::parameter::invalid_value_error, "to_erase");

        node* erased = to_erase._node;
        bool dropping_leftmost  = (erased == _leftmost)  || (_leftmost->has_parent(erased));
        bool dropping_rightmost = (erased == _rightmost) || (_rightmost->has_parent(erased));

        // the children of the node must all be destructed first
        const std::vector<node*>& children = erased->children();
        for (auto child : children)
        {
            _destruct_branch(child);
        }
        children.clear();

        // this way, incrementing the iterator results in the expected behaviour
        // no matter the traversal or direction
        ++to_erase;

        // the actual node can be deleted afterwards
        node* parent = erased->parent();
        if (parent) parent->remove_child(erased);

        _nodes.erase(erased);

        if (dropping_leftmost)  _leftmost  = parent;
        if (dropping_rightmost) _rightmost = parent;

        return to_erase;
    }

    /// @brief Check that this tree's structure and its ordered values are the
    /// same as that of another tree
    ///
    /// @param rhs Other tree to compare this tree to
    ///
    /// @return Whether this tree and rhs are equal
    bool operator==(const tree<T>& other) const
    {
        if (&other == this) return true;

        if (_nodes.size() != other._nodes.size()) return false;

        iterator_impl this_it = cbegin();
        iterator_impl other_it = other.cbegin();

        while (this_it != cend() && other_it != other.cend())
        {
            // lazy-compare structure
            if (this_it.children_count() != other_it.children_count()) return false;

            // compare value
            if (*this_it != *other_it) return false;

            ++this_it;
            ++other_it;
        }

        // cend() reached at a different points for iterators
        if (this_it != cend() || other_it != other.cend())
        {
            return false;
        }

        return true;
    }

    /// @brief Check that this tree's structure or its ordered values are
    /// different from that of another tree
    ///
    /// @param rhs Other tree to compare this tree to
    ///
    /// @return Whether this tree and rhs are not equal
    bool operator!=(const tree<T>& rhs)
    {
        return !(*this == rhs);
    }

    /// @brief Swap two trees
    ///
    /// @param other Tree to swap content with
    /// @param other Other tree to swap content with
    friend void swap(tree<T>& lhs, tree<T>& rhs)
    {
        std::ranges::swap(lhs._nodes, rhs._nodes);
        std::ranges::swap(lhs._root, rhs._root);
        std::ranges::swap(lhs._leftmost, rhs._leftmost);
        std::ranges::swap(lhs._rightmost, rhs._rightmost);
    }

    /// @brief Get the number of elements in the tree
    ///
    /// @return The number of elements in the tree
    size_type size() const
    {
        return _nodes.size();
    }

    /// @brief Get the maximum number of elements there can be in the tree
    ///
    /// @return The maximum number of elements in the tree
    size_type max_size() const
    {
        return _nodes.max_size();
    }

    /// @brief Get whether or not the tree is empty
    ///
    /// @return Whether or not the tree is empty
    bool empty() const
    {
        return _nodes.empty();
    }

    /// @brief Remove all nodes in the tree, including its root
    void clear()
    {
        _nodes.clear();
        _root = nullptr;
    }

    using difference_type =
        typename iterator<default_traversal>::difference_type;

    /// @brief Get an iterator to the first node in a traversal of the tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    iterator<t> begin()
    {
        if constexpr (t == traversal::pre_order)
        {
            return iterator<t>(this, _root);
        }
        else // if constexpr (t == traversal::post_order)
        {
            return iterator<t>(this, _leftmost);
        }
    }

    /// @brief Get the iterator past the end of a traversal of the tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    iterator<t> end()
    {
        return iterator<t>(this, nullptr);
    }

    /// @brief Get a const iterator to the first node in a traversal of the tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    const_iterator<t> cbegin() const
    {
        if constexpr (t == traversal::pre_order)
        {
            return const_iterator<t>(this, _root);
        }
        else // if constexpr (t == traversal::post_order)
        {
            return const_iterator<t>(this, _leftmost);
        }
    }


    /// @brief Get the const iterator past the end of a traversal of the tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    const_iterator<t> cend() const
    {
        return const_iterator<t>(this, nullptr);
    }

    /// @brief Get a reverse iterator to the first node in a traversal of the
    /// tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    reverse_iterator<t> rbegin()
    {
        return reverse_iterator<t>(end<t>());
    }

    /// @brief Get the iterator past the end of a traversal of the tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    reverse_iterator<t> rend()
    {
        return reverse_iterator<t>(begin<t>());
    }

    /// @brief Get a reverse const iterator to the first node in a traversal of 
    /// the tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    const_reverse_iterator<t> crbegin() const
    {
        return const_reverse_iterator<t>(cend<t>());
    }

    /// @brief Get the const iterator past the end of a traversal of the tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    const_reverse_iterator<t> crend() const
    {
        return const_reverse_iterator<t>(cbegin<t>());
    }
};

} // namespace tools::container

#if CPPTOOLS_DEBUG_TREE != 0
#   undef CPPTOOLS_DEBUG_ASSERT
#endif

// TODO: explore cache friendliness:
//  - have the nodes store only the value, make node storage a vector
//  - make a new struct of node metadata
//  - store node metadata entries in a map, associated to their nodes by iterators to them
// TODO: mark everything conditionally no except
// TODO: is it overkill to switch recursion method in _copy_branch_recursion_guard?

#endif//TOOLS__CONTAINER__TREE_HPP