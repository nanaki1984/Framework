#pragma once

#include <atomic>
#include "Core/ClassInfo.h"
#include "Core/Collections/List_type.h"

namespace Framework {

class Allocator;
class GarbageCollector;

class RefCounted {
    DeclareRootClassInfo;
public:
	static GarbageCollector GC;
private:
	ListNode<RefCounted> node;
	
    std::atomic_uint32_t refCount;
public:
	Allocator *allocator;

    RefCounted();

    void AddRef();
    void Release();

    uint32_t GetRefCount() const;

    bool IsAlive() const;
protected:
    virtual ~RefCounted();

	virtual void InvalidateWeakReferences();

	friend class GarbageCollector;
};

class GarbageCollector {
private:
	List<RefCounted, &RefCounted::node> garbage;
public:
	GarbageCollector();
	~GarbageCollector();

	void Collect();

	friend class RefCounted;
};

}; // namespace Framework
