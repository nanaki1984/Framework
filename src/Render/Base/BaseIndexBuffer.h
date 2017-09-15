#pragma once

#include "Render/Base/HardwareBuffer.h"

namespace Framework {
    namespace RHI {

enum IndexSize {
    IndexWord  = 2,
    IndexDword = 4
};

struct IndexBufferDesc {
    HardwareBuffer::Flags flags;
    IndexSize             indexSize;
    uint32_t              nIndices;
};

class BaseIndexBuffer : public HardwareBuffer {
    DeclareClassInfo;
protected:
    IndexSize indexSize;
public:
    BaseIndexBuffer(const IndexBufferDesc &desc);
    virtual ~BaseIndexBuffer();

    IndexSize GetIndexSize() const;
};

    } // namespace RHI
} // namespace Framework
