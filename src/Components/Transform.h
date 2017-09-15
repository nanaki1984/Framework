#pragma once

#include "Game/Component.h"
#include "Core/StringHash.h"
#include "Core/Collections/Hash_type.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"

namespace Framework {

class TransformsManager;
class Transform;

class TransformChildren {
private:
	Handle<Transform> head;
	Handle<Transform> tail;
	uint32_t size;
public:
	TransformChildren();
    TransformChildren(const TransformChildren &other) = delete;
	TransformChildren(TransformChildren &&other);
	~TransformChildren();

    TransformChildren& operator =(const TransformChildren &other) = delete;
    TransformChildren& operator =(TransformChildren &&other);
    
    void Insert(Transform *child);
	void Remove(Transform *child);
	void Clear(const Handle<Transform> &parent);

	uint32_t Count() const;

	const Handle<Transform>& Find(const StringHash &name) const;
	const Handle<Transform>& GetFirst() const;
	const Handle<Transform>& GetLast() const;
	const Handle<Transform>& GetNext(const Handle<Transform> &child) const;
	const Handle<Transform>& GetPrevious(const Handle<Transform> &child) const;
};

inline uint32_t
TransformChildren::Count() const
{
	return size;
}

class Transform : public Component {
    DeclareClassInfo;
	DeclareComponent;
protected:
    TransformsManager *manager;

    StringHash name;
    HandleListNode<Transform> siblingsNode;

    uint32_t id;

    Handle<Transform> root;
    Handle<Transform> parent;
    TransformChildren children;

    Math::Vector3 localPosition;
    Math::Quaternion localRotation;
    Math::Vector3 localScale;

    mutable Math::Vector3 worldPosition;
    mutable Math::Quaternion worldRotation;
    mutable Math::Vector3 worldScale;

    mutable Math::Matrix localToWorld;
    mutable Math::Matrix worldToLocal;
    mutable bool updateMatrices;

    void RefreshWorld() const;
    void RefreshLocal();
public:
    Transform();
	Transform(const StringHash &_name);
	Transform(const StringHash &_name, const Handle<Transform> &_parent);
    Transform(const Transform &other);
    Transform(Transform &&other);
    virtual ~Transform();

    const char* GetName() const;
    uint32_t GetHashCode() const;
    void SetName(const StringHash &newName);

    const Handle<Transform>& GetRoot() const;
    const Handle<Transform>& GetParent() const;
    const TransformChildren& GetChildren() const;

    bool HasChanged() const;

    void SetParent(const Handle<Transform> &newParent, bool keepWorldTransform);
    void AddChild(const Handle<Transform> &newChild, bool keepWorldTransform);

    const Math::Vector3& GetLocalPosition() const;
    const Math::Quaternion& GetLocalRotation() const;
    const Math::Vector3& GetLocalScale() const;

    void SetLocalPosition(const Math::Vector3 &p);
    void SetLocalRotation(const Math::Quaternion &r);
    void SetLocalScale(const Math::Vector3 &s);

    const Math::Vector3& GetWorldPosition() const;
    const Math::Quaternion& GetWorldRotation() const;
    const Math::Vector3& GetWorldScale() const;

    void SetWorldPosition(const Math::Vector3 &p);
    void SetWorldRotation(const Math::Quaternion &r);

    const Math::Matrix& GetLocalToWorld() const;
    const Math::Matrix& GetWorldToLocal() const;

    Math::Vector3 TransformPoint(const Math::Vector3 &p) const;
    Math::Vector3 TransformVector(const Math::Vector3 &v) const;
    Math::Vector3 InverseTransformPoint(const Math::Vector3 &p) const;
    Math::Vector3 InverseTransformVector(const Math::Vector3 &v) const;

    void Deserialize(SerializationServer *server, const BitStream &stream);
    void OnCreate();

//#if defined(EDITOR)
    void OnSerialize(BitStream &stream);
//#endif

	friend class TransformChildren;
    friend class TransformsManager;
};

inline const char*
Transform::GetName() const
{
    return name.string;
}

inline uint32_t
Transform::GetHashCode() const
{
    return name.hash;
}

inline const Handle<Transform>&
Transform::GetRoot() const
{
    return root;
}

inline const Handle<Transform>&
Transform::GetParent() const
{
    return parent;
}

inline const TransformChildren&
Transform::GetChildren() const
{
	return children;
}

inline const Math::Vector3&
Transform::GetLocalPosition() const
{
    return localPosition;
}

inline const Math::Quaternion&
Transform::GetLocalRotation() const
{
    return localRotation;
}

inline const Math::Vector3&
Transform::GetLocalScale() const
{
    return localScale;
}

} // namespace Framework
