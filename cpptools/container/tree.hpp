#ifndef TOOLS__CONTAINER__TREE_HPP
#define TOOLS__CONTAINER__TREE_HPP

#include <algorithm>
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

namespace tools
{

enum class tree_traversal
{
    post_order, // L R N
    pre_order   // N L R
};

enum class tree_traversal_direction
{
    forward,
    reverse
};

/// @brief An arbitrary tree. STL-compatible.
///
/// @tparam T Type of values to be stored
template<typename T>
class tree
{
public:
    using traversal = tree_traversal;
    using direction = tree_traversal_direction;

private:
    static constexpr traversal default_traversal = traversal::pre_order;

public:
    using value_type        = T;
    using reference         = value_type&;
    using const_reference   = const value_type&;

private:
    template<
        bool is_const,
        traversal t = default_traversal,
        direction d = direction::forward
    >
    class iterator_base;

    template<bool, traversal, direction>
    friend class iterator_base;

    /// @brief A node in an arbitrary tree.
    ///
    /// @note Member functions do not perform safety checks on provided
    /// arguments, AT ALL. Careless use may result in disaster.
    class node
    {
    public:
        using id_type = size_t;
        using size_type = typename std::vector<node*>::size_type;
        
        /// @brief Unique ID of this node
        const id_type id;

        /// @brief Value attached to this node
        T value;

    private:
        node(const node& other) = delete;
        node& operator=(const node& other) = delete;

        /// @brief Counts how many nodes were created (generates unique IDs)
        static id_type _count;

        /// @brief Pointer to the parent of this node
        node* _parent;

        /// @brief Index of this node in its parent's vector of children nodes
        size_type _parent_index;

        /// @brief Pointers to the children of this node
        std::vector<node*> _children;

        /// @brief Pointers to successive parent nodes
        std::vector<node*> _parent_chain;

    public:
        /// @param value Value to store in this node
        node(tree::value_type&& value);

        /// @note The destructor does not need to do anything in regards to the
        /// book-keeping of its parent and sibling nodes, since the tree itself
        /// takes care of this, for the precise reason that it makes the node 
        /// destruction process trivial.
        /// A node being destructed means that its branch has been cut off from
        /// the tree instance that owns it, and it is not being moved around in
        /// other trees: it is simply pruned. In so doing, the tree releases the
        /// unique_ptrs to the nodes of the branch. Only some nodes need their
        /// book-keeping updated in this context, and it is the parent node of
        /// the branch being deleted, as well as the sibling nodes of the branch
        /// root. 
        ~node() = default;

        /// @brief Get a pointer to the leftmost child node of this node (to 
        /// any degree)
        ///
        /// @return A pointer as described above
        node* leftmost_child() const;

        /// @brief Get a pointer to the rightmost child node of this node (to 
        /// any degree)
        ///
        /// @return A pointer as described above
        node* rightmost_child() const;

        /// @brief Get a pointer to the next sibling node from this one, as in
        /// a pre-order traversal of the tree
        ///
        /// @return A pointer as described above
        node* next_pre_order_sibling() const;

        /// @brief Get a pointer to the previous sibling node from this one, as
        /// in a pre-order traversal of the tree
        ///
        /// @return A pointer as described above
        node* previous_pre_order_sibling() const;

        /// @brief Get a pointer to the next sibling node from this one, as in
        /// a post-order traversal of the tree
        ///
        /// @return A pointer as described above
        node* next_post_order_sibling() const;

        /// @brief Get a pointer to the previous sibling node from this one, as
        /// in a post-order traversal of the tree
        ///
        /// @return A pointer as described above
        node* previous_post_order_sibling() const;

        /// @brief Append a new child node to this node
        ///
        /// @param child Pointer to the child node to be removed
        ///
        /// @note As part of this operation, the internal book-keeping of the
        /// child node will be entirely taken care of.
        void insert_child(node* child);

        /// @brief Recursively update _parent_chain in all of the children nodes
        /// of this node
        ///
        /// @param self_update Whether or not to also update this node's own
        /// parent chain
        ///
        /// @note It is a precondition that this node has a non-null parent
        /// registered before this function is called with self_update set to 
        /// true.
        void propagate_parent_chain_generation(bool self_update = true);

        /// @brief Remove a child from this node
        ///
        /// @param child Pointer to the child node to be removed
        ///
        /// @note As part of this operation, the internal book-keeping of the
        /// child node will also be taken care of.
        void remove_child(node* child);

        /// @brief Whether this node is a child (to any degree) of a certain
        /// node
        bool has_parent(node* parent) const;

        /// @brief Get pointers to the children of this node
        ///
        /// @return A vector of pointers to children nodes
        std::vector<node*>& children();

        /// @brief Get a pointer to the parent of this node
        ///
        /// @return A pointer to the parent node
        node* parent();
    };
    using node_ptr          = std::unique_ptr<node>;

public:
    using key_type          = typename node::id_type;
    using storage_type      = std::map<key_type, node_ptr>;
    using size_type         = typename storage_type::size_type;
    using pointer           = typename storage_type::pointer;
    using const_pointer     = typename storage_type::const_pointer;
    using key_compare       = typename storage_type::key_compare;
    using value_compare     = typename storage_type::value_compare;
    using allocator_type    = typename storage_type::allocator_type;

private:
    /// @brief Iterator to an element in the tree. Allows bidirectional DFS
    /// traversal of the tree.
    /// 
    /// @tparam is_const Whether or not this base class is a const iterator
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @tparam d Direction of traversal to be implemented by the iterator
    ///
    /// @note Several distinct combinations of template arguments may result in
    /// different iterator types implementing the same traversal order.
    ///
    /// @note Iterators are invalidated only when the node they point to is no 
    /// longer part of a tree.
    ///
    /// @note All iterators returned by the member functions of this class
    /// implement traversal in the same order and direction as those of the
    /// original iterator instance on which such member functions were called.
    template<bool is_const, traversal t, direction d>
    class iterator_base
    {
    friend class tree;

