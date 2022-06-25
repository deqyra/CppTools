#ifndef CPPTOOLS__CONTAINER__TREE_NODE_HPP
#define CPPTOOLS__CONTAINER__TREE_NODE_HPP

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace cpptools
{

// A node to be used in a tree, stores a T-type value and weak pointers to surronding nodes
template<typename T>
class node
{
// friend Tree<T>;

private:
    node(const node& other) = delete;
    node<T>& operator=(const node& other) = delete;

    // Keeps track of how many TreeNode objects were created (used as an ID system)
    static size_t _count;

    // Weak pointer to the parent of this node
    node* _parent;
    // Weak pointers to the children of this node
    std::vector<node*> _children;
    // IDs of the children of this node
    std::vector<size_t> _childrenIds;

    // Weak pointers to all the parents in the parent chain of this node
    std::vector<node*> _parentChain;
    // IDs of all the parents in the parent chain of this node
    std::vector<size_t> _parentIdChain;

    // Update the lists of IDs and pointers to parents in the parent chain
    void _generateParentChains();
    // Get a pointer to the child node which is next from the child with provided id
    node* _getNextSibling(size_t id);

public:
    // Construct new node with value attached
    node(T&& value);

    // Add new child to this node from a pointer
    void addChild(node* child);
    // Remove child from this node using its ID
    void removeChild(size_t id);
    // Whether the node with provided ID is a child of this node
    bool hasChild(size_t id);
    // Whether this node is a parent (to any degree) of the provided node pointer
    bool isAParentOf(node* node);
    // Whether this node is a child (to any degree) of the node with provided ID
    bool isChildOf(size_t id);
    
    // Unique ID of this node
    const size_t id;
    // Value attached to this node
    T value;
};

template<typename T>
size_t node<T>::_count = 0;

template<typename T>
node<T>::node(T&& value) :
    id(_count++),
    _parent(),
    _children(),
    _childrenIds(),
    _parentChain(),
    _parentIdChain(),
    value(std::forward(value))
{

}

template<typename T>
void node<T>::addChild(node* child)
{
    if (!child)
    {
        throw std::runtime_error("TreeNode: cannot add null child.");
    }

    // If this node is already the new child's parent node, do nothing
    if (child->_parent == this) return;

    // If the new child already has a parent node, abort
    if (child->_parent != nullptr)
    {
        throw std::runtime_error("TreeNode: node " + std::to_string(child->id) + " already has a parent, cannot add as child.");
    }

    // If this node is a child of the new child node, abort
    if (isChildOf(child))
    {
        std::string s = "TreeNode: node " + std::to_string(id) + " is a child of node " + std::to_string(child->id) + " and cannot set it as one of its own children.";
        throw std::runtime_error(s.c_str());
    }

    // Update parent pointer in the new child
    child->_parent = this->weak_from_this();
    // Update parent chains in the new child
    child->_generateParentChains();
    // Register child
    _children.push_back(child);
    _childrenIds.push_back(child->id);
}

template<typename T>
bool node<T>::hasChild(size_t childId)
{
    auto it = std::find(_childrenIds.begin(), _childrenIds.end(), childId);

    return it != _children.end();
}

template<typename T>
void node<T>::removeChild(size_t childId)
{
    auto itId = std::find(_childrenIds.begin(), _childrenIds.end(), childId);
    if (itId == _childrenIds.end())
    {
        std::string s = "TreeNode: no child with ID " + std::to_string(childId) + ", cannot remove.";
        throw std::runtime_error(s.c_str());
    }

    size_t index = std::distance(_childrenIds.begin(), itId);
    auto itNode = _children.begin() + index;

    node* child = itNode->lock();

    child->_parent = TreeNodePtr(nullptr);
    child->_generateParentChains();

    _childrenIds.erase(itId);
    _children.erase(itNode);
}

template<typename T>
bool node<T>::isAParentOf(node* node)
{
    if (!node) return false;
    // This node is a parent of the provided node if the provided node is a child of this node
    return node->isChildOf(id);
}

template<typename T>
bool node<T>::isChildOf(size_t childId)
{
    // Find the provided ID in the parent ID chain
    auto it = std::find(_parentIdChain.begin(), _parentIdChain.end(), childId);
    return it != _parentIdChain.end();
}

template<typename T>
void node<T>::_generateParentChains()
{
    _parentChain = std::vector<node*>();
    _parentIdChain = std::vector<size_t>();

    if (_parent != nullptr)
    {
        // Insert parent info and a copy of both parent chains 

        _parentChain.push_back(_parent);
        std::copy(_parent->_parentChain.begin(), _parent->_parentChain.end(), std::back_inserter(_parent->_parentChain));

        _parentIdChain.push_back(_parent->id);
        std::copy(_parent->_parentIdChain.begin(), _parent->_parentIdChain.end(), std::back_inserter(_parent->_parentIdChain));
    }

    // Reverberate to children
    for (auto child = _children.begin(); child != _children.end(); child++)
    {
        child->generateParentChains();
    }
}

template<typename T>
node<T>* node<T>::_getNextSibling(size_t childId)
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
    return _children[index];
}

} // namespace cpptools

#endif//CPPTOOLS__CONTAINER__TREE_NODE_HPP