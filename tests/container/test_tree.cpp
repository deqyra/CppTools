#include <algorithm>
#include <ranges>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

//#include <cpptools/_internal/force_enable_debug.hpp>

#include <cpptools/container/tree.hpp>
#include <cpptools/utility/merge_strategy.hpp>

#include "tree_test_utilities.hpp"

namespace stdv = std::views;
using namespace Catch::Matchers;

constexpr char TAGS[] = "[container][tree]";

namespace tools::test {

TEST_CASE( "Default constructed tree is empty", TAGS ) {
    tree<int> t;

    assert_tree_is_empty(t);
}

TEST_CASE( "Tree copied from another has the same contents", TAGS ) {
    const auto original = make_sample_tree();
    auto original_elements = get_elements_and_addresses(original.root());

    SECTION( "copy construction" ) {
        tree<int> copy_constructed(original);
        REQUIRE( copy_constructed == original );
        assert_trees_were_copied(original, copy_constructed, original_elements);
    }

    SECTION( "copy assignment" ) {
        tree<int> copy_assigned;
        copy_assigned = original;
        REQUIRE( copy_assigned == original );
        assert_trees_were_copied(original, copy_assigned, original_elements);
    }
}

TEST_CASE( "Tree copied from a subtree has the same contents", TAGS ) {
    const auto original = make_sample_tree();
    const auto n5 = original.root().child(1);
    auto original_elements = get_elements_and_addresses(n5);

    SECTION( "copy construction" ) {
        tree<int> copy_constructed(n5);
        assert_subtrees_were_copied(n5, copy_constructed.root(), original_elements);
    }

    SECTION( "copy assignment" ) {
        tree<int> copy_assigned;
        copy_assigned = n5;
        assert_subtrees_were_copied(n5, copy_assigned.root(), original_elements);
    }
}

TEST_CASE( "Tree steals content from another during move", TAGS ) {
    auto original = make_sample_tree();
    auto original_elements = get_elements_and_addresses(original.root());

    SECTION( "move construction" ) {
        tree<int> move_constructed(std::move(original));
        assert_trees_were_moved(original, move_constructed, original_elements);
    }

    SECTION( "move assignment" ) {
        tree<int> move_assigned;
        move_assigned = std::move(original);
        assert_trees_were_moved(original, move_assigned, original_elements);
    }
}

TEST_CASE( "Tree made from an initializer has correct contents and structure", TAGS ) {
    SECTION( "construction" ) {
        tree<int> t(make_sample_tree_initializer());

        assert_sample_tree_contents_and_structure_are_correct(t);
    }

    SECTION( "move assignment" ) {
        tree<int> t;
        t = make_sample_tree_initializer();

        assert_sample_tree_contents_and_structure_are_correct(t);
    }
}

TEST_CASE( "Tree is aware of its size", TAGS ) {
    auto t = make_sample_tree();
    REQUIRE( t.size() == 7 );

    t = {{
        9000, {{69},
               {42}}
    }};
    REQUIRE( t.size() == 3 );
}

TEMPLATE_TEST_CASE( "Tree nodes are aware of their parent, children and siblings", TAGS, tree<int>, const tree<int> ) {
    using nh = std::conditional_t<std::is_const_v<TestType>, const_node_handle<int>, node_handle<int>>;

    TestType t = make_sample_tree();

    const std::array<nh, 7> n = {
        t.root(),
        t.root().child(0),
        t.root().child(0).child(0),
        t.root().child(0).child(1),
        t.root().child(1),
        t.root().child(1).child(0),
        t.root().child(1).child(1)
    };

    const std::array<nh, 7>              ParentMap = { nh::null_handle, n[0], n[1], n[1], n[0], n[4], n[4] };
    const std::array<std::vector<nh>, 7> ChildMap  = {
        std::vector<nh>{ n[1], n[4] },
        std::vector<nh>{ n[2], n[3] },
        std::vector<nh>{},
        std::vector<nh>{},
        std::vector<nh>{ n[5], n[6] },
        std::vector<nh>{},
        std::vector<nh>{}
    };

    constexpr std::array<std::array<bool, 7>, 7> ParentageMatrix = {
        //                   1      2      3      4      5      6      7
        std::array<bool, 7>{ false, true,  true,  true,  true,  true,  true  }, // 1
        std::array<bool, 7>{ false, false, true,  true,  false, false, false }, // 2
        std::array<bool, 7>{ false, false, false, false, false, false, false }, // 3
        std::array<bool, 7>{ false, false, false, false, false, false, false }, // 4
        std::array<bool, 7>{ false, false, false, false, false, true,  true  }, // 5
        std::array<bool, 7>{ false, false, false, false, false, false, false }, // 6
        std::array<bool, 7>{ false, false, false, false, false, false, false }  // 7
    };
    // ParentageMatrix[X][Y] tells whether node X is a parent to node Y to any degree

    const std::array<std::optional<nh>, 7> LeftSiblingMap = {
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::make_optional<nh>(n[2]),
        std::make_optional<nh>(n[1]),
        std::nullopt,
        std::make_optional<nh>(n[5]),
    };

    const std::array<std::optional<nh>, 7> RightSiblingMap = {
        std::nullopt,
        std::make_optional<nh>(n[4]),
        std::make_optional<nh>(n[3]),
        std::nullopt,
        std::nullopt,
        std::make_optional<nh>(n[6]),
        std::nullopt,
    };

    constexpr std::array<std::size_t, 7> SiblingIndexMap = { static_cast<std::size_t>(-1), 0, 0, 1, 1, 0, 1 };

    for (auto i = 0; i < 7; ++i) {
        const nh&  Node = n[i];
        const std::size_t DescendantCount = std::ranges::count(ParentageMatrix[i], true);

        REQUIRE( Node.parent()           == ParentMap[i] );
        REQUIRE( Node.child_count()      == ChildMap[i].size() );
        REQUIRE( Node.descendant_count() == DescendantCount );
        REQUIRE_THAT( Node.children(), RangeEquals(ChildMap[i]) );

        if (Node.parent() != nh::null_handle) {
            const bool HasLeftSibling  = (LeftSiblingMap[i].has_value());
            const bool HasRightSibling = (RightSiblingMap[i].has_value());

            REQUIRE( Node.is_leftmost_sibling()  != HasLeftSibling );
            REQUIRE( Node.is_rightmost_sibling() != HasRightSibling );
            REQUIRE( Node.sibling_index()        == SiblingIndexMap[i] );

            if (HasLeftSibling) {
                REQUIRE( Node.left_sibling() == LeftSiblingMap[i].value() );
            }
            if (HasRightSibling) {
                REQUIRE( Node.right_sibling() == RightSiblingMap[i].value() );
            }
        }

        for (auto j = 0; j < 7; ++j) {
            const auto& Other = n[j];

            REQUIRE( Node.is_parent_of(Other) == ParentageMatrix[i][j] );
            REQUIRE( Other.has_parent(Node)   == ParentageMatrix[i][j] );
        }
    }
}

TEMPLATE_TEST_CASE( "Retrieving the tree's root node, leftmost node and rightmost node yields handles to the proper nodes", TAGS, tree<int>, const tree<int> ) {
    TestType t{{
        9000,  {{69},
                {42}}
    }};

    auto root = t.root();
    REQUIRE( *root == 9000 );

    auto leftmost = t.leftmost();
    REQUIRE( *leftmost == 69 );

    auto rightmost = t.rightmost();
    REQUIRE( *rightmost == 42 );

    if constexpr (!std::is_const_v<TestType>) {
        *root = 1;
        REQUIRE( t.root().value() == 1 );

        *leftmost = 2;
        REQUIRE( t.leftmost().value() == 2 );

        *rightmost = 3;
        REQUIRE( t.rightmost().value() == 3 );
    }
}

TEST_CASE( "Tree keeps track of its extrema leaf nodes", TAGS ) {
    tree<int> t{{
        9000,  {{69},
                {42}}
    }};

    tree<int> t1{{
        1,  {{2},
             {3}}
    }};

    tree<int> t2{{
        4,  {{5},
             {6}}
    }};

    auto n69 = t.leftmost();                //    9000     //
    REQUIRE( *n69 == 69 );                  //    /   \    //
    auto n42 = t.rightmost();               //  69     42  //
    REQUIRE( *n42 == 42 );

    auto n2 = t.adopt_subtree(n69, t1).child(0);

    auto n1 = n2.parent();                  //     9000     //
    REQUIRE( t.leftmost() == n2 );          //     /   \    //
    REQUIRE( *n2 == 2 );                    //   69     42  //
    REQUIRE( t.rightmost() == n42);         //    |         //
    REQUIRE( *n42 == 42 );                  //    1         //
                                            //   / \        //
                                            //  2   3       //

    auto n6 = t.adopt_subtree(n42, t2).child(1);

    auto n5 = n6.left_sibling();            //     9000       //
    auto n4 = n6.parent();                  //     /   \      //
    REQUIRE( t.leftmost() == n2 );          //   69     42    //
    REQUIRE( *n2 == 2 );                    //    |      |    //
    REQUIRE( t.rightmost() == n6 );         //    1      4    //
    REQUIRE( *n6 == 6 );                    //   / \    / \   //
                                            //  2   3  5   6  //

    auto n10 = t.emplace_node(n2, 10);      //     9000       //
    REQUIRE( t.leftmost() == n10 );         //     /   \      //
    REQUIRE( *n10 == 10 );                  //   69     42    //
    REQUIRE( t.rightmost() == n6 );         //    |      |    //
    REQUIRE( *n6 == 6 );                    //    1      4    //
                                            //   / \    / \   //
                                            //  2   3  5   6  //
                                            //  |             //
                                            //  10            //

    auto n20 = t.emplace_node(n6, 20);      //     9000       //
    REQUIRE( t.leftmost() == n10 );         //     /   \      //
    REQUIRE( *n10 == 10 );                  //   69     42    //
    REQUIRE( t.rightmost() == n20 );        //    |      |    //
    REQUIRE( *n20 == 20 );                  //    1      4    //
                                            //   / \    / \   //
                                            //  2   3  5   6  //
                                            //  |          |  //
                                            //  10        20  //

    t.move_subtree(n6, n10);                //     9000         //
    REQUIRE( t.leftmost() == n2 );          //     /   \        //
    REQUIRE( *n2 == 2 );                    //   69     42      //
    REQUIRE( t.rightmost() == n10 );        //    |      |      //
    REQUIRE( *n10 == 10 );                  //    1      4      //
                                            //   / \    / \     //
                                            //  2   3  5   6    //
                                            //            / \   //
                                            //           20 10  //

    t.move_subtree(n2, n6);                 //       9000     //
    REQUIRE( t.leftmost() == n20 );         //       /   \    //
    REQUIRE( *n20 == 20 );                  //     69     42  //
    REQUIRE( t.rightmost() == n5 );         //      |      |  //
    REQUIRE( *n5 == 5 );                    //      1      4  //
                                            //     / \     |  //
                                            //    2   3    5  //
                                            //    |           //
                                            //    6           //
                                            //   / \          //
                                            //  20 10         //

    auto chopped1 = t.chop_subtree(n6);     //      9000        6    //
    REQUIRE( chopped1.leftmost() == n20 );  //      /   \      / \   //
    REQUIRE( *n20 == 20 );                  //    69     42   20 10  //
    REQUIRE( chopped1.rightmost() == n10 ); //     |      |          //
    REQUIRE( *n10 == 10 );                  //     1      4          //
    REQUIRE( t.leftmost() == n2 );          //    / \     |          //
    REQUIRE( *n2 == 2 );                    //   2   3    5          //
    REQUIRE( t.rightmost() == n5 );
    REQUIRE( *n5 == 5 );

    auto chopped2 = t.chop_subtree(n5);     //     9000       5  //
    REQUIRE( chopped2.leftmost() == n5 );   //     /   \         //
    REQUIRE( chopped2.rightmost() == n5 );  //   69     42       //
    REQUIRE( *n5 == 5 );                    //    |      |       //
    REQUIRE( t.leftmost() == n2 );          //    1      4       //
    REQUIRE( *n2 == 2 );                    //   / \             //
    REQUIRE( t.rightmost() == n4 );         //  2   3            //
    REQUIRE( *n4 == 4 );

    t.erase_subtree(n1);                    //    9000     //
    REQUIRE( t.leftmost() == n69 );         //    /   \    //
    REQUIRE( *n69 == 69 );                  //  69     42  //
    REQUIRE( t.rightmost() == n4 );         //          |  //
    REQUIRE( *n4 == 4 );                    //          4  //

    t.erase_subtree(n4);                    //    9000     //
    REQUIRE( t.leftmost() == n69 );         //    /   \    //
    REQUIRE( *n69 == 69 );                  //  69     42  //
    REQUIRE( t.rightmost() == n42 );
    REQUIRE( *n42 == 42 );

    n1 = t.adopt_subtree(n69, t1);          //    9000     //
    n4 = t.adopt_subtree(n42, t2);          //    /   \    //
    n2 = n1.child(0);                       //  69     42  //
    auto n3 = n1.child(1);                  //   |     |   //
    n5 = n4.child(0);                       //   1     4   //
    n6 = n4.child(1);                       //  / \   / \  //
                                            // 2   3 5   6 //

    t.merge_with_parent<merge::add>(n2);        //    9000     //
    REQUIRE( *n1 == 3 );                        //    /   \    //
    REQUIRE( t.leftmost() == n3 );              //  69     42  //
    REQUIRE( *n3 == 3 );                        //   |     |   //
    REQUIRE( t.rightmost() == n6);              //   3     4   //
    REQUIRE( *n6 == 6 );                        //   |    / \  //
                                                //   3   5   6 //
                                            
    t.merge_with_parent<merge::multiply>(n6);   //    9000     //
    REQUIRE( *n4 == 24 );                       //    /   \    //
    REQUIRE( t.leftmost() == n3 );              //  69     42  //
    REQUIRE( *n3 == 3 );                        //   |     |   //
    REQUIRE( t.rightmost() == n5);              //   1     24  //
    REQUIRE( *n5 == 5 );                        //   |     |   //
                                                //   3     5   //
}

TEST_CASE( "Elements can be erased from a tree", TAGS ) {
    auto t = make_sample_tree();
    CHECK( t.size() == 7 );

    SECTION( "Leaf elements" ) {
        auto n7 = t.root().child(1).child(1);
        auto n5 = n7.parent();

        t.erase_subtree(n7);
        REQUIRE( t.size() == 6 );
        REQUIRE( n5.child_count() == 1 );
    }

    SECTION( "Entire subtrees" ) {
        auto n1 = t.root();
        auto n2 = n1.child(0);

        t.erase_subtree(n2);
        REQUIRE( t.size() == 4 );
        REQUIRE( n1.child_count() == 1 );
        REQUIRE( n1.descendant_count() == 3 );
    }
}

TEST_CASE( "Erasing the root of a tree makes it an empty tree", TAGS ) {
    auto t = make_sample_tree();

    t.erase_subtree(t.root());
    assert_tree_is_empty(t);
}

TEST_CASE( "Clearing a tree makes it an empty tree", TAGS ) {
    auto t = make_sample_tree();

    t.clear();
    assert_tree_is_empty(t);
}

TEST_CASE( "Emplacing a child in an empty tree creates a root node in the tree" ) {
    tree<int> t;

    SECTION( "empty tree" ) {

    }

    SECTION( "cleared tree" ) {
        t = make_sample_tree();
        t.clear();
    }

    CHECK( t.size() == 0 );

    auto root = t.emplace_node(t.root(), 50293844);

    REQUIRE( t.size() == 1 );
    REQUIRE( *root == 50293844 );
}

TEST_CASE( "Trees can be swapped", TAGS ) {
    auto full_sample = make_sample_tree();
    auto full_copy = full_sample;

    auto partial_sample = make_sample_tree();
    partial_sample.erase_subtree(partial_sample.root().child(0));
    auto partial_copy = partial_sample;

    CHECK( full_copy == full_sample );
    CHECK( partial_copy == partial_sample );

    swap(full_copy, partial_copy);
    
    REQUIRE( full_copy == partial_sample );
    REQUIRE( partial_copy == full_sample );
}

TEST_CASE( "Same trees compare equal, different trees do not", TAGS ) {
    auto t1 = make_sample_tree();
    auto t2 = make_sample_tree();

    REQUIRE( t1 == t2 );

    t2.root().value() = 3;

    REQUIRE( t1 != t2 );
}

TEST_CASE( "Same iterators compare equal, different iterators do not", TAGS ) {
    auto t1 = make_sample_tree();

    REQUIRE( t1.begin() == t1.begin() );

    auto t2 = make_sample_tree();

    REQUIRE( t1.begin() != t1.end() );
}

TEST_CASE( "A node can be emplaced in the tree as child of another node", TAGS ) {
    auto t = make_sample_tree();

    auto root = t.root();
    auto node2 = root.child(0);
    auto node4 = node2.child(1);

    REQUIRE( node4.child_count() == 0 );

    auto new_node = t.emplace_node(node4, 42);

    REQUIRE( node4.child_count() == 1 );
    REQUIRE( node4.child(0) == new_node );
    REQUIRE( new_node.value() == 42 );
    REQUIRE( root.descendant_count() == 7 );
    REQUIRE( t.size() == 8 );
}

TEST_CASE( "Subtrees can be chopped off", TAGS ) {
    auto t = make_sample_tree();

    auto root = t.root();
    auto n2 = root.child(0);

    auto chopped = t.chop_subtree(n2);

    tree<int> result = {
        { 2, {{3}, {4}}}
    };

    tree<int> remainder = {
        {1, {{ 5, {{6}, {7}}}}}
    };

    REQUIRE( chopped == result );
    REQUIRE( t == remainder );
}

TEST_CASE( "Subtrees can be moved within a tree", TAGS ) {
    auto t = make_sample_tree();

    auto root = t.root();
    auto n2 = root.child(0);
    auto n6 = root.child(1).child(0);

    t.move_subtree(n6, n2);

    tree<int> result = {{
        1, {   {5, {    {6, {  {2,  {   {3},
                                        {4}}}}},
                        {7}}}}
    }};

    REQUIRE( t == result );
}

TEST_CASE( "Subtrees can be moved across trees", TAGS ) {
    auto t1 = make_sample_tree();
    auto t2 = make_sample_tree();

    auto t1n2 = t1.root().child(0);
    auto t2n6 = t2.root().child(1).child(0);

    auto chopped = t1.chop_subtree(t1n2);

    t2.adopt_subtree(t2n6, std::move(chopped));

    tree<int> result = {{
        1, {    {2, {   {3},
                        {4}}},
                {5, {   {6, {   {2, {   {3},
                                        {4}}}}},
                        {7}}}}
    }}; // ngl this syntax is nothing short of a mess

    REQUIRE( t2 == result );
}

struct merge_mock {
    void operator()(const int& original, const int& other) {
        ++call_count;
    }

