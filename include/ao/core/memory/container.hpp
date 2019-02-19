// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

namespace ao::core {
    /**
     * @brief Container
     *
     * @tparam Key Key type
     * @tparam Value Value type
     */
    template<class Key, class Value>
    class Container {
       public:
        /**
         * @brief Construct a new Container object
         *
         */
        Container() = default;

        /**
         * @brief Destroy the Container object
         *
         */
        virtual ~Container() = default;

        /**
         * @brief Operator[]
         *
         * @param key Key
         * @return Value& Value
         */
        virtual Value& operator[](Key const& key) = 0;

        /**
         * @brief Operator[] with bounds verification
         *
         * @param key Key
         * @return Value& Value
         */
        virtual Value& at(Key const& key) = 0;

        /**
         * @brief Remove an object
         *
         * @param key Key
         * @return true Object is removed
         * @return false Object isn't removed
         */
        virtual bool remove(Key const& key) = 0;

        /**
         * @brief Object exists
         *
         * @param key Key
         * @return true Object exists
         * @return false Object doesn't exist
         */
        virtual bool exists(Key const& key) const = 0;

        /**
         * @brief Clear container
         *
         */
        virtual void clear() = 0;

        /**
         * @brief Container's size
         *
         * @return size_t Size
         */
        virtual size_t size() const = 0;
    };
}  // namespace ao::core
