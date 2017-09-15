#include "Core/Memory/Allocator.h"
#include "Core/Collections/List.h"

namespace Framework {

DefineRootAbstractClassInfo(Framework::Allocator);

Allocator::Allocator()
{ }

Allocator::~Allocator()
{ }

Allocator::Allocator(const Allocator &other)
{ }

Allocator&
Allocator::operator =(const Allocator &other)
{
    return (*this);
}

} // namespace Framework
