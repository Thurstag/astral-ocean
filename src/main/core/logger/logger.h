#pragma once

#include <string>

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/core/demangle.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>

#include "proxy_logger.hpp"

namespace ao {
	namespace core {
		using LogLevel = boost::log::trivial::severity_level;

		/// <summary>
		/// Wrapper for Boost::log
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
			explicit Logger(std::type_info const& type);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~Logger() = default;

			/// <summary>
			/// Method to get a logger instance
			/// </summary>
			template<class T>
			inline static Logger getInstance() {
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
			/// <returns>ProxyLogger</returns>
			ProxyLogger operator<<(LogLevel level);

			/// <summary>
			/// Method to set minimum level
			/// </summary>
			/// <param name="level">Level</param>
			static void SetMinLevel(LogLevel level);
		protected:
			std::string key;
		};
	}
}
