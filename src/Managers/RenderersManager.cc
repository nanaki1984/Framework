#include "Math/Math.h"
#include "Managers/RenderersManager.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Collections/List.h"
#include "Game/ComponentsList.h"
#include "Core/Pool/Pool.h"
#include "Components/MeshRenderer.h"
#include "Components/Camera.h"

namespace Framework {

DefineClassInfoWithFactory(Framework::RenderersManager, Framework::BaseManager);
DefineManager(Framework::RenderersManager, 100);

RenderersManager::OctreeNode::OctreeNode(const OctreeNode &node)
{ }

RenderersManager::OctreeNode&
RenderersManager::OctreeNode::operator = (const OctreeNode &node)
{
	return (*this);
}

RenderersManager::OctreeNode::OctreeNode(const Math::Vector3 &_center, float _radius)
: center(_center),
  radius(_radius)
{
	Memory::Zero(children, 8);
}

RenderersManager::OctreeNode::OctreeNode(OctreeNode &&node)
: center(node.center),
  radius(node.radius),
  renderers(std::forward<List<Renderer, &Renderer::octreeNode>>(node.renderers))
{
	Memory::Move(children, node.children, 8);
}

RenderersManager::Frustum::Frustum(const Math::Matrix &viewProjection)
{
    planes[0].a = viewProjection(0, 3) + viewProjection(0, 0);
    planes[0].b = viewProjection(1, 3) + viewProjection(1, 0);
    planes[0].c = viewProjection(2, 3) + viewProjection(2, 0);
    planes[0].d = viewProjection(3, 3) + viewProjection(3, 0);

    planes[1].a = viewProjection(0, 3) - viewProjection(0, 0);
    planes[1].b = viewProjection(1, 3) - viewProjection(1, 0);
    planes[1].c = viewProjection(2, 3) - viewProjection(2, 0);
    planes[1].d = viewProjection(3, 3) - viewProjection(3, 0);

    planes[2].a = viewProjection(0, 3) + viewProjection(0, 1);
    planes[2].b = viewProjection(1, 3) + viewProjection(1, 1);
    planes[2].c = viewProjection(2, 3) + viewProjection(2, 1);
    planes[2].d = viewProjection(3, 3) + viewProjection(3, 1);

    planes[3].a = viewProjection(0, 3) - viewProjection(0, 1);
    planes[3].b = viewProjection(1, 3) - viewProjection(1, 1);
    planes[3].c = viewProjection(2, 3) - viewProjection(2, 1);
    planes[3].d = viewProjection(3, 3) - viewProjection(3, 1);

    planes[4].a = viewProjection(0, 3) + viewProjection(0, 2);
    planes[4].b = viewProjection(1, 3) + viewProjection(1, 2);
    planes[4].c = viewProjection(2, 3) + viewProjection(2, 2);
    planes[4].d = viewProjection(3, 3) + viewProjection(3, 2);

    planes[5].a = viewProjection(0, 3) - viewProjection(0, 2);
    planes[5].b = viewProjection(1, 3) - viewProjection(1, 2);
    planes[5].c = viewProjection(2, 3) - viewProjection(2, 2);
    planes[5].d = viewProjection(3, 3) - viewProjection(3, 2);
/*
    planes[0].a = viewProjection(3, 0) + viewProjection(0, 0);
	planes[0].b = viewProjection(3, 1) + viewProjection(0, 1);
	planes[0].c = viewProjection(3, 2) + viewProjection(0, 2);
	planes[0].d = viewProjection(3, 3) + viewProjection(0, 3);

	planes[1].a = viewProjection(3, 0) - viewProjection(0, 0);
	planes[1].b = viewProjection(3, 1) - viewProjection(0, 1);
	planes[1].c = viewProjection(3, 2) - viewProjection(0, 2);
	planes[1].d = viewProjection(3, 3) - viewProjection(0, 3);

	planes[2].a = viewProjection(3, 0) + viewProjection(1, 0);
	planes[2].b = viewProjection(3, 1) + viewProjection(1, 1);
	planes[2].c = viewProjection(3, 2) + viewProjection(1, 2);
	planes[2].d = viewProjection(3, 3) + viewProjection(1, 3);

	planes[3].a = viewProjection(3, 0) - viewProjection(1, 0);
	planes[3].b = viewProjection(3, 1) - viewProjection(1, 1);
	planes[3].c = viewProjection(3, 2) - viewProjection(1, 2);
	planes[3].d = viewProjection(3, 3) - viewProjection(1, 3);

	planes[4].a = viewProjection(3, 0) + viewProjection(2, 0);
	planes[4].b = viewProjection(3, 1) + viewProjection(2, 1);
	planes[4].c = viewProjection(3, 2) + viewProjection(2, 2);
	planes[4].d = viewProjection(3, 3) + viewProjection(2, 3);

	planes[5].a = viewProjection(3, 0) - viewProjection(2, 0);
	planes[5].b = viewProjection(3, 1) - viewProjection(2, 1);
	planes[5].c = viewProjection(3, 2) - viewProjection(2, 2);
	planes[5].d = viewProjection(3, 3) - viewProjection(2, 3);
*/
    int i = 0;
    for (;  i < 6; ++i)
        planes[i].Normalize();

	Math::Matrix invViewProj = viewProjection.GetInverse();
	for (i = 0; i < 8; ++i)
		corners[i] = invViewProj.MultiplyPoint(Math::Vector3::Corners[i]);
}

bool
RenderersManager::Frustum::Intersects(const Math::Bounds &bounds) const
{
	Math::Vector3 c = bounds.GetCenter(),
		          e = bounds.GetExtents();

    // check box outside frustum
    int i = 0;
    for (; i < 6; ++i)
    {
        Math::Vector3 absN(fabs(planes[i].a), fabs(planes[i].b), fabs(planes[i].c));

        float d  = planes[i].GetPointDistance(c),
              ne = Math::Vector3::Dot(absN, e);

        if ((d + ne) < 0.0f)
            return false;
    }

    // check frustum outside box
    int out;
    out = 0; for (i = 0; i < 8; ++i) { if (corners[i].x < bounds.min.x) ++out; } if (8 == out) return false;
    out = 0; for (i = 0; i < 8; ++i) { if (corners[i].x > bounds.max.x) ++out; } if (8 == out) return false;
    out = 0; for (i = 0; i < 8; ++i) { if (corners[i].y < bounds.min.y) ++out; } if (8 == out) return false;
    out = 0; for (i = 0; i < 8; ++i) { if (corners[i].y > bounds.max.y) ++out; } if (8 == out) return false;
    out = 0; for (i = 0; i < 8; ++i) { if (corners[i].z < bounds.min.z) ++out; } if (8 == out) return false;
    out = 0; for (i = 0; i < 8; ++i) { if (corners[i].z > bounds.max.z) ++out; } if (8 == out) return false;

    return true;
}

RenderersManager::OctreeNode*
RenderersManager::RequestNode(int depth, const Math::Vector3 &center)
{
	OctreeNode *node = octreeRoot;
	int d = 0;
    while (d < depth && node->radius > kOctreeMinRadius)
	{
		Math::Vector3 diff = center - node->center;
		int offset = (diff.x < 0.0f ? 0 : 1) |
					 (diff.y < 0.0f ? 0 : 2) |
					 (diff.z < 0.0f ? 0 : 4);

		if (nullptr == node->children[offset])
		{
			float halfRadius = node->radius * 0.5f;
			node->children[offset] = Memory::New<BlocksAllocator, OctreeNode>(node->center + Math::Vector3::Corners[offset] * halfRadius * 0.5f, halfRadius);
		}

		node = node->children[offset];
		++d;
	}

	return node;
}

void
RenderersManager::FreeNodesRecursively(OctreeNode *node)
{
	if (nullptr == node)
		return;

	for (int i = 0; i < 8; ++i)
		this->FreeNodesRecursively(node->children[i]);

	node->renderers.Clear();

	Memory::Delete<BlocksAllocator>(node);
}

void
RenderersManager::QueryRenderersRecursively(const Frustum &frustum, OctreeNode *node, Array<Handle<Renderer>> &out)
{
    if (nullptr == node)
        return;

    Math::Bounds nodeBounds(node->center, Math::Vector3::One * node->radius);
    if (!frustum.Intersects(nodeBounds))
        return;

    auto rndrNode = node->renderers.Begin();
    while (rndrNode != nullptr)
    {
        if (frustum.Intersects(rndrNode->GetBounds()))
            out.PushBack(rndrNode);

        rndrNode = node->renderers.GetNext(rndrNode);
    }

    for (int i = 0; i < 8; ++i)
        this->QueryRenderersRecursively(frustum, node->children[i], out);
}

RenderersManager::RenderersManager()
: octreeRoot(nullptr),
  lastQueryResult(Memory::GetAllocator<MallocAllocator>())
{ }

RenderersManager::~RenderersManager()
{
    this->FreeNodesRecursively(octreeRoot);
}

void
RenderersManager::OnUpdate()
{ }

void
RenderersManager::OnLateUpdate()
{ }

void
RenderersManager::OnRender()
{
	this->FreeNodesRecursively(octreeRoot);

    Math::Bounds octreeBounds;
	octreeBounds.Reset();

	MeshRenderer *rndrIt  = ComponentsList<MeshRenderer>::Instance()->Begin(),
				 *rndrEnd = ComponentsList<MeshRenderer>::Instance()->End();
    for (; rndrIt != rndrEnd; ++rndrIt)
    {
        if (!rndrIt->IsActive())
            continue;

        octreeBounds.Encapsulate(rndrIt->GetBounds().GetCenter());
    }

    octreeRoot = Memory::New<BlocksAllocator, OctreeNode>(octreeBounds.GetCenter(), std::max(kOctreeMinRadius, octreeBounds.GetSize().GetMagnitude()));

    rndrIt = ComponentsList<MeshRenderer>::Instance()->Begin();
    int c = 0;
	for (; rndrIt != rndrEnd; ++rndrIt)
	{
        if (!rndrIt->IsActive())
            continue;

        float div = floorf(octreeRoot->radius / rndrIt->GetBounds().GetSize().GetMagnitude()) * 0.5f;

		int depth = 0;
		while (depth < kOctreeMaxDepth && div >= 1.0f)
		{
			++depth;
			div *= 0.5f;
		}

		OctreeNode *node = this->RequestNode(depth, rndrIt->GetBounds().GetCenter());
		node->renderers.PushBack(rndrIt);
        ++c;
	}
}

void
RenderersManager::OnPause()
{ }

void
RenderersManager::OnResume()
{ }

void
RenderersManager::OnQuit()
{ }

uint32_t
RenderersManager::QueryRenderers(const Math::Matrix &viewProjection)
{
    lastQueryResult.Clear();
    this->QueryRenderersRecursively(Frustum(viewProjection), octreeRoot, lastQueryResult);
    return lastQueryResult.Count();
}

const Handle<Renderer>&
RenderersManager::GetRenderer(uint32_t index)
{
    return lastQueryResult[index];
}

} // namespace Managers
