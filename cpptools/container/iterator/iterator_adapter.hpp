#ifndef CPPTOOLS__CONTAINER__ITERATOR__ITERATOR_ADAPTER_HPP
#define CPPTOOLS__CONTAINER__ITERATOR__ITERATOR_ADAPTER_HPP

#include <iterator>
#include <type_traits>

namespace tools {

template<typename MyIter, typename T, T(IterTransformFunc)(const MyIter&)>
requires std::is_reference_v<T>
class iterator_adapter {
    MyIter _it;

    static constexpr bool NoThrowValueInit = std::is_nothrow_default_constructible_v<MyIter>;
    static constexpr bool NoThrowCopy = std::is_nothrow_copy_constructible_v<MyIter>;
    static constexpr bool NoThrowMove = std::is_nothrow_move_constructible_v<MyIter>;

public:
    using value_type      = std::remove_reference_t<T>;
    using reference       = T;
    using pointer         = value_type*;
    using difference_type = typename MyIter::difference_type;

    iterator_adapter() noexcept(NoThrowValueInit) :
        _it{}
    {

    }

    iterator_adapter(const MyIter& it) noexcept(NoThrowCopy) :
        _it{std::forward<MyIter&&>(it)}
    {

    }

    iterator_adapter(MyIter&& it) noexcept(NoThrowMove) :
        _it{std::forward<MyIter&&>(it)}
    {

    }

    iterator_adapter(const iterator_adapter& other) noexcept(NoThrowCopy) :
        iterator_adapter(other._it)
    {

    }

    iterator_adapter(iterator_adapter&& other) noexcept(NoThrowMove) :
        iterator_adapter(std::move(other._it))
    {

    }

    ~iterator_adapter() noexcept (std::is_nothrow_destructible_v<MyIter>) = default;

    iterator_adapter& operator=(const iterator_adapter& other) noexcept (std::is_nothrow_copy_assignable_v<MyIter>) {
        _it = other._it;
        return *this;
    }

    iterator_adapter& operator=(iterator_adapter&& other) noexcept (std::is_nothrow_copy_assignable_v<MyIter>) {
        _it = std::move(other._it);
        return *this;
    }

    bool operator==(const iterator_adapter& other) const {
        return _it == other._it;
    }

    iterator_adapter& operator++() requires std::input_iterator<MyIter> {
        ++_it;
        return *this;
    }

    iterator_adapter operator++(int) requires std::input_iterator<MyIter> {
        iterator_adapter tmp = *this;
        ++(*this);
        return tmp;
    }

    iterator_adapter& operator--() requires std::bidirectional_iterator<MyIter> {
        --_it;
        return *this;
    }

    iterator_adapter operator--(int) requires std::bidirectional_iterator<MyIter> {
        iterator_adapter tmp = *this;
        --(*this);
        return tmp;
    }

    reference operator*() const requires std::indirectly_readable<MyIter> {
        return IterTransformFunc(_it);
    }

    pointer operator->() const requires std::indirectly_readable<MyIter> {
        // may be naive but sufficient for now
        return std::addressof(*(*this));
    }

    iterator_adapter operator+(difference_type n) const requires std::random_access_iterator<MyIter> {
        return iterator_adapter(_it + n);
    }

    friend iterator_adapter operator+(difference_type n, const iterator_adapter& it) requires std::random_access_iterator<MyIter> {
        return it + n;
    }

    iterator_adapter operator-(difference_type n) const requires std::random_access_iterator<MyIter> {
        return iterator_adapter(_it - n);
    }

    friend iterator_adapter operator-(difference_type n, const iterator_adapter& it) requires std::random_access_iterator<MyIter> {
        return it - n;
    }

    iterator_adapter& operator+=(difference_type n) requires std::random_access_iterator<MyIter> {
        _it += n;
        return *this;
    }

    iterator_adapter& operator-=(difference_type n) requires std::random_access_iterator<MyIter> {
        _it -= n;
        return *this;
    }

    difference_type operator-(const iterator_adapter& other) const requires std::random_access_iterator<MyIter> {
        return _it - other._it;
    }

    reference operator[](difference_type n) const requires std::random_access_iterator<MyIter> {
        return IterTransformFunc(_it + n);
    }

    bool operator<(const iterator_adapter& other) const requires std::random_access_iterator<MyIter> {
        return _it < other;
    }

    bool operator>(const iterator_adapter& other) const requires std::random_access_iterator<MyIter> {
        return _it > other;
    }

    bool operator<=(const iterator_adapter& other) const requires std::random_access_iterator<MyIter> {
        return _it <= other;
    }

    bool operator>=(const iterator_adapter& other) const requires std::random_access_iterator<MyIter> {
        return _it >= other;
    }
};

} // namespace tools

#endif//CPPTOOLS__CONTAINER__ITERATOR__ITERATOR_ADAPTER_HPP