    public:
        using value_type        = tree::value_type;
        using reference         = std::conditional_t<
            is_const,
            tree::const_reference,
            tree::reference
        >;

        using const_reference   = tree::const_reference;
        using pointer           = tree::pointer;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;
        using size_type         = typename node::size_type;

    protected:
        /// @brief Pointer to the tree being traversed
        tree* _tree;

        /// @brief Pointer to the tree node currently being iterated
        node* _node;

        /// @brief Pointer to the value of the node currently being pointed to
        value_type* _value;

        /// @param node Pointer to the node to start iterating with
        /// @param tree Pointer to the tree being traversed
        iterator_base(node* node, tree* tree);

    public:
        iterator_base();
                        
        ~iterator_base() = default;

        /// @param other Iterator to copy contents from
        ///
        /// @tparam c2 Constness of the provided iterator
        /// @tparam t2 Type of traversal implemented by the provided iterator
        /// @tparam d2 Direction of traversal implemented by the provided
        /// iterator
        template<bool c2, traversal t2, direction d2>
        iterator_base(const iterator_base<c2, t2, d2>& other);

        /// @brief Standard swap between two iterators
        ///
        /// @tparam c2 Constness of the provided iterator
        /// @tparam t2 Type of traversal implemented by the provided iterator
        /// @tparam d2 Direction of traversal implemented by the provided
        /// iterator
        /// 
        /// @param lhs First iterator to swap
        /// @param rhs Second iterator to swap
        template<bool c2, traversal t2, direction d2>
        friend void swap(iterator_base& lhs, iterator_base<c2, t2, d2>& rhs);
        
        /// @param other Iterator to copy-assign contents from
        ///
        /// @tparam c2 Constness of the provided iterator
        /// @tparam t2 Type of traversal implemented by the provided iterator
        /// @tparam d2 Direction of traversal implemented by the provided
        /// iterator
        /// 
        /// @return This iterator
        template<bool c2, traversal t2, direction d2>
        iterator_base& operator=(iterator_base<c2, t2, d2> other);

        iterator_base& operator++();
        iterator_base operator++(int);
        iterator_base& operator--();
        iterator_base operator--(int);

        reference operator*();
        reference operator->();
        const_reference operator*() const;
        const_reference operator->() const;

        bool operator!=(const iterator_base& rhs) const;
        bool operator==(const iterator_base& rhs) const;

        /// @brief Get an iterator to the next element in the traversal order as
        /// indicated by class template parameters
        ///
        /// @return An iterator as described above
        iterator_base next() const;

        /// @brief Get an iterator to the previous element in the traversal
        /// order as indicated by class template parameters
        ///
        /// @return An iterator as described above
        iterator_base previous() const;

        /// @brief Get an iterator to the parent node of this iterator's node
        ///
        /// @return An iterator as described above
        iterator_base parent() const;

        /// @brief Get an array of iterators to the immediate children nodes of
        /// this iterator's node
        ///
        /// @return An std::vector filled with iterators as described above
        ///
        /// @note The iterators in the returned vector are ordered as per the
        /// traversal order indicated by class template parameters.
        std::vector<iterator_base> children() const;

        /// @brief Get the count of children nodes to this iterator's node
        ///
        /// @return The count as described above
        size_type children_count() const;

        /// @brief Tells whether this iterator's node is a parent (to any 
        /// degree) of the provided iterator's node
        ///
        /// @tparam c2 Constness of the provided iterator
        /// @tparam t2 Type of traversal implemented by the provided iterator
        /// @tparam d2 Direction of traversal implemented by the provided
        /// iterator
        ///
        /// @param node_it Iterator to the node whose parent chain is to be
        /// checked
        ///
        /// @return Whether this iterator's node is a parent of the provided
        /// iterator's node
        template<bool c2, traversal t2, direction d2>
        bool is_parent_of(const iterator_base<c2, t2, d2>& node_it) const;
    };

