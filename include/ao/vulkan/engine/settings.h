// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <map>

#include <ao/core/exception/exception.h>
#include <ao/core/logging/log.h>
#include <ao/core/utilities/optional.h>
#include <fmt/format.h>

namespace ao::vulkan {
    namespace settings {
        static constexpr char const* SurfaceWidth = "surface.width";
        static constexpr char const* SurfaceHeight = "surface.height";

        static constexpr char const* WindowVsync = "window.vsync";

        static constexpr char const* AppName = "application.name";
        static constexpr char const* AppVersion = "application.version";

        static constexpr char const* EngineName = "engine.name";
        static constexpr char const* EngineVersion = "engine.version";

        static constexpr char const* ValidationLayers = "vulkan.validation_layers";
        static constexpr char const* StencilBuffer = "vulkan.stencil_buffer";
    };  // namespace settings

    /**
     * @brief Engine settings
     *
     */
    class EngineSettings {
       public:
        /**
         * @brief Construct a new EngineSettings object
         *
         */
        EngineSettings() = default;
        EngineSettings(EngineSettings const&) = delete;

        /**
         * @brief Destroy the EngineSettings object
         *
         */
        virtual ~EngineSettings();

        /**
         * @brief Value exists
         *
         * @param key Key
         * @return true Value exists
         * @return false Value doesn't exist
         */
        bool exists(std::string const& key) const {
            return this->value_exists(key) || this->str_exists(key);
        }

        /**
         * @brief Get value with a specified key
         *
         * @tparam T Value type
         * @param key Key
         * @param _default Default value
         * @return T& Value
         */
        template<class T>
        T& get(std::string const& key, std::optional<T> _default = std::nullopt) {
            constexpr bool is_string = std::is_base_of<std::string, T>::value;

            if constexpr (is_string) {
                if (!this->str_exists(key) && _default) {
                    this->str_values[key] = *_default;
                }
            } else {
                if (!this->value_exists(key)) {
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
                    LOG_MSG(warning) << fmt::format("Cast {} into {}", this->values[key].first.second, typeid(T*).name());
                }
            }

            // Return value
            if constexpr (is_string) {
                return this->str_values[key];
            }
            return *static_cast<T*>(this->values[key].second);
        }

        EngineSettings& operator=(EngineSettings const&) = delete;

       protected:
        std::map<std::string, std::pair<std::pair<size_t, std::string>, void*>> values;
        std::map<std::string, std::string> str_values;

        /**
         * @brief Value exists in str_values
         *
         * @param key Key
         * @return true Value exists
         * @return false Value doesn't exist
         */
        bool str_exists(std::string const& key) const;

        /**
         * @brief Value exists in values
         *
         * @param key Key
         * @return true Value exists
         * @return false Value doesn't exist
         */
        bool value_exists(std::string const& key) const;
    };
}  // namespace ao::vulkan
