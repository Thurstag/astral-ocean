// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <memory>

#include "../buffer.h"

namespace ao::vulkan {
	template<class T>
	class DynamicArrayBuffer : public Buffer {
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="_count">Count</param>
		/// <param name="device">Device</param>
		explicit DynamicArrayBuffer(size_t const _count, std::weak_ptr<Device> device);

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~DynamicArrayBuffer() = default;

		/// <summary>
		/// Method to update entire buffer
		/// </summary>
		/// <param name="data">Data</param>
		/// <returns>This</returns>
		virtual DynamicArrayBuffer<T>* update(std::vector<T> const& data) = 0;

		/// <summary>
		/// Method to update a fragment of buffer
		/// </summary>
		/// <param name="index">Index</param>
		/// <param name="data">Data</param>
		/// <returns>This</returns>
		virtual DynamicArrayBuffer<T>* updateFragment(std::size_t const index, T const* data) = 0;

		/// <summary>
		/// Method to get capacity
		/// </summary>
		/// <returns>Capacity</returns>
		size_t capacity() const;

		virtual bool hasBuffer() const override;
	protected:
		size_t count;
		bool mHasBuffer;
	};

	/* IMPLEMENTATION */

	template<class T>
	DynamicArrayBuffer<T>::DynamicArrayBuffer(size_t _count, std::weak_ptr<Device> device) : Buffer(device), count(_count), mHasBuffer(false) {}

	template<class T>
	size_t DynamicArrayBuffer<T>::capacity() const {
		return this->count;
	}

	template<class T>
	bool DynamicArrayBuffer<T>::hasBuffer() const {
		return this->mHasBuffer;
	}

	template<class T, size_t N>
	class ArrayBuffer : public Buffer {
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="device">Device</param>
		ArrayBuffer(std::weak_ptr<Device> device);

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~ArrayBuffer() = default;

		/// <summary>
		/// Method to update entire buffer
		/// </summary>
		/// <param name="data">Data</param>
		/// <returns>This</returns>
		virtual ArrayBuffer<T, N>* update(std::array<T, N> const& data) = 0;

		/// <summary>
		/// Method to update a fragment of buffer
		/// </summary>
		/// <param name="index">Index</param>
		/// <param name="data">Data</param>
		/// <returns>This</returns>
		virtual ArrayBuffer<T, N>* updateFragment(std::size_t const index, T const* data) = 0;

		/// <summary>
		/// Method to get offset of fragment at index
		/// </summary>
		/// <param name="index">Fragment index</param>
		/// <returns>Offset</returns>
		virtual vk::DeviceSize offset(size_t const index) const = 0;

		virtual bool hasBuffer() const override;

	protected:
		bool mHasBuffer;
	};

	/* IMPLEMENTATION */

	template<class T, size_t N>
	ArrayBuffer<T, N>::ArrayBuffer(std::weak_ptr<Device> device) : Buffer(device), mHasBuffer(false) {}

	template<class T, size_t N>
	bool ArrayBuffer<T, N>::hasBuffer() const {
		return mHasBuffer;
	}
}
