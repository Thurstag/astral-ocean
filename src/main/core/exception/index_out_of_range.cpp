// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "index_out_of_range.h"

#include <fmt/format.h>

ao::core::IndexOutOfRangeException::IndexOutOfRangeException(std::optional<std::pair<u32, u32>> const& range)
    : ao::core::Exception(fmt::format("Index out of range{0}", range ? fmt::format(", range is: {0} -> {1}", range->first, range->second) : "")) {}
