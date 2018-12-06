// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <map>

#include "iterable_container.hpp"

namespace ao::core {
	template<class T, class U>
	class MapContainer : public IterableContainer<T, U, typename std::map<T, U>::iterator> {
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		MapContainer() = default;

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~MapContainer() = default;

		virtual U& operator[](T const& index) override;
		virtual bool remove(T const& index) override;
		virtual bool exists(T const& index) const override;
		virtual void clear() override;
		virtual size_t size() const override;
		typename std::map<T, U>::iterator begin() override;
		typename std::map<T, U>::iterator end() override;

	protected:
		std::map<T, U> map;
	};

	/* IMPLEMENTATION */

	template<class T, class U>
	U& MapContainer<T, U>::operator[](T const& index) {
		return this->map[index];
	}

	template<class T, class U>
	bool MapContainer<T, U>::remove(T const& index) {
		if (!this->exists(index)) {
			return false;
		}
		this->map.erase(this->map.find(index));
		return true;
	}

	template<class T, class U>
	bool MapContainer<T, U>::exists(T const& index) const {
		return this->map.find(index) != this->map.end();
	}

	template<class T, class U>
	void MapContainer<T, U>::clear() {
		this->map.clear();
	}

	template<class T, class U>
	size_t MapContainer<T, U>::size() const {
		return this->map.size();
	}

	template<class T, class U>
	typename std::map<T, U>::iterator MapContainer<T, U>::begin() {
		return this->map.begin();
	}

	template<class T, class U>
	typename std::map<T, U>::iterator MapContainer<T, U>::end() {
		return this->map.end();
	}
}
