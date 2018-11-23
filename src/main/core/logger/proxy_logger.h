#pragma once

#include <string>

#include <boost/log/trivial.hpp>
#include <fmt/format.h>

#include "../exception/exception.h"

namespace ao {
	namespace core {
		struct LoggerData {
			std::string key;

			explicit LoggerData(std::string _key) : key(_key) {}
		};

		class ProxyLogger {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="_level">Level</param>
			/// <param name="_data">Data</param>
			explicit ProxyLogger(boost::log::trivial::severity_level _level, LoggerData _data) : level(_level), data(_data) {}

			/// <summary>
			/// Destructor
			/// </summary>
			~ProxyLogger() = default;

			/// <summary>
			/// Operator << 
			/// </summary>
			/// <param name="message">Message</param>
			void operator<<(std::string message);

			/// <summary>
			/// Operator <<
			/// </summary>
			/// <param name="message">Message</param>
			void operator<<(const char* message);

			/// <summary>
			/// Operator <<
			/// </summary>
			/// <param name="exception">Exception</param>
			void operator<<(core::Exception& exception);

		protected:
			boost::log::trivial::severity_level level;
			LoggerData data;

			/// <summary>
			/// Method to log a message
			/// </summary>
			/// <param name="message">Message</param>
			void log(const char* message);
		};
	}
}
