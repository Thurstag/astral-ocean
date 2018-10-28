#pragma once

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
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="type">Type</param>
			Logger(std::type_info const& type);
			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~Logger();

			/// <summary>
			/// Method to get a logger instance
			/// </summary>
			template<class T>
			inline static Logger getInstance() {
				return ao::core::Logger(typeid(T));
			};

			/// <summary>
			/// Method to define << operator
			/// </summary>
			/// <param name="priority">Priority</param>
			/// <returns>CategoryStream</returns>
			log4cpp::CategoryStream operator<<(log4cpp::Priority::Value priority);
		private:
			log4cpp::Category* category;
		};
	}
}
