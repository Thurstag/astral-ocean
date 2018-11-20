#pragma once

#include <fmt/format.h>

#include "buffer.h"

namespace ao {
	namespace vulkan {
		template<class... T>
		class TupleBuffer : public Buffer {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			TupleBuffer(std::weak_ptr<Device> _device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~TupleBuffer() = default;

			/// <summary>
			/// Method to update entire buffer
			/// </summary>
			/// <param name="data">Data</param>
			/// <returns></returns>
			virtual TupleBuffer<T...>* update(T*... data) = 0;

			/// <summary>
			/// Method to update a fragment of buffer
			/// </summary>
			/// <param name="index">Index</param>
			/// <param name="data">Data</param>
			/// <returns>This</returns>
			virtual TupleBuffer<T...>* updateFragment(std::size_t index, void* data) = 0;

			/// <summary>
			/// Method to get offset of fragment at index
			/// </summary>
			/// <param name="index">Fragment index</param>
			/// <returns>Offset</returns>
			virtual vk::DeviceSize offset(size_t index) = 0;

			virtual bool hasBuffer() override;

		protected:
			bool mHasBuffer;
		};

		/* IMPLEMENTATION */

		template<class ...T>
		TupleBuffer<T...>::TupleBuffer(std::weak_ptr<Device> _device) : Buffer(_device), mHasBuffer(false) {}

		template<class ...T>
		bool TupleBuffer<T...>::hasBuffer() {
			return this->mHasBuffer;
		}
	}
}