    template<traversal t, direction d>
    class mutable_iterator :
        public iterator_base<false /* is_const */, t, d>
    {
    friend class tree;

        using this_iterator_base = iterator_base<false /* is_const */, t, d>;

        template<traversal t2, direction d2>
        using any_iterator = typename tree::template mutable_iterator<t2, d2>;

        /// @param node Pointer to the node to start iterating with
        /// @param tree Pointer to the tree being traversed
        mutable_iterator(node* node, tree* tree);

        /// @brief Insert a node as a child to this iterator's node
        ///
        /// @param node Pointer to the new child
        void _insert_child(node* node);

        // Forbid assignment from any const iterator
        template<traversal t2, direction d2>
        this_iterator_base& operator=(
            iterator_base<true /* is_const */, t2, d2> other
        ) = delete;

    public:
        mutable_iterator();

        /// @tparam t2 Type of traversal implemented by the provided iterator
        /// @tparam d2 Direction of traversal implemented by the provided
        /// iterator
        template<traversal t2, direction d2>
        mutable_iterator(const mutable_iterator<t2, d2>& other);

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
        template<typename ...ArgTypes>
        mutable_iterator emplace_child(ArgTypes&&... args);

        /// @brief Move the branch whose root is this iterator's node, making it
        /// a child of the provided iterator's node
        ///
        /// @tparam t2 Type of traversal implemented by the provided iterator
        /// @tparam d2 Direction of traversal implemented by the provided 
        /// iterator
        ///
        /// @param destination Iterator to the node where this iterator's branch
        /// should be moved to
        ///
        /// @note It is valid for the destination iterator's node to be located
        /// another tree instance which holds the same value type, in which case
        /// the branch will be moved across the trees.
        ///
        /// @note This iterator remains valid after the operation completes, and
        /// still points to the root node of the moved branch.
        template<traversal t2, direction d2>
        void move_as_child_to(const any_iterator<t2, d2>& destination);

        /// @brief Replicate the branch whose root is this iterator's node,
        /// copying the branch as a child of the provided iterator's node
        ///
        /// @tparam t2 Type of traversal implemented by the provided iterator
        /// @tparam d2 Direction of traversal implemented by the provided 
        /// iterator
        ///
        /// @param destination Iterator to the node where this iterator's branch
        /// should be copied to
        ///
        /// @return An iterator to the root node of the copied branch
        ///
        /// @note It is valid for the destination iterator's node to be located
        /// another tree instance which holds the same value type, in which case
        /// the branch will be copied across the trees.
        template<traversal t2, direction d2>
        any_iterator<t2, d2> copy_as_child_to(
            any_iterator<t2, d2> destination
        ) const;

        operator this_iterator_base&() const;
    };

public:
    template<traversal t = default_traversal>
    using iterator                  = mutable_iterator<t, direction::forward>;

    template<traversal t = default_traversal>
    using reverse_iterator          = mutable_iterator<t, direction::reverse>;

    template<traversal t = default_traversal>
    using const_iterator =
        iterator_base<true /* is_const */, t, direction::forward>;

    template<traversal t = default_traversal>
    using const_reverse_iterator =
        iterator_base<true /* is_const */, t, direction::forward>;

private:
    /// @brief Structure mapping node pointers to their node IDs
    storage_type _nodes;

    /// @brief Pointer to the root node of the tree
    node* _root;
    
    /// @brief Pointer to the leftmost node of the tree
    node* _leftmost;
    
    /// @brief Pointer to the rightmost node of the tree
    node* _rightmost;
    
    /// @brief Destruct a node as well as all of its children (recursive)
    ///
    /// @param branchRoot Pointer to the root node of the branch
    void _destruct_branch(node* branchRoot);

    /// @brief Given a source root node and a destination root node, 
    /// recursively copy all source children nodes as children of the 
    /// destination node
    ///
    /// @param source Pointer to the node from which to start copying
    /// @param destination Pointer to the node to which to append the copied
    /// nodes
    void _copy_node_structure(node* source, node* destination);

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
    node* _make_node(ArgTypes&&... args);

    /// @brief Move the set of nodes that make up a branch to another tree
    /// instance
    ///
    /// @tparam t Type of traversal implemented by the provided iterator
    /// @tparam d Direction of traversal implemented by the provided iterator
    ///
    /// @param root Pointer to the root of the branch whose nodes should be
    /// copied
    /// @param destination Iterator to a node in the tree where the branch
    /// should be copied
    ///
    /// @return An iterator to the root of the newly copied branch
    template<traversal t, direction d>
    mutable_iterator<t, d> _copy_branch_across(
        node* root,
        const mutable_iterator<t, d>& destination
    );

    /// @brief Move the set of nodes that make up a branch to another tree
    /// instance
    ///
    /// @tparam t Type of traversal implemented by the provided iterator
    /// @tparam d Direction of traversal implemented by the provided iterator
    ///
    /// @param root Pointer to the root of the branch whose nodes should be
    /// moved
    /// @param destination Iterator to a node in the tree where the branch
    /// should be moved
    template<traversal t, direction d>
    void _move_branch_across(
        node* root,
        const mutable_iterator<t, d>& destination
    );

    /// @brief Move the pointers to all nodes of a branch from this tree to
    /// another tree
    ///
    /// @param root Pointer to the root of the branch whose node should be moved
    /// @param destination Pointer to the tree where the branch should be moved
    void _move_node_ptrs_recursive(node* root, tree* destination);

    /// @brief Returns an iterator that implements traversal according to 
    /// template parameters
    ///
    /// @tparam c Constness the returned iterator
    /// @tparam t Type of traversal implemented by the returned iterator
    /// @tparam d Direction of traversal implemented by the returned iterator
    ///
    /// @return An iterator as described above
    template<bool c, traversal t, direction d>
    iterator_base<c, t, d> _begin();

    /// @brief Returns the tree pointer from an iterator
    ///
    /// @tparam c Constness of the provided iterator
    /// @tparam t Type of traversal implemented by the provided iterator
    /// @tparam d Direction of traversal implemented by the provided iterator
    ///
    /// @param it Iterator whose tree pointer to get
    ///
    /// @return The tree pointer as described above
    template<bool c, traversal t, direction d>
    static tree* _tree_from_iterator(const iterator_base<c, t, d>& it);

    /// @brief Returns the node pointer from an iterator
    ///
    /// @tparam c Constness of the provided iterator
    /// @tparam t Type of traversal implemented by the provided iterator
    /// @tparam d Direction of traversal implemented by the provided iterator
    ///
    /// @param it Iterator whose node pointer to get
    ///
    /// @return The node pointer as described above
    template<bool c, traversal t, direction d>
    static node* _node_from_iterator(const iterator_base<c, t, d>& it);

