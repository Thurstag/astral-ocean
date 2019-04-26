// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "exception.h"

namespace ao::core {
    /**
     * @brief File not found exception
     *
     */
    class FileNotFoundException : public Exception {
       public:
        /**
         * @brief Construct a new FileNotFound Exception object
         *
         * @param filename
         */
        FileNotFoundException(std::string filename);

        /**
         * @brief Destroy the FileNotFound Exception object
         *
         */
        ~FileNotFoundException() = default;
    };

}  // namespace ao::core