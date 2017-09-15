#include "Core/Pool/BaseObject.h"
#include "Core/Pool/BasePool.h"

namespace Framework {

DefineRootClassInfo(Framework::BaseObject);

BaseObject::BaseObject()
{ }

BaseObject::BaseObject(const BaseObject &other)
{ }

BaseObject::BaseObject(BaseObject &&other)
: pool(other.pool),
  id(other.id)
{ }

BaseObject::~BaseObject()
{ }

BasePool*
BaseObject::GetPool() const
{
    return pool;
}

uint32_t
BaseObject::GetInstanceID() const
{
    return id;
}

void
BaseObject::Destroy()
{
    assert(pool);
    pool->Free(this);
}

} // namespace Framework
