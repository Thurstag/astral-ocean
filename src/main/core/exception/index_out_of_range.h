// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <tuple>

#include <boost/optional.hpp>
#include <fmt/format.h>

#include "../utilities/types.h"
#include "exception.h"

namespace ao {
	namespace core {
		class IndexOutOfRangeException : public Exception {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="range">Range</param>
			explicit IndexOutOfRangeException(boost::optional<std::pair<u64, u64>> range = boost::none);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~IndexOutOfRangeException() = default;
		};
	}
}
