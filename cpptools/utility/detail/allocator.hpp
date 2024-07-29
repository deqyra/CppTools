#ifndef CPPTOOLS_UTILITY_ALLOCATOR_HPP
#define CPPTOOLS_UTILITY_ALLOCATOR_HPP

#include <memory>

namespace tools::detail {

template<typename A, typename T>
using rebind_alloc_t = typename std::allocator_traits<A>::template rebind_alloc<T>;

} // namespace tools::detail 

#endif // CPPTOOLS_UTILITY_ALLOCATOR_HPP