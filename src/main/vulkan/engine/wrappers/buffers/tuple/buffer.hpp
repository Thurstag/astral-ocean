// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "../buffer.h"

namespace ao::vulkan {
	template<class... T>
	class TupleBuffer : public Buffer {
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="device">Device</param>
		explicit TupleBuffer(std::weak_ptr<Device> device);

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~TupleBuffer() = default;

		/// <summary>
		/// Method to update entire buffer
		/// </summary>
		/// <param name="data">Data</param>
		/// <returns>This</returns>
		virtual TupleBuffer<T...>* update(T const*... data) = 0;

		/// <summary>
		/// Method to update a fragment of buffer
		/// </summary>
		/// <param name="index">Index</param>
		/// <param name="data">Data</param>
		/// <returns>This</returns>
		virtual TupleBuffer<T...>* updateFragment(std::size_t const index, void* const data) = 0;

		virtual bool hasBuffer() const override;

	protected:
		bool mHasBuffer;
	};

	/* IMPLEMENTATION */

	template<class ...T>
	TupleBuffer<T...>::TupleBuffer(std::weak_ptr<Device> device) : Buffer(device), mHasBuffer(false) {}

	template<class ...T>
	bool TupleBuffer<T...>::hasBuffer() const {
		return this->mHasBuffer;
	}
}
