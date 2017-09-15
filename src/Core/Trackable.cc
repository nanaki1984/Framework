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
    std::lock_guard<std::recursive_mutex> guard(weakRefLock);

    int cnt = weakRefCount++;
    if (nullptr == tail) {
        assert(0 == cnt);
        head = tail = ptr;

        ptr->prevRef = nullptr;
        ptr->nextRef = nullptr;
    } else {
        tail->nextRef = ptr;

        ptr->prevRef = tail;
        ptr->nextRef = nullptr;

        tail = ptr;
    }
}

void
Trackable::RemoveWeakRef(WeakPtr<Trackable> *ptr)
{
    std::lock_guard<std::recursive_mutex> guard(weakRefLock);

    int cnt = weakRefCount--;
    assert(cnt > 0);

    if (head == ptr)
    {
        if (tail == ptr)
        {
            assert(1 == cnt);
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
}

void
Trackable::InvalidateAllWeakRefs()
{
    std::lock_guard<std::recursive_mutex> guard(weakRefLock);

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