    /// @brief Returns the value pointer from an iterator
    ///
    /// @tparam c Constness of the provided iterator
    /// @tparam t Type of traversal implemented by the provided iterator
    /// @tparam d Direction of traversal implemented by the provided iterator
    ///
    /// @param it Iterator whose value pointer to get
    ///
    /// @return The value pointer as described above
    template<bool c, traversal t, direction d>
    static value_type* _value_from_iterator(const iterator_base<c, t, d>& it);

public:
    tree();

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
    tree(ArgTypes&&... args);

    /// @param other Tree to copy contents from
    tree(const tree<T>& other);

    /// @param other Tree to move contents from
    tree(tree<T>&& other);

    /// @param other Tree to copy-assign contents from
    tree<T>& operator=(tree<T> other);

    /// @param other Tree to move-assign contents from
    tree<T>& operator=(tree<T>&& other);

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
    iterator<traversal::pre_order> root();
    const_iterator<traversal::pre_order> root() const;

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
    iterator<t> find(const_reference value);
    template<traversal t = default_traversal>
    const_iterator<t> find(const_reference value) const;

    /// @brief Search the tree for a value in a given traversal order and from a
    /// given starting node, and get an iterator to the first encountered node
    /// that contains the value, or a past-the-end iterator if the value could 
    /// not be found
    ///
    /// @tparam t2 Type of traversal implemented by the provided iterator
    /// @tparam t Type of traversal to use for the search
    ///
    /// @param value Value to find within the tree
    /// @param start Iterator from which the search should start
    ///
    /// @return An iterator as described above
    ///
    /// @note When not provided, the template parameters for the search order
    /// default to the traversal order implemented by the provided iterator.
    /// @note The returned iterator implements traversal in the same order as
    /// that which was used for the search.
    template<traversal t2, traversal t = t2>
    iterator<t> find(const_reference value, iterator<t2> start);
    template<traversal t2, traversal t = t2>
    const_iterator<t> find(
        const_reference value,
        const_iterator<t2> start
    ) const;

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
    iterator<t> erase(iterator<t> to_erase);

    /// @brief Check that this tree's structure and its ordered values are the
    /// same as that of another tree
    ///
    /// @param rhs Other tree to compare this tree to
    ///
    /// @return Whether this tree and rhs are equal
    bool operator==(const tree<T>& rhs);

    /// @brief Check that this tree's structure or its ordered values are
    /// different from that of another tree
    ///
    /// @param rhs Other tree to compare this tree to
    ///
    /// @return Whether this tree and rhs are not equal
    bool operator!=(const tree<T>& rhs);

    /// @brief Swap this tree with another
    ///
    /// @param other Tree to swap content with
    friend void swap(tree<T>& other);

    /// @brief Get the number of elements in the tree
    ///
    /// @return The number of elements in the tree
    size_type size() const;

    /// @brief Get the maximum number of elements there can be in the tree
    ///
    /// @return The maximum number of elements in the tree
    size_type max_size() const;

    /// @brief Get whether or not the tree is empty
    ///
    /// @return Whether or not the tree is empty
    bool empty() const;

    /// @brief Remove all nodes in the tree, including its root
    void clear();

public:
    using difference_type =
        typename iterator<traversal::pre_order>::difference_type;

    /// @brief Get an iterator to the first node in a traversal of the tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    iterator<t> begin();

    /// @brief Get the iterator past the end of a traversal of the tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    iterator<t> end();

    /// @brief Get a const iterator to the first node in a traversal of the tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    const_iterator<t> cbegin() const;

    /// @brief Get the const iterator past the end of a traversal of the tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    const_iterator<t> cend() const;

    /// @brief Get a reverse iterator to the first node in a traversal of the
    /// tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    reverse_iterator<t> rbegin();

    /// @brief Get the iterator past the end of a traversal of the tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    reverse_iterator<t> rend();

    /// @brief Get a reverse const iterator to the first node in a traversal of 
    /// the tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    const_reverse_iterator<t> crbegin() const;

