#pragma once

#include <map>

#include "iterable_container.hpp"

namespace ao {
	namespace core {
		template<class T, class U, class V>
		class MapContainer : public IterableContainer<T, U, V> {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			MapContainer() = default;

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~MapContainer() = default;

			virtual U& operator[](T index) override;
			virtual bool remove(T index) override;
			virtual bool exists(T index) override;
			virtual void clear() override;
			V begin() override;
			V end() override;

		protected:
			std::map<T, U> map;
		};

		/* IMPLEMENTATION */

		template<class T, class U, class V>
		U& MapContainer<T, U, V>::operator[](T index) {
			return this->map[index];
		}

		template<class T, class U, class V>
		bool MapContainer<T, U, V>::remove(T index) {
			if (!this->exists(index)) {
				return false;
			}
			this->map.erase(this->map.find(index));
			return true;
		}

		template<class T, class U, class V>
		bool MapContainer<T, U, V>::exists(T index) {
			return this->map.find(index) != this->map.end();
		}

		template<class T, class U, class V>
		void MapContainer<T, U, V>::clear() {
			this->map.clear();
		}

		template<class T, class U, class V>
		V MapContainer<T, U, V>::begin() {
			return this->map.begin();
		}

		template<class T, class U, class V>
		V MapContainer<T, U, V>::end() {
			return this->map.end();
		}
	}
}
