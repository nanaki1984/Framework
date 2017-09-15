#include "Render/Base/BaseIndexBuffer.h"

namespace Framework {
    namespace RHI {

DefineClassInfo(Framework::RHI::BaseIndexBuffer, Framework::RHI::HardwareBuffer);

BaseIndexBuffer::BaseIndexBuffer(const IndexBufferDesc &desc)
: HardwareBuffer(desc.flags, desc.indexSize * desc.nIndices), 
  indexSize(desc.indexSize)
{ }

BaseIndexBuffer::~BaseIndexBuffer()
{ }

IndexSize
BaseIndexBuffer::GetIndexSize() const
{
    return indexSize;
}

    } // namespace RHI
} // namespace Framework
