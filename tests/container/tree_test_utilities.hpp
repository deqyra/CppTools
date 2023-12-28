#ifndef TESTS__CONTAINER__TREE_TEST_UTILITIES_HPP
#define TESTS__CONTAINER__TREE_TEST_UTILITIES_HPP

#include <ranges>
#include <vector>

#include <catch2/catch_all.hpp>

#include <cpptools/container/tree.hpp>

namespace tools::test
{

struct addressed_elements {
    std::vector<int> values;
    std::vector<const int*> addresses;

    bool operator==(const addressed_elements& other) const = default;
};

tree<int>::initializer make_sample_tree_initializer();
tree<int> make_sample_tree();
void assert_tree_is_empty(const tree<int>& t);
void assert_trees_were_copied(const tree<int>& original, const tree<int>& copy,  const addressed_elements& original_elements);
void assert_trees_were_moved (const tree<int>& original, const tree<int>& moved, const addressed_elements& original_elements);
void assert_sample_tree_contents_and_structure_are_correct(const tree<int>& t);

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

} // namespace tools::test

#endif//TESTS__CONTAINER__TREE_TEST_UTILITIES_HPP