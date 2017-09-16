#include "Core/Trackable.h"
#include "Core/WeakPtr.h"

namespace Framework {

DefineClassInfo(Framework::Trackable, Framework::RefCounted)

Trackable::Trackable()
: weakRefCount(0),
  head(nullptr),
  tail(nullptr)
{ }

Trackable::~Trackable()
{ }

void
Trackable::InvalidateWeakReferences()
{
	weakRefLock.lock();
	WeakPtr<Trackable> *node = head;
	WeakPtr<Trackable> *tmp;

	while (node) {
		tmp = node;
		node = node->nextRef;

		tmp->Invalidate();
	}

	head = tail = nullptr;
	weakRefLock.unlock();

	assert(0 == weakRefCount);
}

void
Trackable::AddWeakRef(WeakPtr<Trackable> *ptr)
{
    int cnt = weakRefCount++;

	weakRefLock.lock();
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
	weakRefLock.unlock();
}

void
Trackable::RemoveWeakRef(WeakPtr<Trackable> *ptr)
{
    int cnt = weakRefCount--;
    assert(cnt > 0);

	weakRefLock.lock();
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
	weakRefLock.unlock();

    ptr->prevRef = nullptr;
    ptr->nextRef = nullptr;
}

} // namespace Framework
