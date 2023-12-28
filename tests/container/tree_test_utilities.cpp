#include "tree_test_utilities.hpp"
#include "cpptools/container/tree.hpp"
#include "tests/container/tree_test_utilities.hpp"

#include <catch2/catch_all.hpp>

#include <vector>

namespace tools::test
{

using namespace Catch::Matchers;

tree<int>::initializer make_sample_tree_initializer()
{
    return  {
        1,  {{  2,  {   {3},
                        {4}}},
             {  5,  {   {6},
                        {7}}}}
    };
}

tree<int> make_sample_tree()
{
    return { make_sample_tree_initializer() };
}

void assert_trees_were_copied(const tree<int>& original, const tree<int>& copy, const addressed_elements& original_elements)
{
    auto elements_after_copy = get_elements_and_addresses(original);
    auto copied_elements     = get_elements_and_addresses(copy);

    // contents from the copied tree should have been left untouched
    REQUIRE_THAT( elements_after_copy.values,    UnorderedRangeEquals(original_elements.values) );
    REQUIRE_THAT( elements_after_copy.addresses, UnorderedRangeEquals(original_elements.addresses) );

    // the copied tree should have the same elements
    REQUIRE_THAT( copied_elements.values, UnorderedRangeEquals(original_elements.values) );

    // the elements of the copied tree should live at different memory addresses from the original elements
    REQUIRE( copied_elements.addresses.size() == original_elements.addresses.size() );
    for (size_t i = 0; i < copied_elements.addresses.size(); ++i) {
        REQUIRE( copied_elements.addresses[i] != original_elements.addresses[i] );
    }

    /* C++23
    for (const auto [copied_address, original_address] : std::views::zip(copied_elements.addresses, original_elements.addresses)) {
        REQUIRE( copied_address != original_address );
    }
    */
}

void assert_trees_were_moved(const tree<int>& original, const tree<int>& moved, const addressed_elements& original_elements)
{
    assert_tree_is_empty(original);

    auto moved_elements = get_elements_and_addresses(moved);
    // the moved tree should have the same elements as the original prior to moving, living at the same memory addresses
    REQUIRE( moved_elements == original_elements );
}

void assert_tree_is_empty(const tree<int>& t)
{
    REQUIRE( t.empty() );
    REQUIRE( t.size() == 0 );
}

addressed_elements get_elements_and_addresses(const tree<int>& tree)
{
    addressed_elements result;

    result.values.reserve(tree.size());
    result.addresses.reserve(tree.size());

    for (const auto& e : tree)
    {
        result.values.push_back(e);
        result.addresses.push_back(&e);
    }

    return result;
}

void assert_sample_tree_contents_and_structure_are_correct(const tree<int>& t)
{
    auto root_node = t.root();
    REQUIRE( *root_node == 1 );

    auto child_nodes = root_node.children();
    auto child_values = tree_enumerate_immediate_children<int>(root_node); 
    REQUIRE(child_values == std::vector<int>{2, 5});

    auto  left_leaf_values = tree_enumerate_immediate_children<int>(child_nodes[0]); 
    auto right_leaf_values = tree_enumerate_immediate_children<int>(child_nodes[1]); 
    REQUIRE( left_leaf_values == std::vector<int>{3, 4});
    REQUIRE(right_leaf_values == std::vector<int>{6, 7});
}

}