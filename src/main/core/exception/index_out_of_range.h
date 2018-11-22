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
			IndexOutOfRangeException(boost::optional<std::pair<u64, u64>> range = boost::none);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~IndexOutOfRangeException() = default;
		};
	}
}
