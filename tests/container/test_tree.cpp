#include <catch2/catch_all.hpp>

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

namespace tools::test
{

TEST_CASE( "A default constructed tree is an empty tree", TAGS ) {
    tree<int> t;

    assert_tree_is_empty(t);
}

TEST_CASE( "A tree copied from another has the same contents", TAGS ) {
    const auto original = make_sample_tree();
    auto original_elements = get_elements_and_addresses(original);

    SECTION( "copy construction" ) {
        tree<int> copy_constructed(original);
        assert_trees_were_copied(original, copy_constructed, original_elements);
    }

    SECTION( "copy assignment" ) {
        tree<int> copy_assigned;
        copy_assigned = original;
        assert_trees_were_copied(original, copy_assigned, original_elements);
    }
}

TEST_CASE( "A tree moved from another has the same contents, but moved", TAGS ) {
    auto original = make_sample_tree();
    auto original_elements = get_elements_and_addresses(original);

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

TEST_CASE( "Tree initializer yields a tree with correct contents and structure", TAGS ) {
    SECTION( "direct-initialization with non-empty init-list" ) {
        tree<int> t{ make_sample_tree_initializer() };

        assert_sample_tree_contents_and_structure_are_correct(t);
    }

    SECTION( "copy assignment from a non-empty init-list" ) {
        tree<int> t;
        t = make_sample_tree_initializer();

        assert_sample_tree_contents_and_structure_are_correct(t);
    }
}

TEST_CASE( "Fetching the root of the tree yields a handle to the root node of the tree", TAGS ) {
    tree<int> t1{{
        1,  {{2},
             {5}}
    }};

    tree<int> t2{{
        2,  {{5},
             {1}}
    }};

    tree<int> t5{{
        5,  {{1},
             {2}}
    }};

    REQUIRE( *t1.root() == 1 );
    REQUIRE( *t2.root() == 2 );
    REQUIRE( *t5.root() == 5 );
}

TEST_CASE( "Tree nodes know their respective child count and descendant count", TAGS ) {
    auto t = make_sample_tree();

    auto n1 = t.root();
    REQUIRE(n1.child_count() == 2);
    REQUIRE(n1.descendant_count() == 6);

    auto n2 = n1.children()[0];
    REQUIRE(n2.child_count() == 2);
    REQUIRE(n2.descendant_count() == 2);

    auto n5 = n1.children()[1];
    REQUIRE(n5.child_count() == 2);
    REQUIRE(n5.descendant_count() == 2);

    auto n3 = n2.children()[0];
    REQUIRE(n3.child_count() == 0);
    REQUIRE(n3.descendant_count() == 0);

    auto n6 = n2.children()[0];
    REQUIRE(n6.child_count() == 0);
    REQUIRE(n6.descendant_count() == 0);

    auto n4 = n2.children()[1];
    REQUIRE(n4.child_count() == 0);
    REQUIRE(n4.descendant_count() == 0);

    auto n7 = n2.children()[1];
    REQUIRE(n7.child_count() == 0);
    REQUIRE(n7.descendant_count() == 0);
}

TEST_CASE( "Erasing a leaf element from a tree erases the correct element", TAGS ) {
    auto t = make_sample_tree();
    auto n7 = t.root().children()[1].children()[1];
    auto n5 = n7.parent();

    t.erase_subtree(n7);
    REQUIRE( t.size() == 6 );
    REQUIRE( n5.child_count() == 1 );
}

TEST_CASE( "Erasing a non-leaf element from a tree erases the whole branch", TAGS ) {
    auto t = make_sample_tree();
    auto n1 = t.root();
    auto n2 = n1.children()[0];

    t.erase_subtree(n2);
    REQUIRE( t.size() == 4 );
    REQUIRE( n1.child_count() == 1 );
    REQUIRE( n1.descendant_count() == 3 );
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

    auto root = t.emplace_node(t.root(), 50293844);

    REQUIRE( t.size() == 1 );
    REQUIRE( *root == 50293844 );
}

TEST_CASE( "Trees can be swapped", TAGS ) {
    auto t1_original = make_sample_tree();
    auto t1_copy = t1_original;

    auto t2_original = make_sample_tree();
    auto t2_n2 = t2_original.root().child(0);
    t2_original.erase_subtree(t2_n2);
    auto t2_copy = t2_original;

    swap(t1_copy, t2_copy);
    REQUIRE( t1_copy == t2_original );
    REQUIRE( t2_copy == t1_original );
}

TEST_CASE( "Same trees should compare equal", TAGS ) {
    auto t1 = make_sample_tree();
    auto t2 = make_sample_tree();

    REQUIRE(       t1 == t2 );
    REQUIRE_FALSE( t1 != t2 );
}

TEST_CASE( "Different trees do not compare equal", TAGS ) {
    auto t1 = make_sample_tree();
    auto t2 = make_sample_tree();
    *(t2.begin()) = 3;

    REQUIRE(       t1 != t2 );
    REQUIRE_FALSE( t1 == t2 );
}

TEST_CASE( "Same iterators from the same tree instances compare equal", TAGS ) {
    auto t = make_sample_tree();

    REQUIRE(       t.begin() == t.begin() );
    REQUIRE_FALSE( t.begin() != t.begin() );
}

TEST_CASE( "Different iterators do not compare equal", TAGS ) {
    auto t1 = make_sample_tree();
    auto t2 = make_sample_tree();

    REQUIRE(       t1.begin() != t1.end() );
    REQUIRE_FALSE( t1.begin() == t1.end() );
}

TEMPLATE_TEST_CASE( "An iterator to a node knows about its parent and children nodes", TAGS, tree<int>, const tree<int> ) {
    TestType t = make_sample_tree();

    auto root = t.root();
    auto root_children = root.children();
    
    auto node2 = root_children[0];
    auto node2_parent = node2.parent();
    auto node2_children = node2.children();
    
    auto node5 = root_children[1];
    auto node5_parent = node5.parent();
    auto node5_children = node5.children();

    auto child3 = node2_children[0];
    auto child4 = node2_children[1];
    auto child6 = node5_children[0];
    auto child7 = node5_children[1];

    REQUIRE( node2_parent == root );
    REQUIRE( node5_parent == root );

    REQUIRE( node2.child_count() == 2 );
    REQUIRE( node2_children.size() == 2 );
    REQUIRE( *child3 == 3 );
    REQUIRE( *child4 == 4 );

    REQUIRE( child3.has_parent(node2) );
    REQUIRE( child3.has_parent(root) );
    REQUIRE( child4.has_parent(node2) );
    REQUIRE( child4.has_parent(root) );

    REQUIRE( node2.is_parent_of(child3) );
    REQUIRE( node2.is_parent_of(child4) );
    REQUIRE( root.is_parent_of(child3) );
    REQUIRE( root.is_parent_of(child4) );   
}

TEST_CASE( "A node can be emplaced in the tree as child of a mutable iterator", TAGS ) {

}

} // namespace tools::container