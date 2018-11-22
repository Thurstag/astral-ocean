#include "index_out_of_range.h"

ao::core::IndexOutOfRangeException::IndexOutOfRangeException(boost::optional<std::pair<u64, u64>> range) : 
	ao::core::Exception(fmt::format("Index out of range{0}", range ? fmt::format(", range is: {0} -> {1}", range->first, range->second) : "")) {}
