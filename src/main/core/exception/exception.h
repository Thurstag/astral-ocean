// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <boost/stacktrace.hpp>

#include "../utilities/optional.h"

namespace ao::core {
    /**
     * @brief Exeption class with stack trace
     *
     */
    class Exception : public std::runtime_error {
       public:
        /**
         * @brief Construct a new Exception object
         *
         * @param message Message
         * @param stacktrace Enable stack trace
         */
        Exception(std::string const& message, bool const stacktrace = true);

        /**
         * @brief Construct a new Exception object
         *
         * @param message Message
         * @param stacktrace Enable stack trace
         */
        Exception(char const* const message, bool const stacktrace = true);

        /**
         * @brief Destroy the Exception object
         *
         */
        virtual ~Exception() = default;

        /**
         * @brief String representation
         *
         * @return std::string String
         */
        std::string str() const;

        /**
         * @brief Operator <<
         *
         * @param os Stream
         * @param e Exception
         * @return std::ostream& Stream
         */
        friend std::ostream& operator<<(std::ostream& os, const Exception& e) {
            return os << e.str();
        }

       protected:
        std::optional<boost::stacktrace::stacktrace> stacktrace;
    };
}  // namespace ao::core
