#include "Render/Base/BaseComputeBuffer.h"

namespace Framework {
    namespace RHI {

DefineClassInfo(Framework::RHI::BaseComputeBuffer, Framework::RHI::HardwareBuffer);

BaseComputeBuffer::BaseComputeBuffer(const ComputeBufferDesc &desc)
: HardwareBuffer(HardwareBuffer::ComputeBuffer, desc.stride * desc.count),
  stride(desc.stride),
  count(desc.count),
  type(desc.type)
{ }

BaseComputeBuffer::~BaseComputeBuffer()
{ }

void
BaseComputeBuffer::Download(const LockInfo &lockInfo, void *data)
{ }

    } // namespace RHI
} // namespace Framework
