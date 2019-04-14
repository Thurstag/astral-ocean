// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <tuple>

#include "../utilities/optional.h"
#include "../utilities/types.h"
#include "exception.h"

namespace ao::core {
    /**
     * @brief Index out of range exception
     *
     */
    class IndexOutOfRangeException : public Exception {
       public:
        /**
         * @brief Construct a new IndexOutOfRangeException object
         *
         * @param range Range
         */
        explicit IndexOutOfRangeException(std::optional<std::pair<u32, u32>> const& range = std::nullopt);

        /**
         * @brief Destroy the IndexOutOfRangeException object
         *
         */
        virtual ~IndexOutOfRangeException() = default;
    };
}  // namespace ao::core
