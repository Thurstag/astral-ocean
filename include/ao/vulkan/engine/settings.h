// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <string>
#include <thread>

#include <ao/core/utilities/types.h>

namespace ao::vulkan {
    struct WindowSettings {
        std::string name;

        u64 width;
        u64 height;

        bool rezisable = true;
        bool vsync = false;

        /// <summary>
        /// Constructor
        /// </summary>
        WindowSettings() = default;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="name">Window's name</param>
        /// <param name="width">Window's width</param>
        /// <param name="height">Window's height</param>
        /// <param name="rezisable">Window is rezisable</param>
        /// <param name="vsync">V-Sync is enabled</param>
        explicit WindowSettings(std::string const& name, u64 const& width, u64 const& height, bool rezisable = true, bool vsync = false)
            : name(name), width(width), height(height), rezisable(rezisable), vsync(vsync) {}
    };

    struct CoreSettings {
        int threadPoolSize = std::thread::hardware_concurrency();
        bool validationLayers = false;

        /// <summary>
        /// Constructor
        /// </summary>
        CoreSettings() = default;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="threadPoolSize">Thread pool's size</param>
        /// <param name="validationLayers">Enable validation layers</param>
        explicit CoreSettings(int threadPoolSize = std::thread::hardware_concurrency(), bool _validationLayers = false)
            : threadPoolSize(threadPoolSize), validationLayers(validationLayers) {}
    };

    struct EngineSettings {
        WindowSettings window;
        CoreSettings core;

        /// <summary>
        /// Constructor
        /// </summary>
        EngineSettings() = default;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="_window">Window settings</param>
        /// <param name="_core">Core settings</param>
        explicit EngineSettings(WindowSettings const& _window, CoreSettings const& _core) : window(_window), core(_core) {}
    };
}  // namespace ao::vulkan
