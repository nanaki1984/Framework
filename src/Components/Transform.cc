#include "Components/Transform.h"
#include "Core/Collections/Hash.h"
#include "Managers/TransformsManager.h"
#include "Managers/GetManager.h"
#include "Game/Entity.h"
#include "Game/ComponentsList.h"

namespace Framework {

TransformChildren::TransformChildren()
: head(nullptr),
  tail(nullptr),
  size(0)
{ }

TransformChildren::TransformChildren(TransformChildren &&other)
: head(other.head),
  tail(other.tail),
  size(other.size)
{
	other.head = nullptr;
	other.tail = nullptr;
	other.size = 0;
}

TransformChildren::~TransformChildren()
{
	assert(0 == size);
}

TransformChildren&
TransformChildren::operator = (TransformChildren &&other)
{
    head = other.head;
    tail = other.tail;
    size = other.size;

    other.head = nullptr;
    other.tail = nullptr;
    other.size = 0;

    return (*this);
}

void
TransformChildren::Insert(Transform *child)
{
	if (Handle<Transform>::Null == tail)
    {
		assert(0 == size);
		head = tail = child;
        child->siblingsNode.prev = Handle<Transform>::Null;
        child->siblingsNode.next = Handle<Transform>::Null;
	}
	else
    {
		tail->siblingsNode.next = child;
		child->siblingsNode.prev = tail;
        child->siblingsNode.next = Handle<Transform>::Null;
		tail = child;
	}
	++size;
}

void
TransformChildren::Remove(Transform *child)
{
    Handle<Transform> prev = child->siblingsNode.prev,
			          next = child->siblingsNode.next;

    child->siblingsNode.prev = Handle<Transform>::Null;;
    child->siblingsNode.next = Handle<Transform>::Null;;

    if (head.EqualsTo(child))
    {
        if (tail.EqualsTo(child))
        {
            assert(1 == size);
            head = tail = Handle<Transform>::Null;
        }
        else
        {
            head = next;
            assert(head.IsValid());
            head->siblingsNode.prev = Handle<Transform>::Null;
        }
    }
    else if (tail.EqualsTo(child))
    {
        tail = prev;
        assert(tail.IsValid());
        tail->siblingsNode.next = Handle<Transform>::Null;
    }
    else
    {
        assert(prev != Handle<Transform>::Null && next != Handle<Transform>::Null);
        prev->siblingsNode.next = next;
        next->siblingsNode.prev = prev;
    }

    assert(size > 0);
    --size;
}

void
TransformChildren::Clear(const Handle<Transform> &parent)
{
	Handle<Transform> node = head, tmp;
	while (node != Handle<Transform>::Null) {
		tmp = node;
		node = node->siblingsNode.next;
        
        tmp->SetParent(parent, true);
	}
    head = tail = Handle<Transform>::Null;
    size = 0;
}

const Handle<Transform>&
TransformChildren::Find(const StringHash &name) const
{
	const Handle<Transform>* it = &head;
	while (it->IsValid()) {
		if ((*it)->name == name)
			return *it;
		it = &(*it)->siblingsNode.next;
	}
	return Handle<Transform>::Null;
}

const Handle<Transform>&
TransformChildren::GetFirst() const
{
	return head;
}

const Handle<Transform>&
TransformChildren::GetLast() const
{
	return tail;
}

const Handle<Transform>&
TransformChildren::GetNext(const Handle<Transform> &child) const
{
	return child->siblingsNode.next;
}

const Handle<Transform>&
TransformChildren::GetPrevious(const Handle<Transform> &child) const
{
	return child->siblingsNode.prev;
}

DefineClassInfo(Framework::Transform, Framework::Component);
DefineComponent(Framework::Transform, -1000000);

Transform::Transform()
: manager(GetManager<TransformsManager>()),
  name("Entity"),
  root(this),
  localPosition(Math::Vector3::Zero),
  localRotation(Math::Quaternion::Identity),
  localScale(Math::Vector3::One),
  worldPosition(Math::Vector3::Zero),
  worldRotation(Math::Quaternion::Identity),
  worldScale(Math::Vector3::One),
  localToWorld(Math::Matrix::Identity),
  worldToLocal(Math::Matrix::Identity),
  updateMatrices(true)
{
    manager->RegisterTransform(this, localToWorld, TransformsManager::kParentNull);
}

Transform::Transform(const StringHash &_name)
: manager(GetManager<TransformsManager>()),
  name(_name),
  root(this),
  localPosition(Math::Vector3::Zero),
  localRotation(Math::Quaternion::Identity),
  localScale(Math::Vector3::One),
  worldPosition(Math::Vector3::Zero),
  worldRotation(Math::Quaternion::Identity),
  worldScale(Math::Vector3::One),
  localToWorld(Math::Matrix::Identity),
  worldToLocal(Math::Matrix::Identity),
  updateMatrices(true)
{
	manager->RegisterTransform(this, localToWorld, TransformsManager::kParentNull);
}

Transform::Transform(const StringHash &_name, const Handle<Transform> &_parent)
: manager(GetManager<TransformsManager>()),
  name(_name),
  root(_parent->root),
  localPosition(Math::Vector3::Zero),
  localRotation(Math::Quaternion::Identity),
  localScale(Math::Vector3::One),
  worldPosition(Math::Vector3::Zero),
  worldRotation(Math::Quaternion::Identity),
  worldScale(Math::Vector3::One),
  localToWorld(Math::Matrix::Identity),
  worldToLocal(Math::Matrix::Identity),
  updateMatrices(true)
{
	parent->children.Insert(this);

	manager->RegisterTransform(this, localToWorld, _parent->id);
}

Transform::Transform(const Transform &other)
: Component(other),
  manager(other.manager),
  name(other.name),
  root(other.root),
  parent(other.parent),
  localPosition(other.localPosition),
  localRotation(other.localRotation),
  localScale(other.localScale),
  updateMatrices(false)
{
    if (other.updateMatrices)
        other.RefreshWorld();

    worldPosition = other.worldPosition;
    worldRotation = other.worldRotation;
    worldScale = other.worldScale;
    localToWorld = other.localToWorld;
    worldToLocal = other.worldToLocal;

    Transform *p = parent;
    if (p != nullptr)
        p->children.Insert(this);

    manager->RegisterTransform(this, localToWorld, nullptr == p ? TransformsManager::kParentNull : p->id);
}

Transform::Transform(Transform &&other)
: Component(std::forward<Component>(other)),
  manager(other.manager),
  name(other.name),
  siblingsNode(other.siblingsNode),
  id(other.id),
  root(other.root),
  parent(other.parent),
  children(std::forward<TransformChildren>(other.children)),
  localPosition(other.localPosition),
  localRotation(other.localRotation),
  localScale(other.localScale),
  worldPosition(other.worldPosition),
  worldRotation(other.worldRotation),
  worldScale(other.worldScale),
  localToWorld(other.localToWorld),
  worldToLocal(other.worldToLocal),
  updateMatrices(other.updateMatrices)
{ }

Transform::~Transform()
{
	children.Clear(parent);
    this->SetParent(Handle<Transform>::Null, false);
	manager->UnregisterTransform(this, id);
}

void
Transform::RefreshWorld() const
{
    if (updateMatrices) {
        localToWorld = manager->GetWorldMatrix(id);
        worldToLocal = localToWorld.GetFastInverse();

        worldPosition = localToWorld.GetColumn(3);
        worldRotation = Math::Quaternion::FromMatrix(localToWorld);

        worldScale.x = localToWorld.GetColumn(0).GetMagnitude();
        worldScale.y = localToWorld.GetColumn(1).GetMagnitude();
        worldScale.z = localToWorld.GetColumn(2).GetMagnitude();

        updateMatrices = false;
    }
}

void
Transform::RefreshLocal()
{
    Math::Matrix local = manager->GetLocalMatrix(id);

    localPosition = local.GetColumn(3);
    localRotation = Math::Quaternion::FromMatrix(local);

    localScale.x = local.GetColumn(0).GetMagnitude();
    localScale.y = local.GetColumn(1).GetMagnitude();
    localScale.z = local.GetColumn(2).GetMagnitude();
}

void
Transform::SetName(const StringHash &newName)
{
    StringHash oldName = name;
    name = newName;
    manager->OnTransformNameChanged(this, oldName);
}

bool
Transform::HasChanged() const
{
    return manager->HasChanged(id);
}

void
Transform::SetParent(const Handle<Transform> &newParent, bool keepWorldTransform)
{
    if (parent == newParent)
        return;

    if (keepWorldTransform)
        this->RefreshWorld();

    if (parent.IsValid())
        parent->children.Remove(this);

    parent = newParent;
    if (parent.IsValid()) {
        parent->children.Insert(this);
        root = parent->root;
    } else
        root = this;

    manager->SetParent(id, parent.IsValid() ? parent->id : TransformsManager::kParentNull);
    if (keepWorldTransform)
    {
        manager->SetWorldMatrix(id, localToWorld);
        this->RefreshLocal();
    }

    updateMatrices = true;
}

void
Transform::AddChild(const Handle<Transform> &newChild, bool keepWorldTransform)
{
    if (newChild->parent.EqualsTo(this))
        return;

    if (keepWorldTransform)
        newChild->RefreshWorld();

    if (newChild->parent.IsValid())
        newChild->parent->children.Remove(this);

    newChild->parent = this;
    newChild->root = root;

    children.Insert(newChild);

    manager->SetParent(newChild->id, id);
    if (keepWorldTransform)
    {
        manager->SetWorldMatrix(newChild->id, newChild->localToWorld);
        newChild->RefreshLocal();
    }

    newChild->updateMatrices = true;
}

void
Transform::SetLocalPosition(const Math::Vector3 &p)
{
    localPosition = p;
    manager->SetLocalMatrix(id, Math::Matrix(localPosition, localRotation, localScale));
    updateMatrices = true;
}

void
Transform::SetLocalRotation(const Math::Quaternion &r)
{
    localRotation = r;
    manager->SetLocalMatrix(id, Math::Matrix(localPosition, localRotation, localScale));
    updateMatrices = true;
}

void
Transform::SetLocalScale(const Math::Vector3 &s)
{
    localScale = s;
    manager->SetLocalMatrix(id, Math::Matrix(localPosition, localRotation, localScale));
    updateMatrices = true;
}

const Math::Vector3&
Transform::GetWorldPosition() const
{
    this->RefreshWorld();
    return worldPosition;
}

const Math::Quaternion&
Transform::GetWorldRotation() const
{
    this->RefreshWorld();
    return worldRotation;
}

const Math::Vector3&
Transform::GetWorldScale() const
{
    this->RefreshWorld();
    return worldScale;
}

void
Transform::SetWorldPosition(const Math::Vector3 &p)
{
    this->RefreshWorld();

    worldPosition = p;

    Math::Matrix worldMatrix = Math::Matrix(worldPosition, worldRotation, worldScale);
    manager->SetWorldMatrix(id, worldMatrix);

    localToWorld = worldMatrix;
    worldToLocal = worldMatrix.GetFastInverse();
    updateMatrices = false;

    this->RefreshLocal();
}

void
Transform::SetWorldRotation(const Math::Quaternion &r)
{
    this->RefreshWorld();

    worldRotation = r;

    Math::Matrix worldMatrix = Math::Matrix(worldPosition, worldRotation, worldScale);
    manager->SetWorldMatrix(id, worldMatrix);

    localToWorld = worldMatrix;
    worldToLocal = worldMatrix.GetFastInverse();
    updateMatrices = false;

    this->RefreshLocal();
}

const Math::Matrix&
Transform::GetLocalToWorld() const
{
    this->RefreshWorld();
    return localToWorld;
}

const Math::Matrix&
Transform::GetWorldToLocal() const
{
    this->RefreshWorld();
    return worldToLocal;
}

Math::Vector3
Transform::TransformPoint(const Math::Vector3 &p) const
{
    this->RefreshWorld();
    return localToWorld.FastMultiplyPoint(p);
}

Math::Vector3
Transform::TransformVector(const Math::Vector3 &v) const
{
    this->RefreshWorld();
    return localToWorld.FastMultiplyVector(v);
}

Math::Vector3
Transform::InverseTransformPoint(const Math::Vector3 &p) const
{
    this->RefreshWorld();
    return worldToLocal.FastMultiplyPoint(p);
}

Math::Vector3
Transform::InverseTransformVector(const Math::Vector3 &v) const
{
    this->RefreshWorld();
    return worldToLocal.FastMultiplyVector(v);
}

void
Transform::Deserialize(SerializationServer *server, const BitStream &stream)
{
    Component::Deserialize(server, stream);

    uint32_t parentSerializationId;
    stream >> parentSerializationId;

    if (parentSerializationId != 0xffffffff)
        this->SetParent(server->GetLoadingObject(parentSerializationId).Cast<Transform>(), false);

    Math::Vector3 v;
    stream >> v;
    this->SetLocalPosition(v);

    Math::Quaternion r;
    stream >> r;
    this->SetLocalRotation(r);

    stream >> v;
    this->SetLocalScale(v);
}

void
Transform::OnCreate()
{
    Component::OnCreate();
    assert(!entity->transform.IsValid() || entity->transform.EqualsTo(this));
    entity->transform = this;
}

//#if defined(EDITOR)
void
Transform::OnSerialize(BitStream &stream)
{
    Component::OnSerialize(stream);

    stream << (parent.IsValid() ? parent->GetSerializationId() : 0xffffffffu)
           << this->GetLocalPosition()
           << this->GetLocalRotation()
           << this->GetLocalScale();
}
//#endif

} // namespace Framework
