// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "settings.h"

#include <algorithm>

ao::vulkan::EngineSettings::~EngineSettings() {
    for (auto& [key, value] : this->values) {
        free(value.second);
    }
}

bool ao::vulkan::EngineSettings::exists(std::string const& key) const {
    return std::find_if(this->values.begin(), this->values.end(), [&key](auto& pair) { return pair.first == key; }) != this->values.end() ||
           std::find_if(this->str_values.begin(), this->str_values.end(), [&key](auto& pair) { return pair.first == key; }) != this->str_values.end();
}
