// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#ifdef __linux__
#define BOOST_LOG_DYN_LINK 1
#endif

#include "logger_instance.hpp"

namespace ao::core {
    /// <summary>
    /// Wrapper for Boost::log
    /// </summary>
    class Logger {
        friend std::function<void(Logger&)>;

       public:
        using Level = boost::log::trivial::severity_level;

        /// <summary>
        /// Constructor
        /// </summary>
        Logger() = default;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="_class">Class</param>
        explicit Logger(std::type_info const& _class);

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~Logger() = default;

        /// <summary>
        /// Method to get a logger instance
        /// </summary>
        template<class T>
        inline static Logger GetInstance() {
            return Logger(typeid(T));
        };

        /// <summary>
        /// Method to init logger
        /// </summary>
        static void Init();

        /// <summary>
        /// Operator <<
        /// </summary>
        /// <param name="level">Level</param>
        /// <returns>LoggerInstance</returns>
        LoggerInstance operator<<(Level const level) const;

        /// <summary>
        /// Method to set minimum level
        /// </summary>
        /// <param name="level">Level</param>
        static void SetMinLevel(Level const level);

       protected:
        LoggerInstance::Data data;
    };
}  // namespace ao::core