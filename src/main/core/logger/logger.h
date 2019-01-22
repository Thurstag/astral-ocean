// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#ifdef __linux__ 
#define BOOST_LOG_DYN_LINK 1
#endif

#include "proxy_logger.hpp"

namespace ao::core {
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
		/// <returns>ProxyLogger</returns>
		ProxyLogger operator<<(LogLevel const level) const;

		/// <summary>
		/// Method to set minimum level
		/// </summary>
		/// <param name="level">Level</param>
		static void SetMinLevel(LogLevel const level);
	protected:
		std::string key;
	};
}
