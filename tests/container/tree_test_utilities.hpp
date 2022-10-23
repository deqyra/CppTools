#ifndef TESTS__CONTAINER__TREE_TEST_UTILITIES_HPP
#define TESTS__CONTAINER__TREE_TEST_UTILITIES_HPP

#include <algorithm>
#include <cpptools/container/tree.hpp>

#include <ranges>
#include <vector>

namespace tools::container
{

void tree_copy_assertions(const tree<int>& t1, const tree<int>& t2);
void tree_move_assertions(tree<int>& t1, const tree<int>& t2, const tree<int>& t3, const int* const addr1);
void tree_init_assertions(tree<int>& t1);

template<typename T, tree_traversal t>
std::vector<T> tree_enumerate_children(typename tree<T>::template const_iterator<t> it)
{
    auto value_view = it.children() | std::views::transform([](auto jt){ return *jt; });
    return std::vector<T>{value_view.begin(), value_view.end()};
}

} // namespace tools::container

#endif//TESTS__CONTAINER__TREE_TEST_UTILITIES_HPP