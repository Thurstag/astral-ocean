#pragma once

#include "../exception/expiration_exception.hpp"

namespace ao {
	namespace core {
		/// <summary>
		/// Method to get shared_ptr from a weak_ptr. Throw an exception if	weak_ptr is expired
		/// </summary>
		template<class T>
		inline std::shared_ptr<T> get(std::weak_ptr<T> ptr) {
			if (auto _ptr = ptr.lock()) {
				return _ptr;
			}
			throw ExpirationException<T>(ptr);
		}
	}
}
