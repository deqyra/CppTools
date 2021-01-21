#ifndef CPPTOOLS__TREE_HPP
#define CPPTOOLS__TREE_HPP

#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "tree_node.hpp"

namespace CppTools
{

/// @brief A tree managing nodes organized in a hierarchical structure, in which
/// one node may have several children but only one parent. Fully STL-compatible.
///
/// @tparam T The type of elements which the tree will store.
template<typename T>
class Tree : public std::enable_shared_from_this<Tree<T>>
{
public:
    using Node = TreeNode<T>;
    using NodePtr = std::shared_ptr<Node>;
    using NodeWPtr = std::weak_ptr<Node>;

    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::unordered_map<unsigned int, NodePtr>::size_type;

private:
    /// @brief Pointer to the root node of the tree.
    NodePtr _root;
    
    /// @brief Structure mapping node pointers to their node IDs.
    std::unordered_map<unsigned int, NodePtr> _nodes;

    /// @param root Pointer to the root node.
    Tree(NodePtr root);

    /// @param root Pointer to the root node.
    /// @param nodeMap Pre-filled map containing pointers to all tree nodes.
    Tree(NodePtr root, std::unordered_map<unsigned int, NodePtr> nodeMap);

    /// @brief Recursively destruct a node as well as all of its children.
    ///
    /// @param branchRoot Pointer to the node which is root of the branch.
    void destructBranch(NodePtr branchRoot);

    /// @brief Get a structure mapping node pointers to their node IDs, for 
    /// the branch whose root is the provided node pointer, as well as all 
    /// of its children.
    ///
    /// @param branchRoot Pointer to the node which is root of the branch.
    ///
    /// @return The node map of the branch.
    std::unordered_map<unsigned int, NodePtr> nodeMapFromBranch(NodePtr branchRoot);

    /// @brief Given a source root node and a destination root node, 
    /// recursively copy all source children nodes as children of the 
    /// destination node.
    ///
    /// @param source Pointer to the node from which to start copying.
    /// @param destination Pointer to the node to which to append the copied
    /// nodes.
    void copyNodeStructure(NodePtr source, NodePtr destination);

    /// @brief Count all elements with a certain value from a starting node,
    /// going downwards.
    ///
    /// @param value Value the elements must have in order to be counted.
    /// @param startingNode Pointer to the node from which to start 
    /// counting.
    ///
    /// @return The count of matched elements.
    unsigned int countValueRoutine(const T& value, NodePtr startingNode);

    /// @brief Return a pointer to the node which is the next one from the node
    /// with provided ID in a DFS traversal of the tree.
    ///
    /// @param id ID of the node whose next node in the traversal needs to be
    /// retrieved.
    ///
    /// @return Pointer to the next node in the traversal of the tree.
    NodePtr next(unsigned int id);

public:
    /// @param rootValue Value which the root node of the tree should hold
    Tree(T rootValue = T());

    Tree(const Tree<T>& other);
    Tree(Tree<T>&& other);
    Tree<T>& operator=(const Tree<T>& other);
    Tree<T>& operator=(Tree<T>&& other);
    ~Tree();

    /// @brief Get a pointer to the root node of the tree.
    ///
    /// @return A pointer to the root node.
    NodePtr getRoot();

    /// @brief Get a pointer to the node with provided ID.
    ///
    /// @param id ID of the node to retrieve.
    ///
    /// @return A pointer to the node if found, or nullptr otherwise.
    NodePtr operator[](unsigned int id);

    /// @brief Get a pointer to the node with provided ID.
    ///
    /// @param id ID of the node to retrieve.
    ///
    /// @return A pointer to the node if found, or nullptr otherwise.
    NodePtr operator[](unsigned int id) const;

    /// @brief Returns whether the contains has a node with provided ID.
    ///
    /// @param id ID of the node to check.
    ///
    /// @return Whether the tree contains a node with provided ID.
    bool hasNode(unsigned int id);

