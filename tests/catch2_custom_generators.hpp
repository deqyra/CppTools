#ifndef TESTS__CATCH2_CUSTOM_GENERATORS_HPP
#define TESTS__CATCH2_CUSTOM_GENERATORS_HPP

#include <catch2/catch2.hpp>

#include <cstddef>
#include <iterator>
#include <algorithm>
#include <memory>
#include <vector>
#include <tuple>
#include <array>

namespace Catch
{
namespace Generators
{
    template<typename T>
    class array_generator : public IGenerator<T>
    {
        private:
            T* _copied_array;
            std::size_t _pos;
            std::size_t _count;

        public:
            array_generator(const T* const array, std::size_t size);
            virtual ~array_generator();
            virtual bool next();
            virtual T const& get() const;
    };

    template<typename T, int N>
    class std_array_generator : public IGenerator<T>
    {
        private:
            std::array<T, N> _copied_array;
            std::size_t _pos;
            std::size_t _count;

        public:
            std_array_generator(const std::array<T, N> array);
            virtual ~std_array_generator();
            virtual bool next();
            virtual T const& get() const;
    };

    template<typename T>
    class vector_generator : public IGenerator<T>
    {
        private:
            std::vector<T> _copied_vector;
            std::size_t _pos;

        public:
            vector_generator(const std::vector<T> vector, std::size_t size = 0);
            virtual ~vector_generator();
            virtual bool next();
            virtual T const& get() const;
    };

    template<typename T>
    array_generator<T>::array_generator(const T* const array, std::size_t size) :
        _copied_array(new T[size]),
        _pos(0),
        _count(size)
    {
        std::copy(array, array + size, _copied_array);
    }

    template<typename T>
    array_generator<T>::~array_generator()
    {
        delete[] _copied_array;
    }

    template<typename T>
    bool array_generator<T>::next()
    {
        return ++_pos < _count;
    }

    template<typename T>
    T const& array_generator<T>::get() const
    {
        return _copied_array[_pos];
    }

    template<typename T, int N>
    std_array_generator<T, N>::std_array_generator(const std::array<T, N> array) :
        _copied_array(array),
        _pos(0),
        _count(N)
    {
    }

    template<typename T, int N>
    std_array_generator<T, N>::~std_array_generator()
    {
    }

    template<typename T, int N>
    bool std_array_generator<T, N>::next()
    {
        return ++_pos < _count;
    }

    template<typename T, int N>
    T const& std_array_generator<T, N>::get() const
    {
        return _copied_array[_pos];
    }

    template<typename T>
    vector_generator<T>::vector_generator(const std::vector<T> vector, std::size_t size) :
        _copied_vector(vector.begin(), size == 0 ? vector.end() : vector.begin() + size),
        _pos(0)
    {
    }

    template<typename T>
    vector_generator<T>::~vector_generator()
    {
    }

    template<typename T>
    bool vector_generator<T>::next()
    {
        return ++_pos < _copied_vector.size();
    }

    template<typename T>
    T const& vector_generator<T>::get() const
    {
        return _copied_vector[_pos];
    }
} // namespace Generators
} // namespace Catch

template<typename T>
Catch::Generators::generator_wrapper<T> vector(std::vector<T> vector, std::size_t size = 0)
{
    return Catch::Generators::generator_wrapper<T>(std::unique_ptr<Catch::Generators::IGenerator<T>>(new Catch::Generators::vector_generator<T>(vector, size)));
}

template<typename T>
Catch::Generators::generator_wrapper<T> array(const T* const array, std::size_t size)
{
    return Catch::Generators::generator_wrapper<T>(std::unique_ptr<Catch::Generators::IGenerator<T>>(new Catch::Generators::array_generator<T>(array, size)));
}

template<typename T, int N>
Catch::Generators::generator_wrapper<T> std_array(const std::array<T, N> array)
{
    return Catch::Generators::generator_wrapper<T>(std::unique_ptr<Catch::Generators::IGenerator<T>>(new Catch::Generators::std_array_generator<T, N>(array)));
}

#endif//TESTS__CATCH2_CUSTOM_GENERATORS_HPP