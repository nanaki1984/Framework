#pragma once

#include "Render/Base/HardwareBuffer.h"

namespace Framework {
    namespace RHI {

enum ComputeBufferType
{
	CBType_Default = 0,
	CBType_GPUOnly
};

struct ComputeBufferDesc {
	uint32_t stride;
	uint32_t count;

	ComputeBufferType type;
};

class BaseComputeBuffer : public HardwareBuffer {
    DeclareClassInfo;
protected:
	uint32_t stride;
	uint32_t count;

	ComputeBufferType type;
public:
	BaseComputeBuffer(const ComputeBufferDesc &desc);
    virtual ~BaseComputeBuffer();

	uint32_t GetStride();
	uint32_t GetCount();
	ComputeBufferType GetType();

	void Download(const LockInfo &lockInfo, void *data);

	void Dispatch(uint32_t x, uint32_t y, uint32_t z);
};

inline uint32_t
BaseComputeBuffer::GetStride()
{
	return stride;
}

inline uint32_t
BaseComputeBuffer::GetCount()
{
	return count;
}

inline ComputeBufferType
BaseComputeBuffer::GetType()
{
	return type;
}

    } // namespace RHI
} // namespace Framework
