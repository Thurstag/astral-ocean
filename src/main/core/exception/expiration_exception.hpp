#pragma once

#include <memory>

#include <fmt/format.h>

#include "exception.h"

namespace ao {
	namespace core {
		/// <summary>
		/// Exception for weak_ptr expiration
		/// </summary>
		template<class T>
		class ExpirationException : public Exception {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			ExpirationException(std::weak_ptr<T> pointer);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~ExpirationException() = default;
		};

		/* IMPLEMENTATION */

		template<class T>
		ExpirationException<T>::ExpirationException(std::weak_ptr<T> pointer) : Exception(fmt::format("{0} pointer is expired", typeid(T).name())) {}
	}
}
