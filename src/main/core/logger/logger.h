#pragma once

#include <functional>
#include <iostream>
#include <typeinfo>
#include <string>
#include <vector>
#include <map>

#include <log4cpp/OstreamAppender.hh>
#include <boost/algorithm/string.hpp>
#include <log4cpp/PatternLayout.hh>
#include <boost/core/demangle.hpp>
#include <log4cpp/Appender.hh>
#include <log4cpp/Category.hh>

using LogLevel = log4cpp::Priority;

namespace ao {
	namespace core {
		/// <summary>
		/// Wrapper for log4cpp::Category class
		/// </summary>
		class Logger {
			friend std::function<void(Logger&)>;
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			Logger() = default;

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="type">Type</param>
			Logger(std::type_info const& type);
			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~Logger() = default;

			/// <summary>
			/// Method to get a logger instance
			/// </summary>
			template<class T>
			inline static Logger getInstance() {
				if (Logger::instances.find(typeid(T).name()) == Logger::instances.end()) {
					Logger::instances[typeid(T).name()] = Logger(typeid(T));
				}
				return Logger::instances[typeid(T).name()];
			};

			/// <summary>
			/// Method to define << operator
			/// </summary>
			/// <param name="priority">Priority</param>
			/// <returns>CategoryStream</returns>
			log4cpp::CategoryStream operator<<(log4cpp::Priority::Value priority);

			/// <summary>
			/// Method to set minimum log level
			/// </summary>
			/// <param name="level">Level</param>
			static void SetMinLevel(log4cpp::Priority::Value level);
		private:
			static std::map<std::string, Logger> instances;
			static log4cpp::Priority::Value level;

			log4cpp::Category* category;

			/// <summary>
			/// Method to update loggers
			/// </summary>
			/// <param name="function">Function</param>
			static void Update(std::function<void(Logger&)> function);
		};
	}
}
