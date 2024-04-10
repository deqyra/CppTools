#include "tree_test_utilities.hpp"

#include <stack>
#include <vector>

#include <catch2/catch_all.hpp>

#include <cpptools/container/tree.hpp>

namespace tools::test {

tree<int>::initializer make_sample_tree_initializer()
{
    return {
        1, {
            {2, {
                {3},
                {4}
            }},
            {5, {
                {6},
                {7}
            }}
        }
    };
}

tree<int> make_sample_tree() {
    return { make_sample_tree_initializer() };
}

addressed_elements get_elements_and_addresses(const const_node_handle<int>& subtree_root) {
    using cnh = const_node_handle<int>;
    addressed_elements result;

    std::stack<cnh> node_stack;
    auto n = subtree_root;

    do {
        result.insert({n.value(), &n.value()});
        // okay because:
        // 1. the underlying unique_ptr-based storage guarantees address stability
        // 2. we deal with copyable types

        if (n.child_count() > 0) {
            node_stack.push(n);
            n = n.child(0);
        } else {
            // unstack to a non-rightmost node
            while (!node_stack.empty() && n.parent() != cnh::null_handle && n.is_rightmost_sibling()) {
                n = node_stack.top();
                node_stack.pop();
            }

            if (!node_stack.empty()) {
                n = n.right_sibling();
            }
        }
    } while (!node_stack.empty());

    return result;
}

void assert_tree_is_empty(const tree<int>& t) {
    REQUIRE( t.empty() );
    REQUIRE( t.size() == 0 );
}

void assert_subtrees_were_copied(const const_node_handle<int>& original, const const_node_handle<int>& copy, const addressed_elements& original_elements) {
    auto original_elements_after_copy = get_elements_and_addresses(original);
    auto copied_elements              = get_elements_and_addresses(copy);

    // contents from the copied tree should have been left untouched
    REQUIRE( original_elements_after_copy == original_elements );

    // the copied tree should have the same elements
    auto orig_it  = original_elements.begin();
    auto orig_end = original_elements.end();
    auto copy_it  = copied_elements.begin();
    auto copy_end = copied_elements.end();

    for (; orig_it != orig_end && copy_it != copy_end; ++orig_it, ++copy_it) {
        REQUIRE( copy_it->first  == orig_it->first  ); // equal values
        REQUIRE( copy_it->second != orig_it->second ); // different addresses, aka copies
    }

    /* C++23
    for (const auto& [original, copied] : std::views::zip(original_elements, copied_elements)) {
        REQUIRE( copied.second == original.second );
        REQUIRE( copied.first  != original.first  );
    }
    */
}

void assert_trees_were_copied(const tree<int>& original, const tree<int>& copy, const addressed_elements& original_elements) {
    assert_subtrees_were_copied(original.root(), copy.root(), original_elements);
}

void assert_trees_were_moved(const tree<int>& original, const tree<int>& moved, const addressed_elements& original_elements) {
    assert_tree_is_empty(original);

    auto moved_elements = get_elements_and_addresses(moved.root());
    // the moved tree should have the same elements as the original prior to moving, living at the same memory addresses
    REQUIRE( moved_elements == original_elements );
}

void assert_sample_tree_contents_and_structure_are_correct(const tree<int>& t) {
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

tree<unsigned char> make_very_large_tree(std::size_t depth, std::size_t breadth) {
    tree<unsigned char> t;
    t.emplace_node(t.root(), static_cast<unsigned char>(0));

    unsigned char i = 0;

    auto node = t.root();

    std::stack<decltype(node)> node_stack;
    
    std::size_t d = 1;
    bool done = false;
    while (!done) {
        if (node.child_count() != breadth) {
            auto emplaced = t.emplace_node(node, static_cast<unsigned char>(++i));
            if (d < depth - 1) {
                node_stack.push(node);
                node = emplaced;
                ++d;
            }
        } else if (!node_stack.empty()) {
            node = node_stack.top();
            node_stack.pop();
            --d;
        } else {
            done = true;
        }
    }

    return t;
}

} // namespace tools::test
