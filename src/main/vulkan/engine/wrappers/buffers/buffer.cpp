#include "buffer.h"

ao::vulkan::Buffer::Buffer(std::weak_ptr<Device> _device) : device(_device), mHasBuffer(false) {}
