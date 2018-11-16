#pragma once

#include <memory>

#include <ao/core/exception/exception.h>

namespace ao {
	namespace core {
		template<class T>
		class dynamic_ptr {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			dynamic_ptr() = default;

			/// <summary>
			/// Method to make a dynamic pointer
			/// </summary>
			template<class _T, class... Args>
			friend dynamic_ptr<_T> make_dynamic(Args... args);

			/// <summary>
			/// Method to update a dynamic pointer
			/// </summary>						 
			/// <param name="pointer">Pointer</param>
			template<class _T, class... Args>
			friend dynamic_ptr<_T>& update(dynamic_ptr<_T>& pointer, Args... args);

			/// <summary>
			/// Operator ->
			/// </summary>
			/// <returns>Object</returns>
			T* operator->();

			/// <summary>
			/// Operator *
			/// </summary>
			/// <returns>Object</returns>
			T& operator*();

			/// <summary>
			/// Method to get use count
			/// </summary>
			/// <returns>Use count</returns>
			long use_count();
		protected:
			std::shared_ptr<T*> ptr;
			static std::allocator<T> Allocator;

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="_ptr">Pointer</param>
			dynamic_ptr(std::shared_ptr<T*> _ptr);
		};

		template<class T>
		std::allocator<T> dynamic_ptr<T>::Allocator;

		/* IMPLEMENTATION */

		template<class T>
		T * dynamic_ptr<T>::operator->() {
			if (this->ptr.get() == nullptr) {
				throw Exception("Pointer isn't initialized");
			}
			return *this->ptr.get();
		}

		template<class T>
		T & dynamic_ptr<T>::operator*() {
			return *this->operator*();
		}

		template<class T>
		long dynamic_ptr<T>::use_count() {
			return this->ptr.use_count();
		}

		template<class T>
		dynamic_ptr<T>::dynamic_ptr(std::shared_ptr<T*> _ptr) : ptr(_ptr) {}

		template<class _T, class... Args>
		dynamic_ptr<_T> make_dynamic(Args... args) {
			// Create object
			_T* object = dynamic_ptr<_T>::Allocator.allocate(1);
			dynamic_ptr<_T>::Allocator.construct(object, args...);

			// Create dynamic ptr
			return dynamic_ptr<_T>(std::shared_ptr<_T*>(new _T*(object), [](_T** pointer) {
				// Delete in allocator
				dynamic_ptr<_T>::Allocator.destroy(*pointer);
				dynamic_ptr<_T>::Allocator.deallocate(*pointer, 1);

				// Delete pointer
				delete pointer;
			}));
		}
		template<class _T, class ...Args>
		dynamic_ptr<_T>& update(dynamic_ptr<_T>& pointer, Args ...args) {
			if (pointer.ptr.get() == nullptr) {
				return pointer = make_dynamic<_T>(args...);
			}

			// Create object
			_T* object = dynamic_ptr<_T>::Allocator.allocate(1);
			dynamic_ptr<_T>::Allocator.construct(object, args...);

			// Delete old value
			dynamic_ptr<_T>::Allocator.destroy(*pointer.ptr);
			dynamic_ptr<_T>::Allocator.deallocate(*pointer.ptr, 1);

			// Change pointer's value
			*pointer.ptr = object;

			return pointer;
		}
	}
}

