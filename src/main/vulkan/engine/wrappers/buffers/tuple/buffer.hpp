#pragma once

#include "../buffer.h"

namespace ao {
	namespace vulkan {
		template<class... T>
		class TupleBuffer : public Buffer {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device">Device</param>
			TupleBuffer(std::weak_ptr<Device> device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~TupleBuffer() = default;

			/// <summary>
			/// Method to update entire buffer
			/// </summary>
			/// <param name="data">Data</param>
			/// <returns>This</returns>
			virtual TupleBuffer<T...>* update(T*... data) = 0;

			/// <summary>
			/// Method to update a fragment of buffer
			/// </summary>
			/// <param name="index">Index</param>
			/// <param name="data">Data</param>
			/// <returns>This</returns>
			virtual TupleBuffer<T...>* updateFragment(std::size_t index, void* data) = 0;

			virtual bool hasBuffer() override;

		protected:
			bool mHasBuffer;
		};

		/* IMPLEMENTATION */

		template<class ...T>
		TupleBuffer<T...>::TupleBuffer(std::weak_ptr<Device> device) : Buffer(device), mHasBuffer(false) {}

		template<class ...T>
		bool TupleBuffer<T...>::hasBuffer() {
			return this->mHasBuffer;
		}
	}
}
