// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <map>

#include "iterable_container.hpp"

namespace ao::core {
    /// <summary>
    /// MapContainer class
    /// </summary>
    template<class Key, class Value>
    class MapContainer : public IterableContainer<Key, Value, typename std::map<Key, Value>::iterator> {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        MapContainer() = default;

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~MapContainer() = default;

        virtual Value& operator[](Key const& index) override;
        virtual bool remove(Key const& index) override;
        virtual bool exists(Key const& index) const override;
        virtual void clear() override;
        virtual size_t size() const override;
        typename std::map<Key, Value>::iterator begin() override;
        typename std::map<Key, Value>::iterator end() override;

       protected:
        std::map<Key, Value> map;
    };

    template<class Key, class Value>
    inline Value& MapContainer<Key, Value>::operator[](Key const& index) {
        return this->map[index];
    }

    template<class Key, class Value>
    inline bool MapContainer<Key, Value>::remove(Key const& index) {
        if (!this->exists(index)) {
            return false;
        }
        this->map.erase(this->map.find(index));
        return true;
    }

    template<class Key, class Value>
    inline bool MapContainer<Key, Value>::exists(Key const& index) const {
        return this->map.find(index) != this->map.end();
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
