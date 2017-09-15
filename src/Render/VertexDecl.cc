#include "Render/VertexDecl.h"
#include "Core/Collections/Array.h"
#include "Core/Memory/MallocAllocator.h"

namespace Framework {
	namespace RHI {

VertexDecl::VertexDecl()
: elements(Memory::GetAllocator<MallocAllocator>())
{ }

VertexDecl::VertexDecl(const VertexDecl &other)
: elements(other.elements)
{ }

VertexDecl::VertexDecl(VertexDecl &&other)
: elements(std::forward<Array<VertexElement>>(other.elements))
{ }

VertexDecl::~VertexDecl ( )
{ }

VertexDecl&
VertexDecl::operator =(const VertexDecl &other)
{
    elements = other.elements;
    return (*this);
}

VertexDecl&
VertexDecl::operator =(VertexDecl &&other)
{
	elements = std::forward<Array<VertexElement>>(other.elements);
    return (*this);
}

uint32_t
VertexDecl::AddVertexElement(VertexType type, VertexSize size)
{
    uint32_t offset = 0;
    if (elements.Count() > 0)
        offset = elements.Back().offset + elements.Back().size;

    elements.PushBack({ type, size, offset });

    return offset;
}

uint32_t
VertexDecl::GetOffset(VertexType type, VertexSize &size, uint32_t wantedSize, uint32_t n) const
{
    uint32_t k = 0;
    for (auto it = elements.Begin(), end = elements.End(); it != end; ++it) {
        if (it->type == type) {
            if (0 != wantedSize && it->size == wantedSize)
                ++k;
            else if (0 == wantedSize)
                ++k;

            if (k == n) {
                size = it->size;
                return it->offset;
            }
        }
    }
    assert(false);
    return 0;
}

const VertexDecl::VertexElement&
VertexDecl::GetElement(uint32_t index) const
{
    return elements[index];
}

uint32_t
VertexDecl::GetElementsCount() const
{
    return elements.Count();
}

uint32_t
VertexDecl::GetVertexStride() const
{
    if (elements.IsEmpty())
        return 0;
    return elements.Back().offset + elements.Back().size;
}

	} // namespace RHI
} // namespace Framework
