#include "Core/Trackable.h"
#include "Core/WeakPtr.h"

namespace Framework {

DefineClassInfo(Framework::Trackable, Framework::RefCounted)

Trackable::Trackable()
: weakRefCount(0),
  head(nullptr),
  tail(nullptr)
{ }

Trackable::~Trackable ( )
{
    this->InvalidateAllWeakRefs();
}

void
Trackable::AddWeakRef(WeakPtr<Trackable> *ptr)
{
    if (nullptr == tail) {
        head = tail = ptr;

        ptr->prevRef = nullptr;
        ptr->nextRef = nullptr;
    } else {
        tail->nextRef = ptr;

        ptr->prevRef = tail;
        ptr->nextRef = nullptr;

        tail = ptr;
    }

    ++weakRefCount;
}

void
Trackable::RemoveWeakRef(WeakPtr<Trackable> *ptr)
{
    if (head == ptr)
    {
        if (tail == ptr)
        {
            assert(1 == weakRefCount);
            head = tail = nullptr;
        }
        else
        {
            head = ptr->nextRef;
            assert(head != nullptr);
            head->prevRef = nullptr;
        }
    }
    else if (tail == ptr)
    {
        tail = ptr->prevRef;
        assert(tail != nullptr);
        tail->nextRef = nullptr;
    }
    else
    {
        assert(ptr->prevRef != nullptr && ptr->nextRef != nullptr);
        ptr->prevRef->nextRef = ptr->nextRef;
        ptr->nextRef->prevRef = ptr->prevRef;
    }

    ptr->prevRef = nullptr;
    ptr->nextRef = nullptr;

    assert(weakRefCount > 0);
    --weakRefCount;
}

void
Trackable::InvalidateAllWeakRefs()
{
    WeakPtr<Trackable> *node = head;
    WeakPtr<Trackable> *tmp;

    while (node) {
        tmp  = node;
        node = node->nextRef;

        tmp->Invalidate();
    }

    head = tail = nullptr;

    weakRefCount = 0;
}

} // namespace Framework
