// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

namespace ao::core::utilities {
    /**
     * @brief Calculate aligned size based on a alignment
     *
     * @param size Size
     * @param alignment Alignment
     * @return size_t Aligned size
     */
    inline size_t calculateAligmentSize(size_t size, size_t alignment = 0) {
        if (alignment > 0) {
            return (size + alignment - 1) & ~(alignment - 1);
        }
        return size;
    }
}  // namespace ao::core::utilities