#include <catch2/catch2.hpp>

#include <string>
#include <utility>
#include <vector>

#define CPPTOOLS_DEBUG_ENABLED 1
#define CPPTOOLS_DEBUG_POLICY CPPTOOLS_DEBUG_POLICY_LOG_AND_THROW
#define CPPTOOLS_DEBUG_TREE 1

#include <cpptools/container/tree.hpp>

#include "tree_test_utilities.hpp"

#define TAGS "[container][tree]"

namespace tools::container
{

class custom
{
private:
    static size_t _copy_constructed;
    static size_t _move_constructed;

public:
    const int a;
    const std::string b;

    custom(int a = 0, const std::string& b = "") : a(a), b(b) {};

    custom(custom& o) : a(o.a), b(o.b)
    { ++_copy_constructed; }

    custom(custom&& o) : a(std::move(o.a)), b(std::move(o.b)) 
    { ++_move_constructed; }

    bool operator==(const custom& o) const
    { return (a == o.a) && (b == o.b); };

    static const size_t& copy_constructed;
    static const size_t& move_constructed;
};

const size_t& custom::copy_constructed = custom::_copy_constructed;
const size_t& custom::move_constructed = custom::_move_constructed;

TEST_CASE("tree default constructs as an empty tree", TAGS)
{
    tree<int> t{};

    // empty
    REQUIRE(t.empty());
}

TEST_CASE("tree root constructor can forward construction parameters to root constructor", TAGS)
{
    tree<custom> t{9, "abc"};

    // not empty
    REQUIRE(!t.empty());

    // one element equal to that constructed by arguments
    auto b = t.cbegin();
    REQUIRE(*b == custom{9, "abc"});
    REQUIRE(++b == t.cend());
}

TEST_CASE("tree root constructor can copy-construct root value", TAGS)
{
    size_t copy_constructed = custom::copy_constructed;
    size_t move_constructed = custom::move_constructed;

    auto c = custom{9, "abc"};
    tree<custom> t{c};

    // not empty
    REQUIRE(!t.empty());
    // copy constructed
    REQUIRE(custom::copy_constructed == copy_constructed + 1);
    // not move constructed
    REQUIRE(custom::move_constructed == move_constructed);

    // one element equal to the copied instance
    auto b = t.cbegin();
    REQUIRE(*b == c);
    REQUIRE(++b == t.cend());
}

TEST_CASE("tree root constructor can move-construct root value", TAGS)
{
    auto c1 = custom{9, "abc"};
    auto c2 = c1;
    size_t copy_constructed = custom::copy_constructed;
    size_t move_constructed = custom::move_constructed;

    tree<custom> t1{custom{9, "abc"}};
    tree<custom> t2{std::move(c2)};
    
    // not empty
    REQUIRE(!t1.empty());
    REQUIRE(!t2.empty());
    // not copy constructed
    REQUIRE(custom::copy_constructed == copy_constructed);
    // move constructed
    REQUIRE(custom::move_constructed == move_constructed + 2);

    // one element equal to a copy of the moved instance
    auto b1 = t1.cbegin();
    REQUIRE(*b1 == c1);
    REQUIRE(++b1 == t1.cend());

    // ...
    auto b2 = t2.cbegin();
    REQUIRE(*b2 == c1);
    REQUIRE(++b2 == t2.cend());
}

TEST_CASE("tree copy constructor", TAGS)
{
    tree<int> t1{{
        1,  {{  2,  {   {3},
                        {4}}},
             {  5,  {   {6},
                        {7}}}}
    }};

    tree<int> t2{t1};

    tree_copy_assertions(t1, t2);
}

TEST_CASE("tree move constructor", TAGS)
{
    tree<int> t1{{
        1,  {{  2,  {   {3},
                        {4}}},
             {  5,  {   {6},
                        {7}}}}
    }};

    tree<int> t2{t1};
    auto addr1 = &(*t1.cbegin());

    tree<int> t3{std::move(t1)};

    tree_move_assertions(t1, t2, t3, addr1);
}

TEST_CASE("tree init constructor", TAGS)
{
    tree<int> t1{{
        1,  {{  2,  {   {3},
                        {4}}},
             {  5,  {   {6},
                        {7}}}}
    }};

    tree_init_assertions(t1);
}

TEST_CASE("tree copy assignment", TAGS)
{
    tree<int> t1{{
        1,  {{  2,  {   {3},
                        {4}}},
             {  5,  {   {6},
                        {7}}}}
    }};

    tree<int> t2;
    t2 = t1;

    tree_copy_assertions(t1, t2);
}

TEST_CASE("tree move assignment", TAGS)
{
    tree<int> t1{{
        1,  {{  2,  {   {3},
                        {4}}},
             {  5,  {   {6},
                        {7}}}}
    }};

    tree<int> t2{t1};
    auto addr1 = &(*t1.cbegin());

    tree<int> t3;
    t3 = std::move(t1);

    tree_move_assertions(t1, t2, t3, addr1);
}

TEST_CASE("tree init assignment", TAGS)
{
    tree<int> t1;
    t1 = {{
        1,  {{  2,  {   {3},
                        {4}}},
             {  5,  {   {6},
                        {7}}}}
    }};

    tree_init_assertions(t1);
}

TEST_CASE("tree root", TAGS)
{
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

    // root values are correct
    REQUIRE(*t1.root() == 1);
    REQUIRE(*t2.root() == 2);
    REQUIRE(*t5.root() == 5);

    // const variant
    const tree<int>& ct1 = t1;
    const tree<int>& ct2 = t2;
    const tree<int>& ct5 = t5;

    // root values are correct
    REQUIRE(*ct1.root() == 1);
    REQUIRE(*ct2.root() == 2);
    REQUIRE(*ct5.root() == 5);
}

TEST_CASE("tree find", TAGS)
{
    tree<int> t1{{
        1,  {{  2,  {   {3},
                        {1, {3}}}},
             {  5,  {   {6},
                        {7}}}}
    }};
    // const variant
    const tree<int>& ct1 = t1;

    // find const and non-const, in pre- and post-order
    auto  it_pr1 =  t1.find<tree_traversal::pre_order>(1);
    auto cit_pr1 = ct1.find<tree_traversal::pre_order>(1);
    auto  it_po1 =  t1.find<tree_traversal::post_order>(1);
    auto cit_po1 = ct1.find<tree_traversal::post_order>(1);

    // correct value was found
    REQUIRE(*it_pr1  == 1);
    REQUIRE(*cit_pr1 == 1);
    REQUIRE(*it_po1  == 1);
    REQUIRE(*cit_po1 == 1);

    // correct node was found
    auto it_pr1_values  = tree_enumerate_children<int, tree_traversal::pre_order>(it_pr1);
    REQUIRE(it_pr1_values == std::vector<int>{2, 5});
    auto cit_pr1_values = tree_enumerate_children<int, tree_traversal::pre_order>(cit_pr1);
    REQUIRE(cit_pr1_values == std::vector<int>{2, 5});
    auto it_po1_values  = tree_enumerate_children<int, tree_traversal::post_order>(it_po1);
    REQUIRE(it_po1_values == std::vector<int>{3});
    auto cit_po1_values = tree_enumerate_children<int, tree_traversal::post_order>(cit_po1);
    REQUIRE(cit_po1_values == std::vector<int>{3});
}

} // namespace tools::container