    /// @brief Add a node to the tree.
    ///
    /// @param value Value which the new node should hold.
    /// @param parentId ID of the node which should be parent of the newly
    /// created node.
    ///
    /// @return ID of the newly created node.
    ///
    /// @exception If the provided parent ID does not match that of a node 
    /// contained within the tree, the function will throw a 
    /// std::runtim_error.
    unsigned int addNode(T value, unsigned int parentId);

    /// @brief Add a whole to the tree, as a new branch.
    ///
    /// @param tree Tree whose structure will be copied to form the new 
    /// @param parentId ID of the node which should be parent of the newly
    /// created branch.
    ///
    /// @return ID of the root of the newly created branch.
    ///
    /// @exception If the provided parent ID does not match that of a node 
    /// contained within the tree, the function will throw a 
    /// std::runtim_error.
    unsigned int addBranch(const Tree<T>& tree, unsigned int parentId);

    /// @brief Remove a branch from the tree.
    ///
    /// @param branchRootId ID of the node which is root of the branch to
    /// remove.
    ///
    /// @exception If the provided ID is the ID of the root of the tree, or
    /// if it otherwise does not match that of node contained within the 
    /// tree, the function will throw a std::runtime_error.
    void removeBranch(unsigned int branchRootId);

    /// @brief Remove a branch from the tree, and return it.
    ///
    /// @param branchRootId ID of the node which is root of the branch to
    /// remove.
    ///
    /// @return The removed branch, as a new tree.
    /// 
    /// @exception If the provided ID is the ID of the root of the tree, or
    /// if it otherwise does not match that of node contained within the 
    /// tree, the function will throw a std::runtime_error.
    Tree<T> popBranch(unsigned int branchRootId);

    /// @brief Get a branch from the tree.
    ///
    /// @param branchRootId ID of the node which is root of the branch to
    /// retrieve.
    ///
    /// @return The branch copied in a new tree.
    Tree<T> getBranch(unsigned int branchRootId);

    /// @brief Move a branch in the tree (change its parent).
    ///
    /// @param branchRootId ID of the node which is root of the branch to
    /// move.
    /// @param newParentId ID of the node which should be the new parent of
    /// the moved branch.
    ///
    /// @exception If the branch root ID is the ID of the root of the tree, 
    /// or if any of the provided IDs do not match that of nodes contained 
    /// within the tree, the function will throw a std::runtime_error.
    void moveBranch(unsigned int branchRootId, unsigned int newParentId);

    /// @brief Remove all nodes in the tree, keeping the root and resetting
    /// it with a default-constructed value.
    void clear();

    /// @brief Count all elements with a certain value from a starting node,
    /// going downwards.
    ///
    /// @param value Value the elements must have in order to be counted.
    /// @param branchRootId Pointer to the node from which to start 
    /// counting.
    ///
    /// @return The count of matched elements.
    unsigned int countValue(const T& value, unsigned int branchRootId);

    bool operator==(const Tree<T>& rhs);

    bool operator!=(const Tree<T>& rhs);

    /// @brief Swap this tree with another.
    ///
    /// @param other Tree to swap content with.
    void swap(Tree<T>& other);

    /// @brief Get the number of elements in the tree.
    ///
    /// @return The number of elements in the tree.
    size_t size();

    /// @brief Get the maximum number of elements there can be in the tree.
    ///
    /// @return The maximum number of elements in the tree.
    size_t max_size();

    /// @brief Whether or not the tree is empty. As it always contains a root 
    /// node, the tree is never empty.
    ///
    /// @return False.
    bool empty();

    /// @brief Iterator to an element in the tree. Implements traversal in DFS
    /// order. Invalidated when the iterated node no longer belongs to the tree 
    /// from which the iterator was instantiated.
    class iterator
    {
    friend class Tree;
    using TreePtr = std::shared_ptr<Tree<T>>;

    private:
        /// @brief Pointer to the tree node currently being iterated.
        mutable NodePtr _node;

        /// @brief Pointer to the tree being traversed.
        const TreePtr _tree;

        /// @param node Pointer to the node to start iterating with.
        /// @param tree Pointer to the tree being traversed.
        iterator(NodePtr node, TreePtr tree);

