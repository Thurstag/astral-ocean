// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/memory/map_container.hpp>

#include "../wrapper/pipeline/pipeline.h"

namespace ao::vulkan {
    /**
     * @brief Pipelines container
     *
     */
    class PipelineContainer : public core::MapContainer<std::string, Pipeline*> {
       public:
        /**
         * @brief Construct a new PipelineContainer object
         *
         */
        PipelineContainer() = default;

        /**
         * @brief Destroy the PipelineContainer object
         *
         */
        virtual ~PipelineContainer();

        virtual void clear() override;
    };

}  // namespace ao::vulkan
