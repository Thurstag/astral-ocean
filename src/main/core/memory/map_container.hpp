#pragma once

#include <map>

#include "container.hpp"

namespace ao {
	namespace core {

		template<class T, class U>
		class MapContainer : public Container<T, U> {
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
			auto begin();
			auto end();

		protected:
			std::map<T, U> map;
		};

		/* IMPLEMENTATION */

		template<class T, class U>
		U& MapContainer<T, U>::operator[](T index) {
			return this->map[index];
		}

		template<class T, class U>
		bool MapContainer<T, U>::remove(T index) {
			if (!this->exists(index)) {
				return false;
			}
			this->map.erase(this->map.find(index));
			return true;
		}

		template<class T, class U>
		bool MapContainer<T, U>::exists(T index) {
			return this->map.find(index) != this->map.end();
		}

		template<class T, class U>
		void MapContainer<T, U>::clear() {
			this->map.clear();
		}

		template<class T, class U>
		auto MapContainer<T, U>::begin() {
			return this->map.begin();
		}

		template<class T, class U>
		auto MapContainer<T, U>::end() {
			return this->map.end();
		}
	}
}
