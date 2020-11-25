#ifndef TREE_HPP
#define TREE_HPP

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <vector>
#include <iostream>

#include "tree_node.hpp"

/// @brief A tree managing nodes organized in a hierarchical structure, in which
/// one node may only have one parent.
///
/// @tparam T The type of elements which the tree will store.
template<typename T>
class Tree
{
    public:
        using Node = TreeNode<T>;
        using NodePtr = std::shared_ptr<Node>;
        using NodeWPtr = std::weak_ptr<Node>;

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

    public:
        /// @param rootValue Value which the root node of the tree should hold
        Tree(T rootValue = T());

        Tree(const Tree<T>& other);
        Tree<T>& operator=(const Tree<T>& other);
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
bool Tree<T>::hasNode(unsigned int id)
{
    NodePtr it = _nodes.find(id);
    return it != _nodes.end();
}

template<typename T>
unsigned int Tree<T>::addNode(T value, unsigned int parentId)
{
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
        std::string s = "Tree: no node with ID " + std::to_string(newParentId) + ", cannot move branch " + std::to_string(branchRootId) + " to it.";
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

#endif//TREE_HPP