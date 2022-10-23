#include "tree_test_utilities.hpp"

#include <catch2/catch2.hpp>

#include <string>
#include <utility>
#include <vector>

namespace tools::container
{

void tree_copy_assertions(const tree<int>& t1, const tree<int>& t2)
{
    // both not empty and equal
    REQUIRE(!t1.empty());
    REQUIRE(!t2.empty());
    REQUIRE(t1 == t2);

    auto b1 = t1.cbegin();
    auto b2 = t2.cbegin();
    auto addr1 = &(*b1);
    auto addr2 = &(*b2);
    // addresses of content different in both trees
    REQUIRE(addr1 != addr2);
}

void tree_move_assertions(tree<int>& t1, const tree<int>& t2, const tree<int>& t3, const int* const addr1)
{
    auto addr2 = &(*t2.cbegin());
    auto addr3 = &(*t3.cbegin());

    // t1 has been moved from and an empty tree
    REQUIRE(t1 == tree<int>{});
    // t3 was move-constructed and is not empty
    REQUIRE(!t3.empty());
    // t3 has same content as t2
    REQUIRE(t2 == t3);

    // nodes in t2 and t3 live at different places
    REQUIRE(addr2 != addr3);
    // nodes in t3 are the same as those which were in t3
    REQUIRE(addr1 == addr3);
}

void tree_init_assertions(tree<int>& t1)
{
    auto it = t1.cbegin<tree_traversal::pre_order>();
    auto child_nodes = it.children();

    auto child_values = tree_enumerate_children<int>(it); 
    // structure and values are correct
    REQUIRE(child_values == std::vector<int>{2, 5});

    // structure and values are correct
    auto left_leaf_values = tree_enumerate_children<int>(child_nodes[0]); 
    auto right_leaf_values = tree_enumerate_children<int>(child_nodes[1]); 
    REQUIRE(left_leaf_values  == std::vector<int>{3, 4});
    REQUIRE(right_leaf_values == std::vector<int>{6, 7});
}

}