#include <catch2/catch_all.hpp>

#define CPPTOOLS_DEBUG_ENABLED 1
#define CPPTOOLS_DEBUG_POLICY CPPTOOLS_DEBUG_POLICY_LOG_AND_THROW
#define CPPTOOLS_DEBUG_TREE 1

#include <cpptools/container/tree.hpp>

#include "tree_test_utilities.hpp"

#undef CPPTOOLS_DEBUG_ENABLED
#undef CPPTOOLS_DEBUG_POLICY
#undef CPPTOOLS_DEBUG_TREE

inline constexpr char TAGS[] = "[container][tree_traversal]";

namespace tools::test
{

TEMPLATE_TEST_CASE( "Traversals of the tree yields the correct sequence of elements", TAGS, tree<int>, const tree<int> ) {
    TestType sample{{
        1,  {{  2,  {   {3},
                        {1, {3}}}},
             {  5,  {   {6},
                        {7}}}}
    }};

    // Tree:        1       |
    //            /   \     |
    //           2     5    |
    //          / \   / \   |
    //         3   1 6   7  |
    //             |        |
    //             3        |

    std::vector<int>         pre_order_elements  = { 1, 2, 3, 1, 3, 5, 6, 7 };
    std::vector<int>         post_order_elements = { 3, 3, 1, 2, 6, 7, 5, 1 };
    std::vector<int> reverse_pre_order_elements;
    std::ranges::reverse_copy(pre_order_elements , std::back_inserter(reverse_pre_order_elements));
    std::vector<int> reverse_post_order_elements;
    std::ranges::reverse_copy(post_order_elements, std::back_inserter(reverse_post_order_elements));

    using ItType = std::conditional_t<
        std::is_const_v<TestType>,
        typename TestType::template const_iterator<TestType::default_traversal>,
        typename TestType::template iterator<TestType::default_traversal>
    >;

    static_assert(std::is_same_v<decltype(sample.begin()), ItType>);

    SECTION( "pre-order traversal" ) {
        std::vector<int> elements;
        std::ranges::copy(sample, std::back_inserter(elements));
        REQUIRE( elements == pre_order_elements );
    }

    SECTION( "post-order traversal" ) {
        std::vector<int> elements;
        std::ranges::copy(sample, std::back_inserter(elements));
        REQUIRE( elements == post_order_elements );
    }

    SECTION( "reverse pre-order traversal" ) {
        std::vector<int> elements;
        std::ranges::copy(sample, std::back_inserter(elements));
        REQUIRE( elements == reverse_pre_order_elements );
    }

    SECTION( "reverse post-order traversal" ) {
        std::vector<int> elements;
        std::ranges::copy(sample, std::back_inserter(elements));
        REQUIRE( elements == reverse_post_order_elements );
    }

    SECTION( "search non-existent element" ) {
        REQUIRE( sample.template find<tree_traversal::pre_order> (8) == sample.template end<tree_traversal::pre_order> () );
        REQUIRE( sample.template find<tree_traversal::post_order>(8) == sample.template end<tree_traversal::post_order>() );
    }
}

TEMPLATE_TEST_CASE( "Finding a node by value in a tree yields a correct iterator", TAGS, tree<int>, const tree<int> ) {
    TestType sample{{
        1,  {{  2,  {   {3},
                        {1, {3}}}},
             {  5,  {   {6},
                        {7}}}}
    }};

    // Tree:        1       |
    //            /   \     |
    //           2     5    |
    //          / \   / \   |
    //         3   1 6   7  |
    //             |        |
    //             3        |
    //         Pre-order  traversal: 1 2 3 1 3 5 6 7
    //         Post-order traversal: 3 3 1 2 6 7 5 1
    // Reverse pre-order  traversal: 7 6 5 3 1 3 2 1
    // Reverse post-order traversal: 1 5 7 6 2 1 3 3

    using ItType = std::conditional_t<
        std::is_const_v<TestType>,
        typename TestType::template const_iterator<TestType::default_traversal>,
        typename TestType::template iterator<TestType::default_traversal>
    >;

    static_assert(std::is_same_v<decltype(sample.find(0)), ItType>);

    SECTION( "pre-order traversal" ) {
        // find the first 1 in pre-order
        auto it = sample.template find<tree_traversal::pre_order>(1);
        assert_node_with_value_and_children_was_found<int>( it.as_node(), 1, {2, 5});

        // find the next 1 in pre-order
        it = sample.template find<tree_traversal::pre_order>(1, it);
        assert_node_with_value_and_children_was_found<int>( it.as_node(), 1, {3});

        // find the next 1 in pre-order but there's none to be found
        it = sample.template find<tree_traversal::pre_order>(1, it);
        REQUIRE( it == sample.template end<tree_traversal::pre_order>() );
    }

    SECTION( "post-order traversal" ) {
        // find the first 1 in post-order
        auto it = sample.template find<tree_traversal::post_order>(1);
        assert_node_with_value_and_children_was_found<int>( it.as_node(), 1, {3});

        // find the next 1 in post-order
        it = sample.template find<tree_traversal::post_order>(1, it);
        assert_node_with_value_and_children_was_found<int>( it.as_node(), 1, {2, 5});

        // find the first 1 in post-order but there's none to be found
        it = sample.template find<tree_traversal::post_order>(1, it);
        REQUIRE( it == sample.template end<tree_traversal::post_order>() );
    }

    SECTION( "reverse pre-order traversal" ) {
        // find the first 1 in pre-order
        auto rb = sample.template rbegin<tree_traversal::pre_order>();
        auto re = sample.template rend<tree_traversal::pre_order>();

        auto it = std::find(rb, re, 1);
        assert_node_with_value_and_children_was_found<int>( it.base().as_node(), 1, {3});

        // find the next 1 in pre-order
        it = sample.template find<tree_traversal::pre_order>(1, it);
        assert_node_with_value_and_children_was_found<int>( it.base().as_node(), 1, {2, 5});

        // find the next 1 in pre-order but there's none to be found
        it = sample.template find<tree_traversal::pre_order>(1, it);
        REQUIRE( it == sample.template rend<tree_traversal::pre_order>() );
    }

    SECTION( "reverse post-order traversal" ) {
        // find the first 1 in post-order
        auto rb = sample.template rbegin<tree_traversal::post_order>();
        auto re = sample.template rend<tree_traversal::post_order>();

        auto it = std::find(rb, re, 1);
        assert_node_with_value_and_children_was_found<int>( it.base().as_node(), 1, {2, 5});

        // find the next 1 in post-order
        it = sample.template find<tree_traversal::post_order>(1, it);
        assert_node_with_value_and_children_was_found<int>( it.base().as_node(), 1, {3});

        // find the first 1 in post-order but there's none to be found
        it = sample.template find<tree_traversal::post_order>(1, it);
        REQUIRE( it == sample.template rend<tree_traversal::post_order>() );
    }

    SECTION( "search non-existent element" ) {
        REQUIRE( sample.template find<tree_traversal::pre_order> (8) == sample.template end<tree_traversal::pre_order> () );
        REQUIRE( sample.template find<tree_traversal::post_order>(8) == sample.template end<tree_traversal::post_order>() );
    }
}

} // namespace tools::test
