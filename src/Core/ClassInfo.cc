#include "Core/ClassInfo.h"
#include "Core/StringHash.h"
#include "Core/RefCounted.h"
#include "Core/SmartPtr.h"
#include "Core/Collections/List.h"
#include "Core/Collections/Hash.h"
#include "Core/Memory/MallocAllocator.h"

namespace Framework {

ClassInfo::ClassInfo(const StringHash &_name, const ClassInfo *_parent, const ClassInfo *_templ, size_t _size, size_t _align, Ctor _ctor)
: name(_name.string),
  hash(_name.hash),
  fcc(kInvalidFCC),
  parent(_parent),
  templ(_templ),
  size(_size),
  align(_align),
  ctor(_ctor),
  staticCtor(nullptr)
{
    assert(name[0]);
    assert(parent != this);
    ClassInfoUtils::Instance()->classes.PushBack(this);
}

ClassInfo::ClassInfo(const StringHash &_name, uint32_t _fcc, const ClassInfo *_parent, const ClassInfo *_templ, size_t _size, size_t _align, Ctor _ctor)
: ClassInfo(_name, _parent, _templ, _size, _align, _ctor)
{
    fcc = _fcc;
}

ClassInfo::~ClassInfo()
{ }

int
ClassInfo::SetStaticCtor(StaticCtor _staticCtor)
{
    assert(nullptr == staticCtor);
    staticCtor = _staticCtor;
    return 0;
}

const char*
ClassInfo::GetName() const
{
    return name;
}

uint32_t
ClassInfo::GetHashCode() const
{
    return hash;
}

bool
ClassInfo::HasFCC() const
{
    return fcc != kInvalidFCC;
}

uint32_t
ClassInfo::GetFCC() const
{
    assert(fcc != kInvalidFCC);
    return fcc;
}

const ClassInfo*
ClassInfo::GetParent() const
{
    return parent;
}

const ClassInfo*
ClassInfo::GetTemplate() const
{
    return templ;
}

size_t
ClassInfo::GetSize() const
{
    return size;
}

size_t
ClassInfo::GetAlign() const
{
    return align;
}

bool
ClassInfo::IsDerivedFrom(const ClassInfo *classInfo) const
{
    if (this == classInfo)
        return true;

    ClassInfo *node = const_cast<ClassInfo*>(parent);

    while (node) {
        if (node == classInfo)
            return true;
        else
            node = const_cast<ClassInfo*>(node->GetParent());
    }

    return false;
}

SmartPtr<RefCounted>
ClassInfo::Create(Allocator *allocator) const
{
    assert(ctor != nullptr);
    assert(this->IsDerivedFrom(&RefCounted::RTTI));

    RefCounted *instance = static_cast<RefCounted*>(ctor(allocator->Allocate(size, align)));
    instance->allocator = allocator;

    SmartPtr<RefCounted> p;
    p.ptr = instance;
    p.ptr->AddRef();

    return p;
}

ClassInfoUtils::ClassInfoUtils()
: classesHash(Memory::GetAllocator<MallocAllocator>()),
  fccHash(Memory::GetAllocator<MallocAllocator>())// FAKE: GetAllocator returns an invalid reference
{ }

ClassInfoUtils::~ClassInfoUtils()
{
    classes.Clear();
}

void
ClassInfoUtils::Initialize()
{
    classesHash = Hash<ClassInfo*>(Memory::GetAllocator<MallocAllocator>());
    fccHash = Hash<ClassInfo*>(Memory::GetAllocator<MallocAllocator>()); // REAL

    auto node = classes.Begin();
    while (node != nullptr)
    {
        if (node->staticCtor != nullptr)
            node->staticCtor();

        classesHash.Add(node->GetHashCode(), node);
        if (node->HasFCC())
        {
            assert(nullptr == fccHash.Get(node->GetFCC()));
            fccHash.Add(node->GetFCC(), node);
        }

        node = classes.GetNext(node);
    }
}

const ClassInfo*
ClassInfoUtils::FindClass(const StringHash &className) const
{
    auto classInfo = classesHash.Get(className.hash);
    while (classInfo != nullptr)
    {
        if (0 == strcmp((*classInfo)->GetName(), className.string))
            break;
        classInfo = classesHash.Next(classInfo);
    }
    if (nullptr == classInfo)
        return nullptr;
    else
        return *classInfo;
}

const ClassInfo*
ClassInfoUtils::FindClassFCC(uint32_t fcc) const
{
    auto classInfo = fccHash.Get(fcc);
    if (nullptr == classInfo)
        return nullptr;
    else
        return *classInfo;
}

char __utilsBuffer[sizeof(ClassInfoUtils)];

ClassInfoUtils* ClassInfoUtils::instance = nullptr;

ClassInfoUtils*
ClassInfoUtils::Instance()
{
    if (nullptr == instance)
        instance = new(__utilsBuffer) ClassInfoUtils();
    return instance;
}

void
ClassInfoUtils::Destroy()
{
    if (nullptr == instance)
        return;

    instance->~ClassInfoUtils();
    instance = nullptr;
}

} // namespace Framework
