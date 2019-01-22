// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <memory>

#include <fmt/format.h>

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

	/* IMPLEMENTATION */

	template<class T>
	ExpirationException<T>::ExpirationException(std::weak_ptr<T> const& pointer) : Exception(fmt::format("{0} pointer is expired", typeid(T).name())) {}
}
