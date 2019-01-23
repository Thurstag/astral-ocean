// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <map>

#include "container.hpp"

namespace ao::core {
    /// <summary>
    /// Container with iterable features
    /// </summary>
    template<class Key, class Value, class Iterator>
    class IterableContainer : public Container<Key, Value> {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        IterableContainer() = default;

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~IterableContainer() = default;

        /// <summary>
        /// Method to get begin iterator
        /// </summary>
        /// <returns></returns>
        virtual Iterator begin() = 0;

        /// <summary>
        /// Method to get end iterator
        /// </summary>
        /// <returns></returns>
        virtual Iterator end() = 0;
    };
}  // namespace ao::core
