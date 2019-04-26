// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "file_not_found.h"

#include <fmt/format.h>

ao::core::FileNotFoundException::FileNotFoundException(std::string const& filename)
    : ao::core::Exception(fmt::format("Fail to open: {0}", filename)) {}