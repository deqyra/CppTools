#ifndef TESTS__CONTAINER__TREE_TEST_UTILITIES_HPP
#define TESTS__CONTAINER__TREE_TEST_UTILITIES_HPP

#include <ranges>
#include <map>
#include <vector>

#include <catch2/catch_all.hpp>

#include <cpptools/container/tree.hpp>

namespace tools {

namespace test {

using addressed_elements = std::map<int, const int*>;

tree<int>::initializer make_sample_tree_initializer();
tree<int> make_sample_tree();
void assert_tree_is_empty(const tree<int>& t);
void assert_subtrees_were_copied(const const_node_handle<int>& original, const const_node_handle<int>& copy,  const addressed_elements& original_elements);
void assert_trees_were_copied   (const tree<int>& original,              const tree<int>& copy,               const addressed_elements& original_elements);
void assert_trees_were_moved    (const tree<int>& original,              const tree<int>& moved,              const addressed_elements& original_elements);
void assert_sample_tree_contents_and_structure_are_correct(const tree<int>& t);

addressed_elements get_elements_and_addresses(const const_node_handle<int>& subtree_root);
addressed_elements get_elements_and_addresses(const tree<int>& tree);

template<typename T, typename NodeHandle>
std::vector<T> tree_enumerate_immediate_children(const NodeHandle& handle)
{
    auto value_view = handle.children() | std::views::transform([](const auto& h){ return *h; });
    return std::vector<T>(value_view.begin(), value_view.end());
}

template<typename T, typename NodeHandle>
void assert_node_with_value_and_children_was_found(NodeHandle handle, const T& value, const std::vector<T>& children)
{
    REQUIRE(*handle == value);
    REQUIRE(tree_enumerate_immediate_children<T, NodeHandle>(handle) == children);
}

tree<unsigned char> make_very_large_tree(std::size_t depth, std::size_t breadth);

} // namespace test

namespace detail {

template<typename T>
std::ostream& operator<<(std::ostream& os, const const_node_handle<T>& nh) {
    return os << nh.value();
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const node_handle<T>& nh) {
    return os << nh.value();
}

} // namespace detail

} // namespace tools

#endif//TESTS__CONTAINER__TREE_TEST_UTILITIES_HPP