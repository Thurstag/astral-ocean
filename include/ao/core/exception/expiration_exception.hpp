// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <fmt/format.h>
#include <memory>

#include "exception.h"

namespace ao::core {
    /// <summary>
    /// Exception for weak_ptr expiration
    /// </summary>
    template<class T>
    class ExpirationException : public Exception {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        explicit ExpirationException(std::weak_ptr<T> const& pointer);

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~ExpirationException() = default;
    };

    template<class T>
    inline ExpirationException<T>::ExpirationException(std::weak_ptr<T> const& pointer)
        : Exception(fmt::format("{0} pointer is expired", typeid(T).name())) {}
}  // namespace ao::core
