// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <fmt/format.h>
#include <memory>

#include "exception.h"

namespace ao::core {
    /**
     * @brief Weak pointer expiration exception
     *
     * @tparam T weak_ptr template type
     */
    template<class T>
    class ExpirationException : public Exception {
       public:
        /**
         * @brief Construct a new ExpirationException object
         *
         * @param pointer Pointer
         */
        explicit ExpirationException(std::weak_ptr<T> const& pointer);

        /**
         * @brief Destroy the ExpirationException object
         *
         */
        virtual ~ExpirationException() = default;
    };

    template<class T>
    inline ExpirationException<T>::ExpirationException(std::weak_ptr<T> const& pointer)
        : Exception(fmt::format("{0} pointer is expired", typeid(T).name())) {}
}  // namespace ao::core
