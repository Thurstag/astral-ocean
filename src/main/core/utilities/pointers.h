// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "../exception/expiration_exception.hpp"

namespace ao {
	namespace core {
		/// <summary>
		/// Method to get shared_ptr from a weak_ptr. Throw an exception if	weak_ptr is expired
		/// </summary>
		template<class T>
		inline std::shared_ptr<T> shared(std::weak_ptr<T> ptr) {
			if (auto _ptr = ptr.lock()) {
				return _ptr;
			}
			throw ExpirationException<T>(ptr);
		}
	}
}
