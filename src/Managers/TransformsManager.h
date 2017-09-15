#pragma once

#include "Managers/BaseManager.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Core/Collections/SimplePool_type.h"
#include "Core/Collections/Hash_type.h"
#include "Core/Pool/Handle_type.h"

namespace Framework {

class Transform;
class String;

class TransformsManager : public BaseManager {
    DeclareClassInfo;
    DeclareManager;
public:
    static const uint32_t kParentNull = 0xffffffff;
private:
    static const uint32_t kFlagsDirty      = 1 << 0;
    static const uint32_t kFlagsHasChanged = 1 << 1;

    struct TransformEntry {
        uint32_t id;
        uint32_t parent;
        uint32_t descendantsCount;
        uint32_t flags;
        // ToDo: look if storing pos, quat & scaling is faster
        Math::Vector4 local[3];
        Math::Vector4 world[3];
        //Skeleton* skel;
    };

    Array<TransformEntry> entries;
    SimplePool<uint32_t> indices;

    Hash<Handle<Transform>> transforms;

    TransformEntry* UpdateTransforms(TransformEntry *startTransform, bool clearHasChanged);
public:
    TransformsManager();
    virtual ~TransformsManager();

    uint32_t RegisterTransform(Transform *pointer, const Math::Matrix &world, uint32_t parentId = kParentNull);
    void OnTransformNameChanged(Transform *pointer, const StringHash &oldName);
    void UnregisterTransform(Transform *pointer, uint32_t id);

    bool IsDirty(uint32_t transformId);
    bool HasChanged(uint32_t transformId);

    void SetParent(uint32_t transformId, uint32_t newParentId);
    void SetLocalMatrix(uint32_t transformId, const Math::Matrix &local);
    void SetWorldMatrix(uint32_t transformId, const Math::Matrix &world);

    Math::Matrix GetLocalMatrix(uint32_t transformId);
    Math::Matrix GetWorldMatrix(uint32_t transformId);

    Handle<Transform> Find(const String &path) const;

    const Handle<Transform>* Begin() const;
    Handle<Transform>* Begin();

    const Handle<Transform>* End() const;
    Handle<Transform>* End();

    virtual void OnUpdate();
    virtual void OnLateUpdate();
	virtual void OnRender();
    virtual void OnPause();
    virtual void OnResume();
    virtual void OnQuit();
};

} // namespace Framework
