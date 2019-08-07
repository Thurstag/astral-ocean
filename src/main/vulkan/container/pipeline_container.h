// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/memory/map_container.hpp>

#include "../pipeline/pipeline.h"

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
        PipelineContainer(PipelineContainer const&) = delete;

        /**
         * @brief Destroy the PipelineContainer object
         *
         */
        virtual ~PipelineContainer();

        /**
         * @brief Set a callback (for each pipeline) that will be executed before pipeline cache destruction
         *
         * @param callback Callback
         */
        void setBeforePipelineCacheDestruction(std::function<void(std::string, vk::PipelineCache)> callback);

        PipelineContainer& operator=(PipelineContainer const&) = delete;
        virtual void clear() override;
    };

}  // namespace ao::vulkan
