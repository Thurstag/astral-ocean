// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <boost/stacktrace.hpp>
#include <optional>

namespace ao::core {
    /// <summary>
    /// Exception with a stack trace
    /// </summary>
    class Exception : public std::runtime_error {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="message">Messagge</param>
        /// <param name="stacktrace">Enable stack trace</param>
        Exception(std::string const& message, bool const stacktrace = true);

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="message">Messagge</param>
        /// <param name="stacktrace">Enable stack trace</param>
        Exception(char const* const message, bool const stacktrace = true);

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~Exception() = default;

        /// <summary>
        /// Method to string
        /// </summary>
        /// <returns>String representation</returns>
        std::string str() const;

        /// <summary>
        /// Method to define << operator
        /// </summary>
        /// <param name="os">Stream</param>
        /// <param name="e">Exception</param>
        /// <returns>Stream</returns>
        friend std::ostream& operator<<(std::ostream& os, const Exception& e) {
            return os << e.str();
        }

       protected:
        std::optional<boost::stacktrace::stacktrace> stack;
    };
}  // namespace ao::core