    /// @brief Get the const iterator past the end of a traversal of the tree
    ///
    /// @tparam t Type of traversal to be implemented by the iterator
    /// @return An iterator as described above
    template<traversal t = default_traversal>
    const_reverse_iterator<t> crend() const;
};

template<
    typename T,
    bool c /* is_const */,
    tree_traversal t,
    tree_traversal_direction d
>
auto next_post_order(
    const typename tree<T>::template iterator_base<c, t, d>& it
) -> typename tree<T>::template iterator_base<c, t, d>
{

}

template<
    typename T,
    bool c /* is_const */,
    tree_traversal t,
    tree_traversal_direction d
>
auto previous_post_order(
    const typename tree<T>::template iterator_base<c, t, d>& it
) -> typename tree<T>::template iterator_base<c, t, d>
{

}

template<typename T>
tree<T>::tree() :
    _nodes(),
    _root(nullptr),
    _leftmost(nullptr),
    _rightmost(nullptr)
{

}

template<typename T>
template<typename... ArgTypes>
tree<T>::tree(ArgTypes&&... args) :
    _nodes(),
    _root(nullptr),
    _leftmost(nullptr),
    _rightmost(nullptr)
{
    _root = _make_node(std::forward(args)...);
    _leftmost = _root;
    _rightmost = _root;
}

template<typename T>
tree<T>::tree(const tree& other) :
    _nodes(),
    _root(nullptr),
    _leftmost(nullptr),
    _rightmost(nullptr)
{
    // copy-construct root node value from other root node value
    _root = _make_node(other._root->value);

    // replicate the node structure from the other root to this root, filling in
    // the node storage with the newly created nodes
    _copy_node_structure(other._root, _root);

    _leftmost = _root->leftmost_child();
    _rightmost = _root->rightmost_child();
}

template<typename T>
tree<T>::tree(tree&& other) :
    _nodes(std::move(other._nodes)),
    _root(other._root),
    _leftmost(other._leftmost),
    _rightmost(other._rightmost)
{

}

template<typename T>
tree<T>& tree<T>::operator=(tree<T> other)
{
    swap(*this, other);
}

template<typename T>
tree<T>& tree<T>::operator=(tree<T>&& other)
{
    _nodes.clear();

    _nodes = std::move(other._nodes);
    _root = other._root;
    _leftmost = other._leftmost;
    _rightmost = other._rightmost;
}

template<typename T>
auto tree<T>::root() -> iterator<traversal::pre_order>
{
    return begin<traversal::pre_order>();
}


template<typename T>
auto tree<T>::root() const
    -> const_iterator<traversal::pre_order>
{
    return cbegin<traversal::pre_order>();
}

template<typename T>
template<tree_traversal t>
auto tree<T>::find(const_reference value) -> iterator<t>
{
    return find(value, begin<t>());
}

template<typename T>
template<tree_traversal t>
auto tree<T>::find(const_reference value) const -> const_iterator<t>
{
    return find(value, cbegin<t>());
}

template<typename T>
template<tree_traversal t2, tree_traversal t>
auto tree<T>::find(const_reference value, iterator<t2> start)
    -> iterator<t>
{
    for (; start != end<t>(); ++start)
    {
        if (*start == value) return start;
    }
    return start;
}

template<typename T>
template<tree_traversal t2, tree_traversal t>
auto tree<T>::find(const_reference value, const_iterator<t2> start) const
    -> const_iterator<t>
{
    for (; start != cend<t>(); ++start)
    {
        if (*start == value) return start;
    }
    return start;
}

template<typename T>
template<tree_traversal t>
auto tree<T>::erase(iterator<t> to_erase) -> iterator<t>
{
    node* erased = to_erase._node;
    bool dropping_leftmost = _leftmost->has_parent(erased);
    bool dropping_rightmost = _rightmost->has_parent(erased);

    std::vector<node*>& children = erased->children();

    for (auto it = children.begin(); it != children.end(); ++it)
    {
        _destruct_branch(*it);
    }
    children.clear();
    
    auto next = to_erase.next();

    node* parent = erased->parent();
    parent->remove_child(erased);
    _nodes.erase(erased->id);

    if (dropping_leftmost)  _leftmost  = parent;
    if (dropping_rightmost) _rightmost = parent;

    return next;
}

template<typename T>
bool tree<T>::operator==(const tree<T>& rhs)
{
    if (&rhs == this) return true;

    if (_nodes.size() != rhs._nodes.size()) return false;

    const_iterator thisIt = cbegin();
    const_iterator otherIt = rhs.template cbegin();

    while (thisIt != cend() && otherIt != rhs.template cend())
    {
        // Lazy-compare structure
        if (thisIt.children_count() != otherIt.children_count()) return false;

        // Compare value
        if (*thisIt != *otherIt) return false;

        ++thisIt;
        ++otherIt;
    }

    // cend() reached at a different points for iterators
    if (thisIt != cend() || otherIt != rhs.template cend())
    {
        return false;
    }

    return true;
}

template<typename T>
bool tree<T>::operator!=(const tree<T>& rhs)
{
    return !(*this == rhs);
}

template<typename T, tree_traversal dt, tree_traversal dt2>
void swap(tree<T>& lhs, tree<T>& rhs)
{
    std::ranges::swap(lhs._nodes, rhs._nodes);
    std::ranges::swap(lhs._root, rhs._root);
    std::ranges::swap(lhs._leftmost, rhs._leftmost);
    std::ranges::swap(lhs._rightmost, rhs._rightmost);
}

template<typename T>
auto tree<T>::size() const -> size_type
{
    return _nodes.size();
}

template<typename T>
auto tree<T>::max_size() const -> size_type
{
    return _nodes.max_size();
}

template<typename T>
bool tree<T>::empty() const
{
    return _nodes.empty();
}

template<typename T>
void tree<T>::clear()
{
    _nodes.clear();
    _root = nullptr;
}

template<typename T>
template<tree_traversal t>
auto tree<T>::begin() -> iterator<t>
{
    return _begin<false /* is_const */, t, direction::forward>();
}

template<typename T>
template<tree_traversal t>
auto tree<T>::end() -> iterator<t>
{
    return iterator<t>(nullptr, this);
}

template<typename T>
template<tree_traversal t>
auto tree<T>::cbegin() const -> const_iterator<t>
{
    return _begin<true /* is_const */, t, direction::forward>();
}

template<typename T>
template<tree_traversal t>
auto tree<T>::cend() const -> const_iterator<t>
{
    return const_iterator<t>(nullptr, this);
}

template<typename T>
template<tree_traversal t>
auto tree<T>::rbegin() -> reverse_iterator<t>
{
    return _begin<false /* is_const */, t, direction::reverse>();
}

template<typename T>
template<tree_traversal t>
auto tree<T>::rend() -> reverse_iterator<t>
{
    return reverse_iterator<t>(nullptr, this);
}

template<typename T>
template<tree_traversal t>
auto tree<T>::crbegin() const -> const_reverse_iterator<t>
{
    return _begin<true /* is_const */, t, direction::reverse>();
}

template<typename T>
template<tree_traversal t>
auto tree<T>::crend() const -> const_reverse_iterator<t>
{
    return const_reverse_iterator<t>(nullptr, this);
}

template<typename T>
void tree<T>::_destruct_branch(node* root)
{
    // recursively destruct branches from children nodes
    std::vector<node_ptr> children = root->getChildren();
    for (auto it = children.begin(); it != children.end(); it++)
    {
        _destruct_branch(*it);
    }

    // delete current node
    _nodes.erase(root->id);
}

template<typename T>
void tree<T>::_copy_node_structure(node* source, node* destination)
{
    std::vector<node*>& sourceChildren = source->children();
    for (auto it = sourceChildren.begin(); it != sourceChildren.end(); it++)
    {
        // make new children nodes holding copies of the other nodes' values
        node* child = *it;
        node* new_child = new node(child->value, destination);
        _nodes[new_child->id] = node_ptr(new_child);

        // recursive copy through each newly created child
        _copy_node_structure(child, new_child);
    }
}

template<typename T>
template<typename... ArgTypes>
auto tree<T>::_make_node(ArgTypes&&... args) -> node*
{
    node* new_node = new node(std::forward(args)...);
    _nodes[new_node->id] = node_ptr(new_node);

    return new_node;
}

template<typename T>
template<tree_traversal t, tree_traversal_direction d>
auto tree<T>::_copy_branch_across(
    node* root,
    const mutable_iterator<t, d>& destination
) -> mutable_iterator<t, d>
{
    tree* other = destination._tree;
    node* dest  = destination._node;

    mutable_iterator<t, d> new_branch = destination.emplace_child(root->value);
    node* new_root = new_branch._node;

    other->_copy_node_structure(root, new_root);

    if (dest == other->_leftmost)
    {
        other->_leftmost = new_root->leftmost_child();
    }

    if (dest == other->_rightmost)
    {
        other->_rightmost = new_root->rightmost_child();
    }

    return new_branch;
}

template<typename T>
template<tree_traversal t, tree_traversal_direction d>
void tree<T>::_move_branch_across(
    node* root,
    const mutable_iterator<t, d>& destination
)
{
    if (root->parent() == destination._node) return;

    tree* other  = destination._tree;
    node* dest   = destination._node;
    node* parent = root->parent();

    bool dropping_leftmost = _leftmost->has_parent(root);
    bool dropping_rightmost = _rightmost->has_parent(root);

    if (parent)
    {
        parent->remove_child(root);
    }

    destination._insert_child(root);

    if (this == other)
    {
        if (dest == _leftmost)
        {
            _leftmost = root->leftmost_child();
        }

        if (dest == _rightmost)
        {
            _rightmost = root->rightmost_child();
        }
    }
    else // if (this != other)
    {
        _move_node_ptrs_recursive(root, other);

        if (dest == other->_leftmost)
        {
            other->_leftmost = dropping_leftmost
                ? _leftmost
                : root->leftmost_child();
        }

        if (dest == other->_rightmost)
        {
            other->_rightmost = dropping_rightmost
                ? _rightmost
                : root->rightmost_child();
        }
    }

    if (dropping_leftmost) _leftmost = parent;

    if (dropping_rightmost) _rightmost = parent;
}

template<typename T>
void tree<T>::_move_node_ptrs_recursive(node* root, tree* destination)
{
    auto map_node = _nodes.extract(root->id);
    destination->_nodes.insert(map_node);

    for (auto it = root->children().begin(); it != root->children().end(); it++)
    {
        _move_node_ptrs_recursive(*it, destination);
    }
}

template<typename T>
template<bool c, tree_traversal t, tree_traversal_direction d>
auto tree<T>::_begin() -> iterator_base<c, t, d>
{
    if constexpr (
        (t == traversal::pre_order  && d == direction::forward) ||
        (t == traversal::post_order && d == direction::reverse)
    )
    {
        return iterator_base<c, t, d>(_root, this);
    }
    else if constexpr ((t == traversal::post_order && d == direction::forward))
    {
        return iterator_base<c, t, d>(_leftmost, this);
    }
    else // if constexpr (t == traversal::pre_order && d == direction::reverse)
    {
        return iterator_base<c, t, d>(_rightmost, this);
    }
}

template<typename T>
template<bool c, tree_traversal t, tree_traversal_direction d>
auto tree<T>::_tree_from_iterator(const iterator_base<c, t, d>& it) -> tree*
{
    return it._tree;
}

template<typename T>
template<bool c, tree_traversal t, tree_traversal_direction d>
auto tree<T>::_node_from_iterator(const iterator_base<c, t, d>& it) -> node*
{
    return it._node;
}

template<typename T>
template<bool c, tree_traversal t, tree_traversal_direction d>
auto tree<T>::_value_from_iterator(const iterator_base<c, t, d>& it) -> value_type*
{
    return it._value;
}

template<typename T>
tree<T>::node::node(tree::value_type&& value) :
    id(_count++),
    value(std::move(value)),
    _parent(nullptr),
    _children(),
    _parent_index(),
    _parent_chain()
{

}

template<typename T>
auto tree<T>::node::leftmost_child() const -> node*
{
    node* root = this;
    while (!root->_children.empty())
    {
        root = root->_children.front();
    }

    return root;
}

template<typename T>
auto tree<T>::node::rightmost_child() const -> node*
{
    node* root = this;
    while (!root->_children.empty())
    {
        root = root->_children.back();
    }

    return root;
}

template<typename T>
auto tree<T>::node::next_pre_order_sibling() const -> node*
{
    if (_parent)
    {        
        auto& siblings = _parent->_children;
        return (_parent_index < (siblings->size() - 1))
            ? siblings[_parent_index + 1]
            : _parent->next_pre_order_sibling();
    }
    return nullptr;
}

template<typename T>
auto tree<T>::node::previous_pre_order_sibling() const -> node*
{
    if (_parent)
    {
        auto& siblings = _parent->_children;
        return (_parent_index > 0)
            ? siblings[_parent_index - 1]
            : _parent->previous_pre_order_sibling();
    }
    return nullptr;
}

template<typename T>
auto tree<T>::node::next_post_order_sibling() const -> node*
{
    if (_parent)
    {        
        auto& siblings = _parent->_children;
        return (_parent_index < (siblings->size() - 1))
            ? siblings[_parent_index + 1]
            : _parent->next_post_order_sibling();
    }
    return nullptr;
}

template<typename T>
auto tree<T>::node::previous_post_order_sibling() const -> node*
{
    if (_parent)
    {
        auto& siblings = _parent->_children;
        return (_parent_index > 0)
            ? siblings[_parent_index - 1]
            : _parent->previous_post_order_sibling();
    }
    return nullptr;
}

template<typename T>
void tree<T>::node::insert_child(node* child)
{
    child->_parent_index = _children.size();

    child->_parent = this;
    child->propagate_parent_chain_generation();
    _children.push_back(child);
}

template<typename T>
void tree<T>::node::propagate_parent_chain_generation(bool self_update)
{
    if (self_update)
    {
        _parent_chain.clear();
        _parent_chain.reserve(_parent->_parent_chain.size() + 1);
        _parent_chain.push_back(this);
        std::copy(
            _parent_chain.begin(),
            _parent_chain.end(),
            std::back_inserter(_parent_chain)
        );
    }

    for (auto it = _children.begin(); it != _children.end(); it++)
    {
        propagate_parent_chain_generation(true /* self_update */);
    }
}

template<typename T>
void tree<T>::node::remove_child(node* child)
{
    for (
        auto it = _children.erase(_children.begin() + child->_parent_index);
        it != _children.end();
        it++
    )
    {
        --((*it)->_parent_index);
    }
    
    child->_parent_index = 0;
    child->_parent_chain.clear();
    child->_parent = nullptr;
}

template<typename T>
bool tree<T>::node::has_parent(node* parent) const
{
    auto it = std::find(_parent_chain.begin(), _parent_chain.end(), parent);
    
    return it != _parent_chain.end();
}

template<typename T>
auto tree<T>::node::children() -> std::vector<node*>&
{
    return _children;
}

template<typename T>
auto tree<T>::node::parent() -> node*
{
    return _parent;
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
tree<T>::iterator_base<c, t, d>::iterator_base() :
    _tree(nullptr),
    _node(nullptr),
    _value(nullptr)
{

}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
template<bool c2 /* is_const */, tree_traversal t2, tree_traversal_direction d2>
tree<T>::iterator_base<c, t, d>::iterator_base(
    const iterator_base<c2, t2, d2>& other
) :
    _tree(_tree_from_iterator(other)),
    _node(_node_from_iterator(other)),
    _value(_value_from_iterator(other))
{

}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
tree<T>::iterator_base<c, t, d>::iterator_base(node* node, tree* tree) :
    _tree(tree),
    _node(node),
    _value(node ? &(node->value) : nullptr)
{

}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
template<bool c2 /* is_const */, tree_traversal t2, tree_traversal_direction d2>
auto tree<T>::iterator_base<c, t, d>::operator=(iterator_base<c2, t2, d2> other)
    -> iterator_base&
{
    swap(*this, other);

    return *this;
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
auto tree<T>::iterator_base<c, t, d>::operator++() -> iterator_base&
{
    *this = next();
    return *this;
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
auto tree<T>::iterator_base<c, t, d>::operator++(int _) -> iterator_base
{
    iterator_base tmp = *this;
    *this = next();
    return tmp;
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
auto tree<T>::iterator_base<c, t, d>::operator--() -> iterator_base&
{
    *this = previous();
    return *this;
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
auto tree<T>::iterator_base<c, t, d>::operator--(int _) -> iterator_base
{
    iterator_base tmp = *this;
    *this = previous();
    return tmp;
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
auto tree<T>::iterator_base<c, t, d>::operator*() -> reference
{
    return *(_node->value);
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
auto tree<T>::iterator_base<c, t, d>::operator->() -> reference
{
    return *(_node->value);
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
auto tree<T>::iterator_base<c, t, d>::operator*() const -> const_reference
{
    return *(_node->value);
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
auto tree<T>::iterator_base<c, t, d>::operator->() const -> const_reference
{
    return *(_node->value);
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
bool tree<T>::iterator_base<c, t, d>::operator==(const iterator_base& rhs) const
{
    return
        (_node == rhs._node) &&
        (_tree == rhs._tree);
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
bool tree<T>::iterator_base<c, t, d>::operator!=(const iterator_base& rhs) const
{
    return !(operator==(rhs));
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
auto tree<T>::iterator_base<c, t, d>::next() const -> iterator_base
{
    if constexpr (d == direction::forward)
    {
        if constexpr (t == traversal::pre_order)
        {
            return next_pre_order(_tree, this);
        }
        else // if constexpr (t == traversal::post_order)
        {
            return next_post_order(_tree, this);
        }
    }
    else // if constexpr (d == direction::reverse)
    {
        if constexpr (t == traversal::pre_order)
        {
            return previous_pre_order(_tree, this);
        }
        else // if constexpr (t == traversal::post_order)
        {
            return previous_post_order(_tree, this);
        }
    }
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
auto tree<T>::iterator_base<c, t, d>::previous() const -> iterator_base
{
    if constexpr (d == direction::forward)
    {
        if constexpr (t == traversal::pre_order)
        {
            return previous_pre_order(this);
        }
        else // if constexpr (t == traversal::post_order)
        {
            return previous_post_order(this);
        }
    }
    else // if constexpr (d == direction::reverse)
    {
        if constexpr (t == traversal::pre_order)
        {
            return next_pre_order(this);
        }
        else // if constexpr (t == traversal::post_order)
        {
            return next_post_order(this);
        }
    }
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
auto tree<T>::iterator_base<c, t, d>::parent() const -> iterator_base 
{
    return iterator_base(_node->parent(), _tree);
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
auto tree<T>::iterator_base<c, t, d>::children() const
    -> std::vector<iterator_base> 
{
    std::vector<iterator_base> children_it;
    auto& children = _node->children();
    children_it.reserve(children.size());

    auto to_iterator = [this](node* n)
    {
        return iterator_base(n, _tree);
    };

    if constexpr (d == direction::forward)
    {
        std::transform(
            _node->children().begin(),
            _node->children().end(),
            std::back_inserter(children),
            to_iterator
        );
    }
    else // if constexpr (d == direction::reverse)
    {
        std::transform(
            _node->children().rbegin(),
            _node->children().rend(),
            std::back_inserter(children),
            to_iterator
        );
    }

    return children;
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
auto tree<T>::iterator_base<c, t, d>::children_count() const -> size_type 
{
    return _node->children().size();
}

template<typename T>
template<bool c /* is_const */, tree_traversal t, tree_traversal_direction d>
template<bool c2 /* is_const */, tree_traversal t2, tree_traversal_direction d2>
bool tree<T>::iterator_base<c, t, d>::is_parent_of(
    const iterator_base<c2, t2, d2>& node_it
) const
{
    return tree<T>::_node_from_iterator(node_it)->has_parent(_node);
}

template<typename T>
template<tree_traversal t, tree_traversal_direction d>
tree<T>::mutable_iterator<t, d>::mutable_iterator() : this_iterator_base()
{

}

template<typename T>
template<tree_traversal t, tree_traversal_direction d>
tree<T>::mutable_iterator<t, d>::mutable_iterator(node* node, tree* tree) :
    this_iterator_base(node, tree)
{

}

template<typename T>
template<tree_traversal t, tree_traversal_direction d>
template<typename ...ArgTypes>
auto tree<T>::mutable_iterator<t, d>::emplace_child(ArgTypes&&... args)
    -> mutable_iterator 
{
    node* child = this->_tree->_make_node(std::forward(args)...);
    _insert_child(child);
    return mutable_iterator(child, this->_tree);
}

template<typename T>
template<tree_traversal t, tree_traversal_direction d>
template<tree_traversal t2, tree_traversal_direction d2>
void tree<T>::mutable_iterator<t, d>::move_as_child_to(
    const any_iterator<t2, d2>& destination
)
{
    this->_tree->_move_branch_across(this->_node, destination);
    this->_tree = tree<T>::_tree_from_iterator(destination);
}

template<typename T>
template<tree_traversal t, tree_traversal_direction d>
template<tree_traversal t2, tree_traversal_direction d2>
auto tree<T>::mutable_iterator<t, d>::copy_as_child_to(
    any_iterator<t2, d2> destination
) const -> any_iterator<t2, d2>
{
    return this->_tree->_copy_branch_across(this->_node, destination);
}

template<typename T>
template<tree_traversal t, tree_traversal_direction d>
tree<T>::mutable_iterator<t, d>::operator this_iterator_base&() const
{
    return static_cast<this_iterator_base&>(*this);
}

template<typename T>
template<tree_traversal t, tree_traversal_direction d>
void tree<T>::mutable_iterator<t, d>::_insert_child(node* child)
{
    if (!this->_node && !this->_tree->_root)
    {
        this->_tree->_root = child;
        child->propagate_parent_chain_generation(false /* self_update */);
    }
    else
    {
        this->_node->insert_child(child);
    }
}

template<
    typename T,
    bool c,  tree_traversal t,  tree_traversal_direction d,
    bool c2, tree_traversal t2, tree_traversal_direction d2
>
void swap
(
    typename tree<T>::template iterator_base<c,  t,  d>& lhs,
    typename tree<T>::template iterator_base<c2, t2, d2>& rhs
)
{
    std::ranges::swap(lhs._node, rhs._node);
    std::ranges::swap(lhs._tree, rhs._tree);
    std::ranges::swap(lhs._value, rhs._value);
}

} // namespace tools

// TODO: equivalent reverse traversals of different types for iterator_bases
// TODO: debug assertions
// TODO: reversible iterators
// TODO: initializer list construction (proxy object needed?)
// TODO: explore cache-friendliness:
//  - have the nodes store only the value, make node storage a vector
//  - make a new struct of node metadata
//  - store node metadata entries in a map, associated to their nodes by iterators to them
// TODO: node::insert_child makes no guarantees in case of an exception
// TODO: make mutable_iterator::insert_child branchless
// TODO: tree::erase would be cleaner if it was possible to _destruct_branch directly from iterated node
// TODO: ensure what happens when a branch is being copied to a node which is part of it already
// TODO: check for move at same location
// TODO: figure out casting between mutable and const iterators, how it plays out in next() and previous()
// TODO: investigate removal of mixed usage of iterators and nodes
// TODO: explanatory comments
// TODO: impl of traversal cannot access tree private type iterator_base and is not friend of them
// TODO: change iterator_base(node*, tree*) to iterator_base(tree*, node*)
// TODO: remove tree_node.hpp
// TODO: Ctrl+F "TODO"

#endif//TOOLS__CONTAINER__TREE_HPP