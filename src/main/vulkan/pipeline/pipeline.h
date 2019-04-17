// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "descriptor_pool.h"
#include "pipeline_layout.h"

namespace ao::vulkan {
    /**
     * @brief vk::Pipeline wrapper
     *
     */
    class Pipeline {
       public:
        /**
         * @brief Construct a new Pipeline object
         *
         */
        Pipeline(){};

        /**
         * @brief Construct a new Pipeline object
         *
         * @param device Device
         * @param layout Layout
         * @param pipeline Pipeline
         * @param cache_create_info Pipeline cache create info
         */
        Pipeline(std::shared_ptr<vk::Device> device, std::shared_ptr<PipelineLayout> layout, vk::Pipeline pipeline,
                 vk::PipelineCacheCreateInfo cache_create_info);

        /**
         * @brief Destroy the Pipeline object
         *
         */
        virtual ~Pipeline();

        /**
         * @brief Get descriptor pools
         *
         * @return std::vector<DescriptorPool>& Pools
         */
        std::vector<DescriptorPool>& pools() {
            return this->pools_;
        }

        /**
         * @brief Get pipeline layout
         *
         * @return std::shared_ptr<PipelineLayout> Pipeline layout
         */
        std::shared_ptr<PipelineLayout> layout() {
            return this->layout_;
        }

        /**
         * @brief Get pipeline
         *
         * @return vk::Pipeline Pipeline
         */
        vk::Pipeline value() const {
            return this->pipeline;
        }

        /**
         * @brief Set a callback that will be executed before pipeline cache destruction
         *
         * @param callback Callback
         */
        void setBeforePipelineCacheDestruction(std::function<void(vk::PipelineCache)> callback) {
            this->before_cache_destruction = callback;
        }

       protected:
        std::optional<std::function<void(vk::PipelineCache)>> before_cache_destruction;
        std::shared_ptr<vk::Device> device;

        std::shared_ptr<PipelineLayout> layout_;
        std::vector<DescriptorPool> pools_;
        vk::PipelineCache cache;
        vk::Pipeline pipeline;
    };

}  // namespace ao::vulkan