    static int get_call_count() {
        return call_count;
    }

private:
    static int call_count;
};

int merge_mock::call_count = 0;

TEST_CASE( "Node can be merged with its parent", TAGS ) {
    auto t = make_sample_tree();

    SECTION( "Merging process calls the merge function" ) {
        int merge_count = merge_mock::get_call_count();

        auto n2 = t.root().child(0);
        auto n3 = n2.child(0);
        auto n4 = n2.child(1);
        t.merge_with_parent<merge_mock>(n3);

        REQUIRE( merge_mock::get_call_count() == merge_count + 1 );
        REQUIRE( n2.child_count() == 1 );
        REQUIRE( n2.child(0) == n4 );
        REQUIRE( *n4 == 4 );
    }

    SECTION( "Child nodes are adopted at the proper position by their new parent" ) {
        auto n2 = t.root().child(0);
        auto n5 = t.root().child(1);

        // merging a "left" node
        t.merge_with_parent<merge::keep>(n2);

        tree<int> result = {{
            1, {    {3},
                    {4},
                    {5, {   {6},
                            {7}}}}
        }};

        REQUIRE( t == result );

        // merging a "right" node
        t.merge_with_parent<merge::replace>(n5);

        result = {{
            5, {    {3},
                    {4},
                    {6},
                    {7}}
        }};

        REQUIRE( t == result );
    }
}

TEMPLATE_TEST_CASE( "DFS traversal yields correctly ordered values", TAGS, tree<int>, const tree<int> ) {
    TestType t = make_sample_tree();

    constexpr auto PreorderTraversalValues  = std::array{ 1, 2, 3, 4, 5, 6, 7 };
    constexpr auto PostorderTraversalValues = std::array{ 3, 4, 2, 6, 7, 5, 1 };

    SECTION( "Pre-order" ) {
        REQUIRE_THAT( dfs<traversal::pre_order>(t), RangeEquals(PreorderTraversalValues) );
    }

    SECTION( "Post-order" ) {
        REQUIRE_THAT( dfs<traversal::post_order>(t), RangeEquals(PostorderTraversalValues) );
    }

    SECTION( "Reverse pre-order" ) {
        REQUIRE_THAT( reverse_dfs<traversal::pre_order>(t), RangeEquals(PreorderTraversalValues | stdv::reverse) );
    }

    SECTION( "Reverse post-order" ) {
        REQUIRE_THAT( reverse_dfs<traversal::post_order>(t), RangeEquals(PostorderTraversalValues | stdv::reverse) );
    }
}

} // namespace tools::container