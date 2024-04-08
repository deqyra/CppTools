#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_all.hpp>

#include <cpptools/container/tree.hpp>

#include <cmath>

constexpr char TAGS[] = "[container][tree][stress]";

namespace tools::test {

// TEST_CASE( "Generating a very large tree", TAGS ) {
    // for (int n = 1; n <= 10; ++n) {
    //     Catch::cout() << "Considering n = " << n << '\n';

    //     std::size_t node_count = 0;
    //     std::size_t size_bytes = 0;

    //     std::size_t leaf_node_size_bytes = sizeof(detail::node<unsigned char>);
    //     std::size_t non_leaf_node_size_bytes = leaf_node_size_bytes + n * sizeof(detail::node<unsigned char>*);

    //     for (int i = 0; i < n; ++i) {
    //         std::size_t this_depth_node_count = static_cast<std::size_t>(std::pow(n, i));
    //         std::size_t this_depth_node_size = (i == n - 1)
    //             ? leaf_node_size_bytes
    //             : non_leaf_node_size_bytes;

    //         node_count += this_depth_node_count;
    //         size_bytes += this_depth_node_count * this_depth_node_size;
    //     }
    //     size_bytes += sizeof(detail::unsafe_tree<unsigned char>);

    //     std::size_t delta = size_bytes - node_count;
    //     float ratio = static_cast<float>(delta) / node_count;
    //     Catch::cout() << "Node count          = " << std::setw(12) << std::setfill(' ') << node_count << " = total size in bytes of the contained unsigned char\n";
    //     Catch::cout() << "Total size in bytes = " << std::setw(12) << std::setfill(' ') << size_bytes << " bytes\n";
    //     Catch::cout() << "Non-data memory     = " << std::setw(12) << std::setfill(' ') << delta << " bytes\n";
    //     Catch::cout() << "Ratio               = " << std::setw(12) << std::setfill(' ') << ratio << std::endl  << std::endl;
    // }
// }

} // namespace tools::test