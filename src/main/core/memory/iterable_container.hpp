// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <map>

#include "container.hpp"

namespace ao::core {
	template<class T, class U, class V>
	class IterableContainer : public Container<T, U> {
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		IterableContainer() = default;

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~IterableContainer() = default;

		/// <summary>
		/// Method to get begin iterator
		/// </summary>
		/// <returns></returns>
		virtual V begin() = 0;

		/// <summary>
		/// Method to get end iterator
		/// </summary>
		/// <returns></returns>
		virtual V end() = 0;
	};
}
