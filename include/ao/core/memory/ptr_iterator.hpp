// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <iterator>

namespace ao::core {

    /**
     * @brief Iterator with a pointer
     *
     * @tparam T Pointer type
     */
    template<class T>
    class PtrIterator : public std::iterator<std::input_iterator_tag, T> {
       public:
        /**
         * @brief Construct a new PtrIterator object
         *
         * @param pointer Pointer
         * @param stride Stride (in bytes)
         */
        PtrIterator(T* pointer, size_t stride = sizeof(T)) : ptr(pointer), stride(stride) {}

        /**
         * @brief Operator++
         *
         * @return PtrIterator& Iterator
         */
        PtrIterator& operator++() {
            this->ptr = reinterpret_cast<T*>(reinterpret_cast<char*>((void*)this->ptr) + this->stride);

            return *this;
        }

        /**
         * @brief Operator++()
         *
         * @return PtrIterator Iterator
         */
        PtrIterator operator++(int) {
            PtrIterator tmp(*this);

            (*this)++;
            return tmp;
        }

        /**
         * @brief Operator==
         *
         * @param iterator Iterator
         * @return true Iterators are equal
         * @return false Iterators aren't equal
         */
        bool operator==(PtrIterator const& iterator) const {
            return this->ptr == iterator.ptr;
        }

        /**
         * @brief Operator!=
         *
         * @param iterator Iterator
         * @return true Iterators aren't equal
         * @return false Iterators are equal
         */
        bool operator!=(PtrIterator const& iterator) const {
            return this->ptr != iterator.ptr;
        }

        /**
         * @brief Operator*
         *
         * @return T& Value
         */
        T& operator*() {
            return *this->ptr;
        }

       protected:
        size_t stride;
        T* ptr;
    };
}  // namespace ao::core
