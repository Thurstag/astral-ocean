// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <map>
#include <optional>

#include <ao/core/exception/exception.h>
#include <ao/core/logger/logger.h>
#include <fmt/format.h>

namespace ao::vulkan {
    namespace settings {
        constexpr char const* WindowWidth = "window.width";
        constexpr char const* WindowHeight = "window.height";
        constexpr char const* WindowVsync = "window.vsync";

        constexpr char const* AppName = "application.name";
        constexpr char const* AppVersion = "application.version";

        constexpr char const* EngineName = "engine.name";
        constexpr char const* EngineVersion = "engine.version";

        constexpr char const* ValidationLayers = "vulkan.validation_layers";
        constexpr char const* StencilBuffer = "vulkan.stencil_buffer";
    };  // namespace settings

    /// <summary>
    /// EngineSettings class
    /// </summary>
    class EngineSettings {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        EngineSettings() = default;

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~EngineSettings();

        /// <summary>
        /// Method to know if existing value with specified key
        /// </summary>
        /// <param name="key">Key</param>
        /// <returns>Exist</returns>
        bool exists(std::string const& key) const;

        /// <summary>
        ///  Method to get value with a specified key
        /// </summary>
        /// <param name="key">Key</param>
        /// <param name="_default">Default value returned</param>
        /// <returns>Value</returns>
        template<class T>
        T& get(std::string const& key, std::optional<T> _default = std::nullopt) {
            constexpr bool is_string = std::is_base_of<std::string, T>::value;

            if (!this->exists(key)) {
                if constexpr (is_string) {
                    if (_default) {
                        this->str_values[key] = *_default;
                    }
                } else {
                    // Allocate
                    T* ptr = static_cast<T*>(calloc(1, sizeof(T)));

                    // Create value
                    this->values[key] = std::make_pair(std::make_pair(typeid(T*).hash_code(), typeid(T*).name()), ptr);

                    // Assign
                    if (_default) {
                        *ptr = *_default;
                    }
                }
            }

            // Check type
            if constexpr (!is_string) {
                if (this->values[key].first.first != typeid(T*).hash_code()) {
                    this->LOGGER << core::Logger::Level::warning << fmt::format("Cast {} into {}", this->values[key].first.second, typeid(T*).name());
                }
            }

            // Return value
            if constexpr (is_string) {
                return this->str_values[key];
            }
            return *static_cast<T*>(this->values[key].second);
        }

       protected:
        core::Logger LOGGER = core::Logger::GetInstance<EngineSettings>();

        std::map<std::string, std::pair<std::pair<size_t, std::string>, void*>> values;
        std::map<std::string, std::string> str_values;
    };
}  // namespace ao::vulkan
