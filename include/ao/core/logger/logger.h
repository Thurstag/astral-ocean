// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "logger_instance.hpp"

namespace ao::core {
    namespace logger {
        constexpr char const* DefaultFormat = "[%TimeStamp%] [Thread-%ThreadID%] [%Severity%] %Message%";
    }

    /**
     * @brief Wrapper for Boost::log
     *
     */
    class Logger {
        friend std::function<void(Logger&)>;

       public:
        using Level = boost::log::trivial::severity_level;

        /**
         * @brief Construct a new Logger object
         *
         */
        Logger() = default;

        /**
         * @brief Construct a new Logger object
         *
         * @param _class Class
         */
        explicit Logger(std::type_info const& _class);

        /**
         * @brief Destroy the Logger object
         *
         */
        virtual ~Logger() = default;

        /**
         * @brief Generate a logger instance
         *
         * @tparam T Class where logger is instantiated
         * @return Logger Logger
         */
        template<class T>
        inline static Logger GetInstance() {
            return Logger(typeid(T));
        };

        /**
         * @brief Operator <<
         *
         * @param level Level
         * @return LoggerInstance LoggerInstance object
         */
        LoggerInstance operator<<(Level const level) const;

        /**
         * @brief Init logger
         *
         */
        static void Init();

        /**
         * @brief Logger is initiated
         *
         * @return true Logger is initiated
         * @return false Logger isn't initiated
         */
        static bool Initiated();

        /**
         * @brief Set the minimum level
         *
         * @param level
         */
        static void SetMinLevel(Level const level);

       protected:
        static bool initiated_;

        LoggerInstance::Data data;
    };
}  // namespace ao::core
