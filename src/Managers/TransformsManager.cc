#include "Managers/TransformsManager.h"
#include "Core/Collections/SimplePool.h"
#include "Core/Collections/Hash.h"
#include "Core/Pool/Handle.h"
#include "Components/Transform.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Memory/ScratchAllocator.h"
#include "Core/StringHash.h"
#include "Core/String.h"

namespace Framework {

DefineClassInfoWithFactory(Framework::TransformsManager, Framework::BaseManager);
DefineManager(Framework::TransformsManager, 0);

uint32_t
TransformsManager::RegisterTransform(Transform *pointer, const Math::Matrix &world, uint32_t parentId)
{
    if (pointer != nullptr)
        transforms.Add(pointer->GetHashCode(), pointer);

    uint32_t id = indices.Allocate();
    TransformEntry *entry = nullptr;

    if (kParentNull == parentId) {
        // place @ end
        indices.Set(id, entries.Count());
        entries.PushBack(TransformEntry());
        entry = &entries.Back();
    } else {
        // place after last parent descendant
        uint32_t entryIndex = indices.Get(parentId);
        entryIndex += (entries[entryIndex].descendantsCount + 1);

        indices.Set(id, entryIndex);
        entries.Insert(entryIndex, TransformEntry());
        entry = entries.Begin() + entryIndex;

        // update descendants
        uint32_t pId = parentId;
        while (pId != kParentNull) {
            TransformEntry *parentEntry = entries.Begin() + indices.Get(pId);
            ++parentEntry->descendantsCount;
            pId = parentEntry->parent;
        }

        // move other indices
        for (TransformEntry *movedEntry = entry + 1; movedEntry < entries.End(); ++movedEntry)
            indices.Set(movedEntry->id, movedEntry - entries.Begin());
    }

    Math::Matrix local;
    if (kParentNull == parentId) {
        local = world;
    } else {
        local = this->GetWorldMatrix(parentId);
        local.FastInvert();
        local = world.FastMultiply(local);
    }

    // setup entry
    entry->id = id;
    entry->parent = parentId;
    entry->descendantsCount = 0;
    entry->flags = kFlagsDirty | kFlagsHasChanged;
    entry->local[0] = Math::Vector4(local[ 0], local[ 4], local[ 8], local[12]);
    entry->local[1] = Math::Vector4(local[ 1], local[ 5], local[ 9], local[13]);
    entry->local[2] = Math::Vector4(local[ 2], local[ 6], local[10], local[14]);
    entry->world[0] = Math::Vector4(world[ 0], world[ 4], world[ 8], world[12]);
    entry->world[1] = Math::Vector4(world[ 1], world[ 5], world[ 9], world[13]);
    entry->world[2] = Math::Vector4(world[ 2], world[ 6], world[10], world[14]);

    if (pointer != nullptr)
        pointer->id = id;

    return id;
}

void
TransformsManager::OnTransformNameChanged(Transform *pointer, const StringHash &oldName)
{
    auto tr = transforms.Get(oldName.hash);
    while (tr != nullptr) {
        if (tr->EqualsTo(pointer))
            break;
        tr = transforms.Next(tr);
    }
    assert(tr != nullptr);
    transforms.Remove(tr);

    assert(pointer != nullptr);
    transforms.Add(pointer->GetHashCode(), pointer);
}

void
TransformsManager::UnregisterTransform(Transform *pointer, uint32_t id)
{
    uint32_t index    = indices.Get(id),
             parentId = entries[index].parent;

    TransformEntry *descendantsEnd = entries.Begin() + entries[index].descendantsCount + 1;

    entries.RemoveAt(index);
    indices.Free(id);

    // move other indices & change parent / assign dirty flag
    for (TransformEntry *movedEntry = entries.Begin() + index; movedEntry < entries.End(); ++movedEntry) {
        if (movedEntry < descendantsEnd) {
            movedEntry->flags |= kFlagsDirty;

            if (movedEntry->parent == id)
                movedEntry->parent = parentId;
        }

        indices.Set(movedEntry->id, movedEntry - entries.Begin());
    }

    // update descendants
    while (parentId != kParentNull) {
        TransformEntry *parentEntry = entries.Begin() + indices.Get(parentId);
        --parentEntry->descendantsCount;
        parentId = parentEntry->parent;
    }

    if (pointer != nullptr) {
        auto tr = transforms.Get(pointer->GetHashCode());
        while (tr != nullptr) {
            if (tr->EqualsTo(pointer))
                break;
            tr = transforms.Next(tr);
        }
        assert(tr != nullptr);
        transforms.Remove(tr);
    }
}

bool
TransformsManager::IsDirty(uint32_t transformId)
{
    return kFlagsDirty == (entries[indices.Get(transformId)].flags & kFlagsDirty);
}

bool
TransformsManager::HasChanged(uint32_t transformId)
{
    return kFlagsHasChanged == (entries[indices.Get(transformId)].flags & kFlagsHasChanged);
}

void
TransformsManager::SetParent(uint32_t transformId, uint32_t newParentId)
{
    uint32_t prevIndex = indices.Get(transformId),
             parentId  = entries[prevIndex].parent;

    // copy & remove descendants
    Array<TransformEntry> entriesToMove(Memory::GetAllocator<ScratchAllocator>(), entries[prevIndex].descendantsCount + 1);
    entriesToMove.InsertRange(0, &entries[prevIndex], entriesToMove.Capacity());
    entries.RemoveRange(prevIndex, entriesToMove.Count());

    // move other indices
    for (TransformEntry *movedEntry = entries.Begin() + prevIndex; movedEntry < entries.End(); ++movedEntry)
        indices.Set(movedEntry->id, movedEntry - entries.Begin());

    // update descendants
    while (parentId != kParentNull) {
        TransformEntry *parentEntry = entries.Begin() + indices.Get(parentId);
        parentEntry->descendantsCount -= entriesToMove.Count();
        parentId = parentEntry->parent;
    }

    entriesToMove.Front().parent = newParentId;

    uint32_t newIndex;
    TransformEntry *movedEntriesEnd;
    if (kParentNull == newParentId) {
        // place @ end
        newIndex = entries.Count();
        entries.InsertRange(newIndex, entriesToMove.Begin(), entriesToMove.Count());

        movedEntriesEnd = entries.End();
    } else {
        // place after last parent descendant
        newIndex = indices.Get(newParentId);
        newIndex += (entries[newIndex].descendantsCount + 1);
        entries.InsertRange(newIndex, entriesToMove.Begin(), entriesToMove.Count());

        movedEntriesEnd = entries.Begin() + newIndex + entriesToMove.Count();

        // update descendants
        while (newParentId != kParentNull) {
            TransformEntry *parentEntry = entries.Begin() + indices.Get(newParentId);
            parentEntry->descendantsCount += entriesToMove.Count();
            newParentId = parentEntry->parent;
        }
    }

    // move indices & assign dirty flag
    for (TransformEntry *movedEntry = entries.Begin() + newIndex; movedEntry < entries.End(); ++movedEntry) {
        if (movedEntry < movedEntriesEnd)
            movedEntry->flags |= kFlagsDirty;

        indices.Set(movedEntry->id, movedEntry - entries.Begin());
    }
}

void
TransformsManager::SetLocalMatrix(uint32_t transformId, const Math::Matrix &local)
{
    TransformEntry *entry = entries.Begin() + indices.Get(transformId);

    entry->local[0] = Math::Vector4(local[ 0], local[ 4], local[ 8], local[12]);
    entry->local[1] = Math::Vector4(local[ 1], local[ 5], local[ 9], local[13]);
    entry->local[2] = Math::Vector4(local[ 2], local[ 6], local[10], local[14]);
    entry->flags |= kFlagsDirty;

    TransformEntry *descendantsEnd = entry + entry->descendantsCount + 1;
    for (TransformEntry *descendant = entry + 1; descendant < descendantsEnd; ++descendant)
        descendant->flags |= kFlagsDirty;
}

void
TransformsManager::SetWorldMatrix(uint32_t transformId, const Math::Matrix &world)
{
    TransformEntry *entry = entries.Begin() + indices.Get(transformId);

    entry->world[0] = Math::Vector4(world[ 0], world[ 4], world[ 8], world[12]);
    entry->world[1] = Math::Vector4(world[ 1], world[ 5], world[ 9], world[13]);
    entry->world[2] = Math::Vector4(world[ 2], world[ 6], world[10], world[14]);

    if (kParentNull == entry->parent) {
        entry->local[0] = entry->world[0];
        entry->local[1] = entry->world[1];
        entry->local[2] = entry->world[2];
    } else {
        Math::Matrix local = this->GetWorldMatrix(entry->parent);
        local.FastInvert();
        local = world.FastMultiply(local);

        entry->local[0] = Math::Vector4(local[ 0], local[ 4], local[ 8], local[12]);
        entry->local[1] = Math::Vector4(local[ 1], local[ 5], local[ 9], local[13]);
        entry->local[2] = Math::Vector4(local[ 2], local[ 6], local[10], local[14]);
    }

    TransformEntry *descendantsEnd = entry + entry->descendantsCount + 1;
    for (TransformEntry *descendant = entry + 1; descendant < descendantsEnd; ++descendant)
        descendant->flags |= kFlagsDirty;
}

Math::Matrix
TransformsManager::GetLocalMatrix(uint32_t transformId)
{
    TransformEntry *entry = entries.Begin() + indices.Get(transformId);

    Math::Matrix local;
    local.GetColumn(0) = entry->local[0];
    local.GetColumn(1) = entry->local[1];
    local.GetColumn(2) = entry->local[2];
    local.GetColumn(3) = Math::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    local.Transpose();

    return local;
}

Math::Matrix
TransformsManager::GetWorldMatrix(uint32_t transformId)
{
    TransformEntry *entry = entries.Begin() + indices.Get(transformId);
    if (entry->flags & kFlagsDirty) {
        // find a parent that isn't dirty or root
        TransformEntry *parent = nullptr, *node = entry;
		while (node->parent != kParentNull) {
			parent = entries.Begin() + indices.Get(node->parent);
			if (0 == (parent->flags & kFlagsDirty))
                break;
			node = parent;
        }
        this->UpdateTransforms(node, false);
    }

    Math::Matrix world;
    world.GetColumn(0) = entry->world[0];
    world.GetColumn(1) = entry->world[1];
    world.GetColumn(2) = entry->world[2];
    world.GetColumn(3) = Math::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    world.Transpose();

    return world;
}

TransformsManager::TransformEntry*
TransformsManager::UpdateTransforms(TransformEntry *startTransform, bool clearHasChanged)
{
    uint32_t lastParentId = startTransform->id;
    Math::Matrix lastParentWorld, world, local;

    if (startTransform->flags & kFlagsDirty) {
        if (kParentNull == startTransform->parent) {
            startTransform->world[0] = startTransform->local[0];
            startTransform->world[1] = startTransform->local[1];
            startTransform->world[2] = startTransform->local[2];

            lastParentWorld.GetColumn(0) = startTransform->world[0];
            lastParentWorld.GetColumn(1) = startTransform->world[1];
            lastParentWorld.GetColumn(2) = startTransform->world[2];
            lastParentWorld.GetColumn(3) = Math::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
            lastParentWorld.Transpose();
        } else {
            TransformEntry *parentEntry = entries.Begin() + indices.Get(startTransform->parent);
            assert(0 == (parentEntry->flags & kFlagsDirty));

            world.GetColumn(0) = parentEntry->world[0];
            world.GetColumn(1) = parentEntry->world[1];
            world.GetColumn(2) = parentEntry->world[2];
            world.GetColumn(3) = Math::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
            world.Transpose();

            local.GetColumn(0) = startTransform->local[0];
            local.GetColumn(1) = startTransform->local[1];
            local.GetColumn(2) = startTransform->local[2];
            local.GetColumn(3) = Math::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
            local.Transpose();

            lastParentWorld = local.FastMultiply(world);

            startTransform->world[0] = Math::Vector4(lastParentWorld[ 0], lastParentWorld[ 4], lastParentWorld[ 8], lastParentWorld[12]);
            startTransform->world[1] = Math::Vector4(lastParentWorld[ 1], lastParentWorld[ 5], lastParentWorld[ 9], lastParentWorld[13]);
            startTransform->world[2] = Math::Vector4(lastParentWorld[ 2], lastParentWorld[ 6], lastParentWorld[10], lastParentWorld[14]);
        }

        startTransform->flags &= ~kFlagsDirty;
        startTransform->flags |= kFlagsHasChanged;
    } else {
        lastParentWorld.GetColumn(0) = startTransform->world[0];
        lastParentWorld.GetColumn(1) = startTransform->world[1];
        lastParentWorld.GetColumn(2) = startTransform->world[2];
        lastParentWorld.GetColumn(3) = Math::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        lastParentWorld.Transpose();

        if (clearHasChanged)
            startTransform->flags &= ~kFlagsHasChanged;
    }

    TransformEntry *entry = startTransform + 1, *end = startTransform + startTransform->descendantsCount + 1;
    while (entry < end) {
        if (0 == (entry->flags & kFlagsDirty))
        {
            if (clearHasChanged)
                entry->flags &= ~kFlagsHasChanged;
            ++entry;
            continue;
        }

        if (entry->parent != lastParentId) {
            TransformEntry *parent = entries.Begin() + indices.Get(entry->parent);
            lastParentWorld.GetColumn(0) = parent->world[0];
            lastParentWorld.GetColumn(1) = parent->world[1];
            lastParentWorld.GetColumn(2) = parent->world[2];
            lastParentWorld.GetColumn(3) = Math::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
            lastParentWorld.Transpose();
            lastParentId = entry->parent;
        }

        local.GetColumn(0) = entry->local[0];
        local.GetColumn(1) = entry->local[1];
        local.GetColumn(2) = entry->local[2];
        local.GetColumn(3) = Math::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        local.Transpose();

        world = local.FastMultiply(lastParentWorld);

        entry->world[0] = Math::Vector4(world[ 0], world[ 4], world[ 8], world[12]);
        entry->world[1] = Math::Vector4(world[ 1], world[ 5], world[ 9], world[13]);
        entry->world[2] = Math::Vector4(world[ 2], world[ 6], world[10], world[14]);

        entry->flags &= ~kFlagsDirty;
        entry->flags |= kFlagsHasChanged;

        ++entry;
    }

    return entry;
}

TransformsManager::TransformsManager()
: entries(Memory::GetAllocator<MallocAllocator>()),
  indices(Memory::GetAllocator<MallocAllocator>()),
  transforms(Memory::GetAllocator<MallocAllocator>())
{ }

TransformsManager::~TransformsManager()
{ }

Handle<Transform>
TransformsManager::Find(const String &path) const
{
    Array<String> names = path.Split("/");

    StringHash lastName = names.Back();
    names.PopBack();

    auto tr = transforms.Get(lastName.hash);
    while (tr != nullptr) {
        if ((*tr)->name == lastName) {
            bool valid = true;
            auto ptr = *tr;
            int32_t index = names.Count() - 1;
            while (index >= 0) {
                StringHash wantedParent = names[index];
                if (ptr->parent->name != wantedParent) {
                    valid = false;
                    break;
                } else {
                    --index;
                    ptr = ptr->parent;
                }
            }
            if (valid)
                return *tr;
        }

        tr = transforms.Next(tr);
    }

    return nullptr;
}

const Handle<Transform>*
TransformsManager::Begin() const
{
    return transforms.Begin();
}

Handle<Transform>*
TransformsManager::Begin()
{
    return transforms.Begin();
}

const Handle<Transform>*
TransformsManager::End() const
{
    return transforms.End();
}

Handle<Transform>*
TransformsManager::End()
{
    return transforms.End();
}

void
TransformsManager::OnUpdate()
{ }

void
TransformsManager::OnLateUpdate()
{ }

void
TransformsManager::OnRender()
{
    TransformEntry *entry = entries.Begin(), *end = entries.End();
    while (entry < end)
        entry = this->UpdateTransforms(entry, true);
}

void
TransformsManager::OnPause()
{ }

void
TransformsManager::OnResume()
{ }

void
TransformsManager::OnQuit()
{ }

} // namespace Managers
