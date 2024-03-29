#ifndef TESTS__CATCH2_CUSTOM_GENERATORS_HPP
#define TESTS__CATCH2_CUSTOM_GENERATORS_HPP

#include <catch2/catch_all.hpp>

#include <cstddef>
#include <algorithm>
#include <memory>
#include <vector>
#include <array>

namespace Catch {

namespace Generators {

template<typename T>
class array_generator : public IGenerator<T> {
private:
    T* _copied_array;
    std::size_t _pos;
    std::size_t _count;

public:
    array_generator(const T* const array, std::size_t size) :
        _copied_array(new T[size]),
        _pos(0),
        _count(size)
    {
        std::copy(array, array + size, _copied_array);
    }

    virtual ~array_generator() {
        delete[] _copied_array;
    }

    virtual bool next() {
        return ++_pos < _count;
    }

    virtual T const& get() const {
        return _copied_array[_pos];
    }
};

template<typename T, int N>
class std_array_generator : public IGenerator<T> {
private:
    std::array<T, N> _copied_array;
    std::size_t _pos;
    std::size_t _count;

public:
    std_array_generator(const std::array<T, N> array) :
        _copied_array(array),
        _pos(0),
        _count(N)
    {
    }

    virtual ~std_array_generator() = default;
    virtual bool next() {
        return ++_pos < _count;
    }

    virtual T const& get() const {
        return _copied_array[_pos];
    }

};

template<typename T>
class vector_generator : public IGenerator<T> {
private:
    std::vector<T> _copied_vector;
    std::size_t _pos;

public:
    vector_generator(const std::vector<T>& vector, std::size_t size = 0) :
        _copied_vector(vector.begin(), vector.begin() + size),
        _pos(0)
    {
    }

    virtual ~vector_generator() = default;
    virtual bool next()  {
        return ++_pos < _copied_vector.size();
    }

    virtual T const& get() const{
        return _copied_vector[_pos];
    }

};

} // namespace Generators

} // namespace Catch

template<typename T>
Catch::Generators::GeneratorWrapper<T> vector(std::vector<T> vector, std::size_t size = 0)
{
    return Catch::Generators::GeneratorWrapper<T>(std::make_unique<Catch::Generators::vector_generator<T>>(vector, size));
}

template<typename T>
Catch::Generators::GeneratorWrapper<T> array(const T* const array, std::size_t size)
{
    return Catch::Generators::GeneratorWrapper<T>(std::make_unique<Catch::Generators::array_generator<T>>(array, size));
}

template<typename T, std::size_t N>
Catch::Generators::GeneratorWrapper<T> std_array(const std::array<T, N> array)
{
    return Catch::Generators::GeneratorWrapper<T>(std::make_unique<Catch::Generators::std_array_generator<T, N>>(array));
}

#endif//TESTS__CATCH2_CUSTOM_GENERATORS_HPP