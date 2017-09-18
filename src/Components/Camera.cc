#include <type_traits>
#include "Math/Math.h"
#include "Math/Matrix.h"
#include "Core/Application.h"
#include "Game/Entity.h"
#include "Game/ComponentsList.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Managers/RenderersManager.h"
#include "Render/RenderQueue.h"
#include "Render/Key.h"

namespace Framework {

DefineClassInfo(Framework::Camera, Framework::Component);
DefineComponent(Framework::Camera, 1000000);

Camera::Camera()
: pixelRect(0.0f, 0.0f, (float)Application::Instance()->GetScreenWidth(), (float)Application::Instance()->GetScreenHeight()),
  nearClipPlane(1.0f),
  farClipPlane(1000.0f),
  fieldOfView(60.0f),
  aspect(pixelRect.GetWidth() / pixelRect.GetHeight()),
  projectionOverride(false),
  projectionDirty(false),
  clearFlags(RHI::BaseRenderer::ClearAll),
  clearColor(Color::Black),
  depth(128)
{
	Math::MatrixProjection(fieldOfView * Math::Deg2Rad, aspect, nearClipPlane, farClipPlane, projection);
}

Camera::Camera(const Camera &other)
: Component(other),
  pixelRect(other.pixelRect),
  nearClipPlane(other.nearClipPlane),
  farClipPlane(other.farClipPlane),
  fieldOfView(other.fieldOfView),
  aspect(other.aspect),
  projection(other.projection),
  projectionOverride(other.projectionOverride),
  projectionDirty(other.projectionDirty),
  clearFlags(other.clearFlags),
  clearColor(other.clearColor),
  depth(other.depth)
{ }

Camera::Camera(Camera &&other)
: Component(std::forward<Component>(other)),
  pixelRect(other.pixelRect),
  nearClipPlane(other.nearClipPlane),
  farClipPlane(other.farClipPlane),
  fieldOfView(other.fieldOfView),
  aspect(other.aspect),
  projection(other.projection),
  projectionOverride(other.projectionOverride),
  projectionDirty(other.projectionDirty),
  clearFlags(other.clearFlags),
  clearColor(other.clearColor),
  depth(other.depth)
{ }

Camera::~Camera()
{ }

void
Camera::Deserialize(SerializationServer *server, const BitStream &stream)
{
    Component::Deserialize(server, stream);

    stream >> nearClipPlane
           >> farClipPlane
           >> fieldOfView
           >> aspect
           >> clearFlags
           >> clearColor
           >> depth;

    Math::MatrixProjection(fieldOfView * Math::Deg2Rad, aspect, nearClipPlane, farClipPlane, projection);
}

void
Camera::Render()
{
    float t0 = TimeServer::Instance()->GetMilliseconds();

	Math::Matrix view     = entity->GetTransform()->GetWorldToLocal(),
                 viewProj = view * this->GetProjection();

    auto rndrMng = GetManager<RenderersManager>();
    uint32_t count = rndrMng->QueryRenderers(viewProj);

    RHI::Viewport viewport;
    viewport.x      = (uint16_t)floorf(pixelRect.min.x);
    viewport.y      = (uint16_t)floorf(pixelRect.min.y);
    viewport.width  = (uint16_t)floorf(pixelRect.GetWidth());
    viewport.height = (uint16_t)floorf(pixelRect.GetHeight());
    viewport.minZ   = -1.0f;
    viewport.maxZ   =  1.0f;

    RHI::Key key;
    key.cameraDepth = depth;
    key.cameraLayer = 0; // ToDo: needed for complex renderers?

    key.Sequence(0).ResetRenderTarget();
    RenderQueue::Instance()->SendCommand(key);

    key.Sequence(0).SetViewport(viewport);
    RenderQueue::Instance()->SendCommand(key);

    key.Sequence(0).Clear(clearFlags, clearColor, 1.0f, 0);
    RenderQueue::Instance()->SendCommand(key);

    MaterialParamsBlock *matParamsBlock;
    for (uint32_t i = 0; i < count; ++i)
    {
        auto rndr = rndrMng->GetRenderer(i);
        if (!rndr.IsValid())
            continue;

        if (rndr->IsA<MeshRenderer>())
        {
            auto *meshRndr = rndr.Cast<MeshRenderer>();
            auto &mesh     = meshRndr->GetMesh();

            Math::Matrix worldView     = meshRndr->GetEntity()->GetTransform()->GetLocalToWorld(),
                         worldViewProj = worldView * viewProj;
            worldView.FastMultiply(view);

            for (uint32_t j = 0; j < mesh->GetSubMeshCount(); ++j)
            {
                uint32_t matParamsBlockId = RenderQueue::Instance()->GetMaterialParamsBlock(&matParamsBlock);
                matParamsBlock->AddMatrix("WorldViewProj", worldViewProj);

                float depth = -worldView.FastMultiplyPoint(mesh->GetSubMeshBounds(j).GetCenter()).z;
                depth = (depth - nearClipPlane) / (farClipPlane - nearClipPlane);

                key.DrawCall(meshRndr->GetSortingOrder(), meshRndr->GetMaterial(j), 0, mesh, j, matParamsBlockId, depth);
                RenderQueue::Instance()->SendCommand(key);
            }
        }
    }

	float t1 = TimeServer::Instance()->GetMilliseconds();
    Log::Instance()->Write(Log::Info, "Render camera time: %f (renderers: %d)", (t1 - t0), count);
}

const Rect&
Camera::GetPixelRect() const
{
	return pixelRect;
}

float
Camera::GetNearClipPlane() const
{
	return nearClipPlane;
}

float
Camera::GetFarClipPlane() const
{
	return farClipPlane;
}

float
Camera::GetFieldOfView() const
{
	return fieldOfView;
}

float
Camera::GetAspect() const
{
	return aspect;
}

RHI::BaseRenderer::ClearFlags
Camera::GetClearFlags() const
{
    return clearFlags;
}

const Color&
Camera::GetClearColor() const
{
    return clearColor;
}

uint8_t
Camera::GetDepth() const
{
    return depth;
}

const Math::Matrix&
Camera::GetProjection() const
{
	if (projectionDirty)
	{
		Math::MatrixProjection(fieldOfView * Math::Deg2Rad, aspect, nearClipPlane, farClipPlane, projection);
		projectionDirty = false;
	}

	return projection;
}

void
Camera::SetPixelRect(const Rect &rect)
{
	pixelRect = rect;
	aspect = pixelRect.GetWidth() / pixelRect.GetHeight();
	projectionDirty = !projectionOverride;
}

void
Camera::SetNearClipPlane(float distance)
{
	nearClipPlane = distance;
	projectionDirty = !projectionOverride;
}

void
Camera::SetFarClipPlane(float distance)
{
	farClipPlane = distance;
	projectionDirty = !projectionOverride;
}

void
Camera::SetFieldOfView(float angle)
{
	fieldOfView = angle;
	projectionDirty = !projectionOverride;
}

void
Camera::SetProjection(const Math::Matrix &matrix)
{
	projection = matrix;
	projectionOverride = true;
	projectionDirty = false;
}

void
Camera::ResetProjection()
{
	if (projectionOverride)
	{
		projectionOverride = false;
		projectionDirty = true;
	}
}

void
Camera::SetClearFlags(RHI::BaseRenderer::ClearFlags flags)
{
    clearFlags = flags;
}

void
Camera::SetClearColor(const Color &color)
{
    clearColor = color;
}

void
Camera::SetDepth(uint8_t cameraDepth)
{
    depth = cameraDepth;
}

//#if defined(EDITOR)
void
Camera::OnSerialize(BitStream &stream)
{
    Component::OnSerialize(stream);

    stream << nearClipPlane
           << farClipPlane
           << fieldOfView
           << aspect
           << clearFlags
           << clearColor
           << depth;
}
//#endif

} // namespace Framework
