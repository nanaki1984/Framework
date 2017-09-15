#pragma once

#include "Managers/BaseManager.h"
#include "Core/Collections/Array_type.h"
#include "Core/Collections/Hash_type.h"
#include "Core/SmartPtr.h"
#include "Game/BaseComponentsList.h"
#include "Game/BaseMsgDispatcher.h"

namespace Framework {

class ComponentsManager : public BaseManager {
    DeclareClassInfo;
    DeclareManager;
private:
    Array<SmartPtr<BaseComponentsList>> lists;
    Hash<SmartPtr<BaseComponentsList>> listsHash;
    Array<SmartPtr<BaseMsgDispatcher>> dispatchers;
public:
    ComponentsManager();
    virtual ~ComponentsManager();

    void AddComponentsList(const SmartPtr<BaseComponentsList> &p);
    void AddMsgDispatcher(const SmartPtr<BaseMsgDispatcher> &p);

    const SmartPtr<BaseComponentsList>& GetComponentsList(const StringHash &className);

    void DeserializeComponents(SerializationServer *server, const BitStream &stream);

    virtual void OnUpdate();
    virtual void OnLateUpdate();
	virtual void OnRender();
    virtual void OnPause();
    virtual void OnResume();
    virtual void OnQuit();

//#if defined(EDITOR)
    void SerializeMarkedComponents(BitStream &stream);
//#endif
};

} // namespace Framework
