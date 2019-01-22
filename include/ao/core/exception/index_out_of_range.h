// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <optional>
#include <tuple>

#include "../utilities/types.h"
#include "exception.h"

namespace ao::core {
	class IndexOutOfRangeException : public Exception {
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="range">Range</param>
		explicit IndexOutOfRangeException(std::optional<std::pair<u64, u64>> const& range = std::nullopt);

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~IndexOutOfRangeException() = default;
	};
}
