#include "Managers/ComponentsManager.h"
#include "Core/Collections/Array.h"
#include "Core/Collections/Hash.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/IO/BitStream.h"
#include "Game/SerializationServer.h"

namespace Framework {

DefineClassInfoWithFactory(Framework::ComponentsManager, Framework::BaseManager);
DefineManager(Framework::ComponentsManager, -100);

ComponentsManager::ComponentsManager()
: lists(Memory::GetAllocator<MallocAllocator>()),
  listsHash(Memory::GetAllocator<MallocAllocator>()),
  dispatchers(Memory::GetAllocator<MallocAllocator>())
{ }

ComponentsManager::~ComponentsManager()
{ }

void
ComponentsManager::AddComponentsList(const SmartPtr<BaseComponentsList> &p)
{
    int index = BinarySearch<SmartPtr<BaseComponentsList>, BaseComponentsList>(lists, 0, lists.Count(), *p,
    [] (const SmartPtr<BaseComponentsList>& a,
        const BaseComponentsList& b)
    {
        return (*a).Compare(b);
    });
    if (index < 0)
        index = ~index;
    lists.Insert(index, p);

    listsHash.Add(p->GetComponentClassInfo()->GetHashCode(), p);
}

void
ComponentsManager::AddMsgDispatcher(const SmartPtr<BaseMsgDispatcher> &p)
{
    dispatchers.PushBack(p);
}

const SmartPtr<BaseComponentsList>&
ComponentsManager::GetComponentsList(const StringHash &className)
{
    auto list = listsHash.Get(className.hash);
    while (list != nullptr)
    {
        if (0 == strcmp((*list)->GetComponentClassInfo()->GetName(), className.string))
            break;
        list = listsHash.Next(list);
    }
    assert(list != nullptr);
    return *list;
}

void
ComponentsManager::DeserializeComponents(SerializationServer *server, const BitStream &stream)
{
    while (!stream.EndOfStream() && stream.RemainingBytes() >= 4) {
        uint32_t serializationid;
        stream >> serializationid;

        // ToDo: component data size checks
        // ToDo: optimize, components in stream are ordered by type

        Component *component = server->GetLoadingObject(serializationid).Cast<Component>();
        for (auto it = lists.Begin(), end = lists.End(); it != end; ++it) {
            if ((*it)->TryDeserializeComponent(component, server, stream))
                break;
        }
    }
}

void
ComponentsManager::OnUpdate()
{
    for (auto it = lists.Begin(), end = lists.End(); it != end; ++it)
        (*it)->OnUpdate();
}

void
ComponentsManager::OnLateUpdate()
{
    for (auto it = lists.Begin(), end = lists.End(); it != end; ++it)
        (*it)->OnLateUpdate();
}

void
ComponentsManager::OnRender()
{
	for (auto it = lists.Begin(), end = lists.End(); it != end; ++it)
		(*it)->OnRender();
}

void
ComponentsManager::OnPause()
{
    for (auto it = lists.Begin(), end = lists.End(); it != end; ++it)
        (*it)->OnPause();
}

void
ComponentsManager::OnResume()
{
    for (auto it = lists.Begin(), end = lists.End(); it != end; ++it)
        (*it)->OnResume();
}

void
ComponentsManager::OnQuit()
{
    for (auto it = lists.Begin(), end = lists.End(); it != end; ++it)
        (*it)->OnQuit();
}

//#if defined(EDITOR)
void
ComponentsManager::SerializeMarkedComponents(BitStream &stream)
{
    for (auto it = lists.Begin(), end = lists.End(); it != end; ++it)
        (*it)->OnSerializeComponents(stream);
}
//#endif

} // namespace Framework
