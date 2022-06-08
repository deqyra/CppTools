#ifndef TOOLS__CONTAINER__TREE_NODE_HPP
#define TOOLS__CONTAINER__TREE_NODE_HPP

#include <algorithm>
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace tools
{

template<typename T>
// A node to be used in a tree, stores a T-type value and weak pointers to surronding nodes
class TreeNode : public std::enable_shared_from_this<TreeNode<T>>
{
    using TreeNodeWPtr = std::weak_ptr<TreeNode<T>>;
    using TreeNodePtr = std::shared_ptr<TreeNode<T>>;

    private:
        TreeNode(const TreeNode& other) = delete;
        TreeNode<T>& operator=(const TreeNode& other) = delete;

        // Keeps track of how many TreeNode objects were created (used as an ID system)
        static unsigned int _count;

        // Weak pointer to the parent of this node
        TreeNodeWPtr _wParent;
        // Weak pointers to the children of this node
        std::vector<TreeNodeWPtr> _wChildren;
        // IDs of the children of this node
        std::vector<unsigned int> _childrenIds;

        // Weak pointers to all the parents in the parent chain of this node
        std::vector<TreeNodeWPtr> _wParentChain;
        // IDs of all the parents in the parent chain of this node
        std::vector<unsigned int> _parentIdChain;

        // Update the lists of IDs and pointers to parents in the parent chain
        void generateParentChains();

    public:
        // Construct new node with value attached
        TreeNode(const T& value);

        // Construct new node with value attached
        TreeNode(T&& value);

        // Get a pointer to the parent of this node
        TreeNodePtr getParent();
        // Set the parent of this node from a pointer
        void setParent(TreeNodePtr parent);
        // Get a list of pointers to all parents of this node, from closest to furthest
        std::vector<TreeNodePtr> getParentChain();
        // Get list of IDs of all parents of this node, from closest to furthest
        std::vector<unsigned int> getParentIdChain();
        // Get list of pointers to all children of this node
        std::vector<TreeNodePtr> getChildren();
        // Add new child to this node from a pointer
        void addChild(TreeNodePtr child);
        // Remove child from this node using its ID
        void removeChild(unsigned int id);
        // Remove all children from this node
        void removeAllChildren();
        // Recursively remove all children from this node
        void removeAllChildrenRecursive();
        // Whether the node with provided ID is a child of this node
        bool hasChild(unsigned int id);
        // Get a pointer to the child node which is next from the child with provided id
        TreeNodePtr getNextSibling(unsigned int id);
        // Whether this node is a parent (to any degree) of the provided node pointer
        bool isAParentOf(TreeNodePtr node);
        // Whether this node is a child (to any degree) of the node with provided ID
        bool isChildOf(unsigned int id);
        
        // Unique ID of this node
        const unsigned int id;
        // Value attached to this node
        T value;
};

template<typename T>
unsigned int TreeNode<T>::_count = 0;

template<typename T>
TreeNode<T>::TreeNode(const T& value) :
    id(_count++),
    _wParent(),
    _wChildren(),
    _childrenIds(),
    _wParentChain(),
    _parentIdChain(),
    value(value)
{

}

template<typename T>
TreeNode<T>::TreeNode(T&& value) :
    id(_count++),
    _wParent(),
    _wChildren(),
    _childrenIds(),
    _wParentChain(),
    _parentIdChain(),
    value(std::move(value))
{

}

template<typename T>
typename TreeNode<T>::TreeNodePtr TreeNode<T>::getParent()
{
    return _wParent.lock();
}

template<typename T>
void TreeNode<T>::setParent(TreeNodePtr newParent)
{
    TreeNodePtr parent = _wParent.lock();

    // If the new parent is already this node's parent, do nothing
    if (newParent == parent) return;

    if (newParent.get() == this)
    {
        throw std::runtime_error("TreeNode: node cannot be its own parent.");
    }

    // If this node is a parent of the new parent, abort
    if (isAParentOf(newParent))
    {
        std::string s = "TreeNode: node " + std::to_string(id) + " is a parent of node " + std::to_string(newParent->id) + " and cannot set it as its own parent.";
        throw std::runtime_error(s.c_str());
    }

    // Remove this node as a child from its current parent if applicable
    if (parent != nullptr)
    {
        parent->removeChild(id);
    }

    // If applicable, register this node as a new child of the new parent
    if (newParent != nullptr)
    {
        newParent->_wChildren.push_back(this->weak_from_this());
        newParent->_childrenIds.push_back(id);
    }
    // Update this node's parent weak pointer
    _wParent = newParent;

    // Update parent chains
    generateParentChains();
}

template<typename T>
std::vector<typename TreeNode<T>::TreeNodePtr> TreeNode<T>::getParentChain()
{
    std::vector<TreeNodePtr> parentChain;
    parentChain.reserve(_wParentChain.size());

    for (auto it = _wParentChain.begin(); it != _wParentChain.end(); it++)
    {
        parentChain.push_back(it->lock());
    }

    return parentChain;
}

template<typename T>
std::vector<unsigned int> TreeNode<T>::getParentIdChain()
{
    return _parentIdChain;
}

template<typename T>
std::vector<typename TreeNode<T>::TreeNodePtr> TreeNode<T>::getChildren()
{
    std::vector<TreeNodePtr> children;
    children.reserve(_wChildren.size());

    for (auto it = _wChildren.begin(); it != _wChildren.end(); it++)
    {
        children.push_back(it->lock());
    }

    return children;
}

template<typename T>
void TreeNode<T>::addChild(TreeNodePtr child)
{
    if (!child)
    {
        throw std::runtime_error("TreeNode: provided node pointer is null, cannot add as child.");
    }

    TreeNodePtr childParent = child->_wParent.lock();

    // If this node is already the new child's parent node, do nothing
    if (childParent.get() == this) return;

    // If the new child already has a parent node, abort
    if (childParent != nullptr)
    {
        std::string s = "TreeNode: node " + std::to_string(child->id) + " already has a parent, cannot add as child.";
        throw std::runtime_error(s.c_str());
    }

    // If this node is a child of the new child node, abort
    if (isChildOf(child))
    {
        std::string s = "TreeNode: node " + std::to_string(id) + " is a child of node " + std::to_string(child->id) + " and cannot set it as one of its own children.";
        throw std::runtime_error(s.c_str());
    }

    // Update parent pointer in the new child
    child->_wParent = this->weak_from_this();
    // Update parent chains in the new child
    child->generateParentChains();
    // Register child
    _wChildren.push_back(child);
    _childrenIds.push_back(child->id);
}

template<typename T>
bool TreeNode<T>::hasChild(unsigned int childId)
{
    auto it = std::find(_childrenIds.begin(), _childrenIds.end(), childId);

    return it != _wChildren.end();
}

template<typename T>
typename TreeNode<T>::TreeNodePtr TreeNode<T>::getNextSibling(unsigned int childId)
{
    auto it = std::find(_childrenIds.begin(), _childrenIds.end(), childId);

    if (it == _childrenIds.end())
    {
        std::string s = "TreeNode: node " + std::to_string(id) + " has no child node with ID " + std::to_string(childId) + ", cannot get next sibling.";
        throw std::runtime_error(s.c_str());
    }
    
    // Seek the ID which is next from the provided one
    it++;
    if (it == _childrenIds.end()) return nullptr;

    // Return the correspondind pointer
    int index = std::distance(_childrenIds.begin(), it);
    return _wChildren[index].lock();
}

template<typename T>
void TreeNode<T>::removeChild(unsigned int childId)
{
    auto itId = std::find(_childrenIds.begin(), _childrenIds.end(), childId);
    if (itId == _childrenIds.end())
    {
        std::string s = "TreeNode: no child with ID " + std::to_string(childId) + ", cannot remove.";
        throw std::runtime_error(s.c_str());
    }

    size_t index = std::distance(_childrenIds.begin(), itId);
    auto itNode = _wChildren.begin() + index;

    TreeNodePtr child = itNode->lock();

    child->_wParent = TreeNodePtr(nullptr);
    child->generateParentChains();

    _childrenIds.erase(itId);
    _wChildren.erase(itNode);
}

template<typename T>
void TreeNode<T>::removeAllChildren()
{
    for (size_t i = _wChildren.size(); i > 0; i--)
    {
        TreeNodePtr child = _wChildren[i-1].lock();
        child->setParent(nullptr);
    }

    _wChildren.clear();
}

template<typename T>
void TreeNode<T>::removeAllChildrenRecursive()
{
    for (size_t i = _wChildren.size(); i > 0; i--)
    {
        TreeNodePtr child = _wChildren[i-1].lock();
        child->removeAllChildrenRecursive();
        child->setParent(nullptr);
    }

    _wChildren.clear();
}

template<typename T>
bool TreeNode<T>::isAParentOf(TreeNodePtr node)
{
    if (!node) return false;
    // This node is a parent of the provided node if the provided node is a child of this node
    return node->isChildOf(id);
}

template<typename T>
bool TreeNode<T>::isChildOf(unsigned int childId)
{
    // Find the provided ID in the parent ID chain
    auto it = std::find(_parentIdChain.begin(), _parentIdChain.end(), childId);
    return it != _parentIdChain.end();
}

template<typename T>
void TreeNode<T>::generateParentChains()
{
    auto parent = _wParent.lock();
    if (parent != nullptr)
    {
        // If this node has a parent, copy both of its own parent chains
        _wParentChain = parent->_wParentChain;
        _parentIdChain = parent->_parentIdChain;
        // Prepend the parent node to both
        _wParentChain.insert(_wParentChain.begin(), _wParent);
        _parentIdChain.insert(_parentIdChain.begin(), parent->id);
    }
    else
    {
        // If this node has no parent, the chains are empty
        _wParentChain = std::vector<TreeNodeWPtr>();
        _parentIdChain = std::vector<unsigned int>();
    }

    // Reverberate the update to all children
    for (auto it = _wChildren.begin(); it != _wChildren.end(); it++)
    {
        auto child = it->lock();
        child->generateParentChains();
    }
}

} // namespace tools

#endif//TOOLS__CONTAINER__TREE_NODE_HPP