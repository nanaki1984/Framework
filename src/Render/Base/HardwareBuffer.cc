#include "Render/Base/HardwareBuffer.h"
#include "Core/Memory/Memory.h"

namespace Framework {
    namespace RHI {

DefineClassInfo(Framework::RHI::HardwareBuffer, Framework::RefCounted);

HardwareBuffer::HardwareBuffer(Flags _flags, uint32_t _size)
: flags(_flags),
  size(_size)
{ }

HardwareBuffer::~HardwareBuffer()
{ }

HardwareBuffer::Flags
HardwareBuffer::GetFlags() const
{
    return flags;
}

uint32_t
HardwareBuffer::GetSize() const
{
    return size;
}

void
HardwareBuffer::Upload(const LockInfo &lockInfo, const void *data)
{
    assert(false);
}

    } // namespace RHI
} // namespace Framework
