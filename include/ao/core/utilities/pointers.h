// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "../exception/expiration_exception.hpp"

namespace ao::core {
    /**
     * @brief Extract shared_ptr from a weak_ptr (Throws an exception if	weak_ptr is expired)
     *
     * @tparam T Pointer object type
     * @param ptr Pointer
     * @return std::shared_ptr<T> Shared pointer
     */
    template<class T>
    inline std::shared_ptr<T> shared(std::weak_ptr<T> const& ptr) {
        if (auto _ptr = ptr.lock()) {
            return _ptr;
        }
        throw ExpirationException<T>(ptr);
    }
}  // namespace ao::core
