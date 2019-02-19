// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <map>

#include "iterable_container.hpp"

namespace ao::core {
    /**
     * @brief Container with a map implementation
     *
     * @tparam Key Key
     * @tparam Value Value
     */
    template<class Key, class Value>
    class MapContainer : public IterableContainer<Key, Value, typename std::map<Key, Value>::iterator> {
       public:
        /**
         * @brief Construct a new MapContainer object
         *
         */
        MapContainer() = default;

        /**
         * @brief Destroy the MapContainer object
         *
         */
        virtual ~MapContainer() = default;

        virtual Value& operator[](Key const& key) override;
        virtual Value& at(Key const& key) override;
        virtual bool remove(Key const& key) override;
        virtual bool exists(Key const& key) const override;
        virtual void clear() override;
        virtual size_t size() const override;
        typename std::map<Key, Value>::iterator begin() override;
        typename std::map<Key, Value>::iterator end() override;

       protected:
        std::map<Key, Value> map;
    };

    template<class Key, class Value>
    inline Value& MapContainer<Key, Value>::operator[](Key const& key) {
        return this->map[key];
    }

    template<class Key, class Value>
    inline Value& MapContainer<Key, Value>::at(Key const& key) {
        return this->map.at(key);
    }

    template<class Key, class Value>
    inline bool MapContainer<Key, Value>::remove(Key const& key) {
        if (!this->exists(key)) {
            return false;
        }
        this->map.erase(this->map.find(key));
        return true;
    }

    template<class Key, class Value>
    inline bool MapContainer<Key, Value>::exists(Key const& key) const {
        return this->map.find(key) != this->map.end();
    }

    template<class Key, class Value>
    inline void MapContainer<Key, Value>::clear() {
        this->map.clear();
    }

    template<class Key, class Value>
    inline size_t MapContainer<Key, Value>::size() const {
        return this->map.size();
    }

    template<class Key, class Value>
    inline typename std::map<Key, Value>::iterator MapContainer<Key, Value>::begin() {
        return this->map.begin();
    }

    template<class Key, class Value>
    inline typename std::map<Key, Value>::iterator MapContainer<Key, Value>::end() {
        return this->map.end();
    }
}  // namespace ao::core
