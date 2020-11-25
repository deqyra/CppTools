#ifndef TOOLS__ITERABLE_TOOLS_HPP
#define TOOLS__ITERABLE_TOOLS_HPP

#include <algorithm>

namespace IterTools
{
    // Given two iterators to an iterable of iterables, returns the maximum length of the contained iterables.
    template<typename Iter>
    int maxIterableLength(Iter first, Iter last)
    {
        // Classic max algorithm.
        int maxSize = 0;
        for (Iter it = first; it != last; it++)
        {
            int size = it->size();
            if (size > maxSize)
            {
                maxSize = size;
            }
        }

        return maxSize;
    }

    // Given an iterable of iterables, returns the maximum length of the contained iterables.
    template<typename Iterable>
    int maxIterableLength(const Iterable& container)
    {
        return maxIterableLength(container.begin(), container.end());
    }

    // Given two iterators to a range of iterables, returns the sum of all of their contents.
    template<typename T, typename Iter>
    T sum2NestedIterables(Iter first, Iter last)
    {
        // Classic sum algorithm.
        T result = 0;
        for (auto it = first; it != last; it++)
        {
            for (auto jt = it->begin(); jt != it->end(); jt++)
            {
                result += *jt;
            }
        }

        return result;
    }

   // Given an iterable of iterables, returns the sum of all of their contents.
    template<typename T, typename Iterable>
    T sum2NestedIterables(const Iterable& container)
    {
        return sum2NestedIterables<T>(container.begin(), container.end());
    }
 
    // Given two iterators to a range of comparable values, returns the index of the element with the maximum value.
    template<typename Iter>
    int indexOfMaxElement(Iter first, Iter last)
    {
        // Type-aware distance between vector start pointer and max element pointer = max index.
        return std::distance(first, std::max_element(first, last));
    }

    // Given an iterator to comparable values and an element count, returns the index of the element with the maximum value.
    template<typename Iter>
    int indexOfMaxElement(Iter first, int count)
    {
        // Type-aware distance between vector start pointer and max element pointer = max index.
        return std::distance(first, std::max_element(first, first + count));
    }

    // Given an iterable with comparable values, returns the index of the element with the maximum value.
    template<typename Iterable>
    int indexOfMaxElement(const Iterable& container)
    {
        return indexOfMaxElement(container.begin(), container.end());
    }

    template<typename Iterable, typename T>
    void stripElementFromIterable(Iterable& container, T element)
    {
        // Find the element in the input container.
        auto found = std::find(container.begin(), container.end(), element);

        // While elements are found...
        while (found != container.end())
        {
            // Erase that element and get the new position of the element that followed it.
            found = container.erase(found);
            // Find a new element from the current position.
            found = std::find(found, container.end(), element);
        }
    }
} // namespace IterTools

#endif//TOOLS__ITERABLE_TOOLS_HPP