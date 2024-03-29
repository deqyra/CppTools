#include <algorithm>
#include <catch2/catch_all.hpp>

#include <ranges>
#include <utility>

#define CPPTOOLS_DEBUG_ENABLED 1
#define CPPTOOLS_DEBUG_POLICY CPPTOOLS_DEBUG_POLICY_LOG_AND_THROW
#define CPPTOOLS_DEBUG_TREE 1

#include <cpptools/container/tree.hpp>

#include "tree_test_utilities.hpp"

#undef CPPTOOLS_DEBUG_ENABLED
#undef CPPTOOLS_DEBUG_POLICY
#undef CPPTOOLS_DEBUG_TREE

inline constexpr char TAGS[] = "[container][tree]";

using namespace Catch::Matchers;

namespace tools::test
{

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
        REQUIRE( std::ranges::equal(Node.children(), ChildMap[i]) );

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

TEST_CASE( "Handle to the tree root exposes the tree root", TAGS ) {
    tree<int> t{{
        9000,  {{69},
                {42}}
    }};

    auto root = t.root();
    REQUIRE( *root == 9000 );

    *root = 42;
    REQUIRE( t.root().value() == 42 );
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

    t.move_subtree(n2, n6);

    tree<int> result = {{
        1, {   {5, {   {6, {   {2,  {   {3},
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
                {5, {   {6, {   {2,  {   {3},
                                        {4}}}}},
                       {7}}}}
    }}; // ngl this syntax is nothing short of a mess

    REQUIRE( t2 == result );
}

TEMPLATE_TEST_CASE( "DFS traversal yields correctly ordered values", TAGS, tree<int>, const tree<int> ) {
    TestType t = make_sample_tree();

    SECTION( "Pre-order" ) {
        REQUIRE( std::ranges::equal(dfs<traversal::pre_order>(t), std::vector{ 1, 2, 3, 4, 5, 6, 7 }) );
    }

    SECTION( "Post-order" ) {
        REQUIRE( std::ranges::equal(dfs<traversal::post_order>(t), std::vector{ 3, 4, 2, 6, 7, 5, 1 }) );
    }

    SECTION( "Reverse pre-order" ) {
        REQUIRE( std::ranges::equal(reverse_dfs<traversal::pre_order>(t), std::vector{ 7, 6, 5, 4, 3, 2, 1 }) );
    }

    SECTION( "Reverse post-order" ) {
        REQUIRE( std::ranges::equal(reverse_dfs<traversal::post_order>(t), std::vector{ 1, 5, 7, 6, 2, 4, 3 }) );
    }
}

} // namespace tools::container