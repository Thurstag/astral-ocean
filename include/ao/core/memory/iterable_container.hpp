// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <map>

#include "container.hpp"

namespace ao::core {
    /**
     * @brief Container with iterable features
     *
     * @tparam Key Key
     * @tparam Value Value
     * @tparam Iterator Iterator
     */
    template<class Key, class Value, class Iterator>
    class IterableContainer : public Container<Key, Value> {
       public:
        /**
         * @brief Construct a new IterableContainer object
         *
         */
        IterableContainer() = default;

        /**
         * @brief Destroy the IterableContainer object
         *
         */
        virtual ~IterableContainer() = default;

        /**
         * @brief Begin iterator
         *
         * @return Iterator Iterator
         */
        virtual Iterator begin() = 0;

        /**
         * @brief End iterator
         *
         * @return Iterator Iterator
         */
        virtual Iterator end() = 0;
    };
}  // namespace ao::core