    public:
        iterator();
        iterator(const iterator& other) = default;
        iterator& operator=(const iterator other) = default;
        ~iterator() = default;

        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::forward_iterator_tag;

        friend void swap(iterator& lhs, iterator& rhs);

        iterator& operator++();
        const iterator& operator++() const;

        iterator& operator++(int);
        const iterator& operator++(int) const;

        T& operator->();
        const T& operator->() const;

        T& operator*();
        const T& operator*() const;

        bool operator!=(const iterator& rhs) const;
        bool operator==(const iterator& rhs) const;

        /// @brief Iterate over the next element in a DFS traversal of the tree.
        void next() const;
    };
    using const_iterator = const iterator;
    using difference_type = iterator::difference_type;

    friend class iterator;

    /// @brief Get an iterator to start a DFS traversal of the tree.
    iterator begin();

    /// @brief Get the iterator past the end of a DFS traversal of the tree.
    iterator end();

    /// @brief Get an iterator to start a DFS traversal of the tree.
    const_iterator cbegin();

    /// @brief Get the iterator past the end of a DFS traversal of the tree.
    const_iterator cend();
};

template<typename T>
Tree<T>::Tree(T rootValue) :
    _root(nullptr),
    _nodes()
{
    // Create a node from the root value and register it in the node map
    _root = std::make_shared<Node>(rootValue);
    _nodes[_root->id] = _root;
}

template<typename T>
Tree<T>::Tree(const Tree<T>& other) :
    _root(nullptr),
    _nodes()
{
    // Copy the root node value from the other tree, make it this tree's own root
    _root = std::make_shared<Node>(other._root->value);
    // Copy the node structure from the other tree's root to this tree's root
    copyNodeStructure(other._root, _root);
    // Compute the node map
    _nodes = nodeMapFromBranch(_root);
}

template<typename T>
Tree<T>::Tree(Tree<T>&& other) :
    _root(std::move(other._root)),
    _nodes(std::move(other._nodes))
{

}

template<typename T>
Tree<T>& Tree<T>::operator=(const Tree<T>& other)
{
    // As this tree is being assigned to, its current content must be deleted
    destructBranch(_root);
    _nodes.clear();

    // Copy the root node value from the other tree, make it this tree's own root
    _root = std::make_shared<Node>(other._root->value);
    // Copy the node structure from the other tree's root to this tree's root
    copyNodeStructure(other._root, _root);
    // Compute the new node map
    _nodes = nodeMapFromBranch(_root);
}

template<typename T>
Tree<T>& Tree<T>::operator=(Tree<T>&& other)
{
    // As this tree is being assigned to, its current content must be deleted
    destructBranch(_root);
    _nodes.clear();

    _root = std::move(other._root);
    _nodes = std::move(other._nodes);
}

template<typename T>
Tree<T>::~Tree()
{
    // Basically delete everything
    destructBranch(_root);
    _nodes.clear();
}

template<typename T>
Tree<T>::Tree(NodePtr root) :
    _root(root),
    _nodes()
{
    _nodes = nodeMapFromBranch(_root);
}

template<typename T>
Tree<T>::Tree(NodePtr root, std::unordered_map<unsigned int, NodePtr> nodeMap) :
    _root(root),
    _nodes(nodeMap)
{

}

template<typename T>
typename Tree<T>::NodePtr Tree<T>::getRoot()
{
    return _root;
}

template<typename T>
typename Tree<T>::NodePtr Tree<T>::operator[](unsigned int id)
{
    auto it = _nodes.find(id);
    if (it == _nodes.end())
    {
        return nullptr;
    }

    return it->second;
}

template<typename T>
typename Tree<T>::NodePtr Tree<T>::operator[](unsigned int id) const
{
    auto it = _nodes.find(id);
    if (it == _nodes.end())
    {
        return nullptr;
    }

    return it->second;
}

template<typename T>
bool Tree<T>::hasNode(unsigned int id)
{
    NodePtr it = _nodes.find(id);
    return it != _nodes.end();
}

template<typename T>
unsigned int Tree<T>::addNode(T value, unsigned int parentId)
{
    if (size() == max_size())
    {
        throw std::runtime_error("Tree: max size reached, cannot add node.");
    }

    auto it = _nodes.find(parentId);
    if (it == _nodes.end())
    {
        std::string s = "Tree: no node with ID " + std::to_string(parentId) + ", cannot add child node.";
        throw std::runtime_error(s.c_str());
    }

    // Create the new node and have it automatically registered in its parent
    NodePtr node = std::make_shared<Node>(value);
    node->setParent(it->second);
    _nodes[node->id] = node;
    return node->id;
}

template<typename T>
unsigned int Tree<T>::addBranch(const Tree<T>& tree, unsigned int parentId)
{
    size_type remainingSpace = max_size() - size();
    if (tree.size() > remainingSpace)
    {
        std::string s = "Tree: only " + std::to_string(remainingSpace) + " additional nodes can be added, "
            "cannot add branch with " + std::to_string(tree.size()) + " nodes.";

        throw std::runtime_error(s.c_str());
    }

    auto it = _nodes.find(parentId);
    if (it == _nodes.end())
    {
        std::string s = "Tree: no node with ID " + std::to_string(parentId) + ", cannot add child branch.";
        throw std::runtime_error(s.c_str());
    }

    // Copy the other tree's root value as a new node of this tree, whose parent is a node with the provided ID
    unsigned int newBranchRootId = addNode(tree._root->value, parentId);
    // Get the pointer to the new node
    NodePtr newBranchRoot = _nodes[newBranchRootId];
    // Copy everything from the provided tree into the new node
    copyNodeStructure(tree._root, newBranchRoot);
    // Compute new node map starting from new node and merge it with the current node map
    std::unordered_map<unsigned int, NodePtr> newNodes = nodeMapFromBranch(newBranchRoot);
    _nodes.insert(newNodes.begin(), newNodes.end());

    return newBranchRootId;
}

template<typename T>
void Tree<T>::removeBranch(unsigned int branchRootId)
{
    if (branchRootId == _root->id)
    {
        throw std::runtime_error("Tree: cannot remove root.");
    }

    auto it = _nodes.find(branchRootId);
    if (it == _nodes.end())
    {
        std::string s = "Tree: no node with ID " + std::to_string(branchRootId) + ", cannot remove branch.";
        throw std::runtime_error(s.c_str());
    }

    NodePtr node = _nodes[branchRootId];
    NodePtr parent = it->second->getParent();
    // Detach the node with provided ID from its parent
    parent->removeChild(branchRootId);
    // Destruct all content starting from that node
    destructBranch(node);
}

template<typename T>
Tree<T> Tree<T>::popBranch(unsigned int branchRootId)
{
    if (branchRootId == _root->id)
    {
        throw std::runtime_error("Tree: cannot remove root.");
    }

    auto it = _nodes.find(branchRootId);
    if (it == _nodes.end())
    {
        std::string s = "Tree: no node with ID " + std::to_string(branchRootId) + ", cannot pop branch.";
        throw std::runtime_error(s.c_str());
    }

    NodePtr node = _nodes[branchRootId];
    NodePtr parent = node->getParent();
    // Detach the node with provided ID from its parent
    parent->removeChild(branchRootId);

    // Construct new tree from that node
    Tree<T> newTree = Tree<T>(node);

    // Delete all children pointers from this tree's node map, using the new tree's node map
    for (auto it = newTree._nodes.begin(); it != newTree._nodes.end(); it++)
    {
        _nodes.erase(it->first);
    }

    return newTree;
}

template<typename T>
Tree<T> Tree<T>::getBranch(unsigned int branchRootId)
{
    auto it = _nodes.find(branchRootId);
    if (it == _nodes.end())
    {
        std::string s = "Tree: no node with ID " + std::to_string(branchRootId) + ", cannot get branch.";
        throw std::runtime_error(s.c_str());
    }

    // Create a new node with a copy of the branch root node
    NodePtr root = std::make_shared<Node>(it->second->value);
    // Copy the branch structure into the new root
    copyNodeStructure(it->second, root);
    // Make a tree out of it
    return Tree<T>(root);
}

template<typename T>
void Tree<T>::moveBranch(unsigned int branchRootId, unsigned int newParentId)
{
    auto branchIt = _nodes.find(branchRootId);
    if (branchIt == _nodes.end())
    {
        std::string s = "Tree: no node with ID " + std::to_string(branchRootId) + ", cannot move branch.";
        throw std::runtime_error(s.c_str());
    }

    auto parentIt = _nodes.find(newParentId);
    if (parentIt == _nodes.end())
    {
        std::string s = "Tree: no node with ID " + std::to_string(newParentId) + ", "
            "cannot move branch " + std::to_string(branchRootId) + " to it.";

        throw std::runtime_error(s.c_str());
    }

    // Simply reset the parent of the branch root node to new parent node
    branchIt->second->setParent(parentIt->second);
}

template<typename T>
void Tree<T>::clear()
{
    _root->removeAllChildren();
    _nodes.clear();
    _root->value = T();
}

template<typename T>
unsigned int Tree<T>::countValue(const T& value, unsigned int branchRootId)
{
    auto branchIt = _nodes.find(branchRootId);
    if (branchIt == _nodes.end())
    {
        std::string s = "Tree: no node with ID " + std::to_string(branchRootId) + ", cannot move branch.";
        throw std::runtime_error(s.c_str());
    }

    return countValueRoutine(value, branchIt->second);
}

template<typename T>
bool Tree<T>::operator==(const Tree<T>& rhs)
{
    if (&rhs == this) return true;

    if (_nodes.size() != rhs._nodes.size()) return false;

    const_iterator thisIt = cbegin();
    const_iterator otherIt = rhs.cbegin();

    while (thisIt != cend() && otherIt != rhs.cend())
    {
        // Compare structure
        if (thisIt._node._wChildren.size() != otherIt._node._wChildren.size()) return false;

        // Compare value
        if (*thisIt != *otherIt) return false;

        thisIt++;
        otherIt++;
    }
    if (thisIt != cend() || otherIt != rhs.cend()) return false;

    return true;
}

template<typename T>
bool Tree<T>::operator!=(const Tree<T>& rhs)
{
    return !(operator==(rhs));
}

template<typename T>
void Tree<T>::swap(Tree<T>& other)
{
    using std::swap;

    swap(_root, other._root);
    swap(_nodes, other._nodes);
}

template<typename T>
size_t Tree<T>::size()
{
    return _nodes.size() + 1;
}

template<typename T>
size_t Tree<T>::max_size()
{
    return _nodes.size();
}

template<typename T>
bool Tree<T>::empty()
{
    return false;
}

template<typename T>
Tree<T>::iterator Tree<T>::begin()
{
    return iterator(_root, shared_from_this());
}

template<typename T>
Tree<T>::iterator Tree<T>::end()
{
    return iterator(nullptr, shared_from_this());
}

template<typename T>
Tree<T>::const_iterator Tree<T>::cbegin()
{
    return const_iterator(_root, shared_from_this());
}

template<typename T>
Tree<T>::const_iterator Tree<T>::cend()
{
    return const_iterator(nullptr, shared_from_this());
}

template<typename T>
void Tree<T>::destructBranch(NodePtr root)
{
    // First destroy all children of root
    std::vector<NodePtr> children = root->getChildren();
    for (auto it = children.begin(); it != children.end(); it++)
    {
        destructBranch(*it);
    }
    // Reset root parent
    root->setParent(nullptr);
    // Delete pointer to root
    _nodes.erase(root->id);
}

template<typename T>
std::unordered_map<unsigned int, typename Tree<T>::NodePtr> Tree<T>::nodeMapFromBranch(NodePtr branchRoot)
{
    std::unordered_map<unsigned int, NodePtr> registeredNodes;

    std::vector<NodePtr> children = branchRoot->getChildren();
    for (auto it = children.begin(); it != children.end(); it++)
    {
        // Compute the node map of each child
        std::unordered_map<unsigned int, NodePtr> childNodes = nodeMapFromBranch(*it);
        // Merge it with the current node map
        registeredNodes.insert(childNodes.begin(), childNodes.end());
    }

    // Add root node to map
    registeredNodes[branchRoot->id] = branchRoot;
    return registeredNodes;
}

template<typename T>
void Tree<T>::copyNodeStructure(NodePtr source, NodePtr destination)
{
    std::vector<NodePtr> sourceChildren = source->getChildren();
    for (auto it = sourceChildren.begin(); it != sourceChildren.end(); it++)
    {
        // Make new nodes by copying each child node's value
        NodePtr newChild = std::make_shared<Node>((*it)->value);
        newChild->setParent(destination);
        // Copy the node structure from each child to the corresponding newly created node
        copyNodeStructure(*it, newChild);
    }
}

template<typename T>
unsigned int Tree<T>::countValueRoutine(const T& value, NodePtr startingNode)
{
    int count = startingNode->value == value ? 1 : 0;
    std::vector<NodePtr> children = startingNode->getChildren();
    for (auto it = children.begin(); it != children.end(); it++)
    {
        count += countValueRoutine(value, *it);
    }

    return count;
}

template<typename T>
Tree<T>::NodePtr Tree<T>::next(unsigned int id)
{
    auto it = _nodes.find(id);
    if (it == _nodes.end())
    {
        throw std::runtime_error("Tree: next() called from invalid iterator.");
    }

    NodePtr node = it->second;
    std::vector<NodePtr> children = node->getChildren();
    if (children.size() > 0)
    {
        return children[0];
    }
    
    NodePtr parent = node->getParent();
    if (parent != nullptr)
    {
        NodePtr nextNode = parent->getNextSibling(node->id);
        if (nextNode != nullptr) return nextNode;
        
        return next(parent->id);
    }

    return nullptr;
}

template<typename T>
Tree<T>::iterator::iterator() :
    _node(nullptr),
    _tree(nullptr)
{

}

template<typename T>
Tree<T>::iterator::iterator(NodePtr node, TreePtr tree) :
    _node(node),
    _tree(tree)
{

}

template<typename T>
Tree<T>::iterator& Tree<T>::iterator::operator++()
{
    next();
    return *this;
}

template<typename T>
const Tree<T>::iterator& Tree<T>::iterator::operator++() const
{
    next();
    return *this;
}

template<typename T>
Tree<T>::iterator& Tree<T>::iterator::operator++(int)
{
    iterator tmp = *this;
    next();
    return tmp;
}

template<typename T>
const Tree<T>::iterator& Tree<T>::iterator::operator++(int) const
{
    iterator tmp = *this;
    next();
    return tmp;
}

template<typename T>
T& Tree<T>::iterator::operator->()
{
    return node->value;
}

template<typename T>
const T& Tree<T>::iterator::operator->() const
{
    return node->value;
}

template<typename T>
T& Tree<T>::iterator::operator*()
{
    return node->value;
}

template<typename T>
const T& Tree<T>::iterator::operator*() const
{
    return node->value;
}

template<typename T>
bool Tree<T>::iterator::operator!=(const iterator& rhs) const
{
    return !(operator==(rhs));
}

template<typename T>
bool Tree<T>::iterator::operator==(const iterator& rhs) const
{
    return (_node == rhs._node) && (_tree == rhs._tree);
}

template<typename T>
void Tree<T>::iterator::next() const
{
    _node = _tree->next(_node->id);
}

}//namespace CppTools

template<typename T>
void swap(CppTools::Tree<T>::iterator& lhs, CppTools::Tree<T>::iterator& rhs)
{
    using std::swap;

    swap(lhs._node, rhs._node);
    swap(lhs._tree, rhs._tree);
}

namespace std
{

template<typename T>
void advance(CppTools::Tree<T>::iterator& it, std::iterator_traits<CppTools::Tree<T>::iterator>::difference_type n)
{
    for (auto i = 0; i < n; i++) ++it;
}

template<typename T>
CppTools::Tree<T>::iterator next(
    CppTools::Tree<T>::iterator it,
    std::iterator_traits<CppTools::Tree<T>::iterator>::difference_type n
)
{
    advance(it, n);
    return it;
}

}//namespace std

#endif//CPPTOOLS__TREE_HPP