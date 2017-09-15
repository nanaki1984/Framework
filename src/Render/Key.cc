#include "Render/Key.h"
#include "Math/Math.h"

namespace Framework {
    namespace RHI {

Key::Key()
{ }

Key::Key(const KeyCode &code)
{
    cameraDepth = code.bytes[0];
    cameraLayer = ((code.bytes[1] & 0xF0) >> 4);
    isCommand   = (0 == (code.bytes[1] & 0x08));

    if (isCommand)
        sequenceNumber = code.bytes[1] & 0x7;
    else
        translucency = static_cast<BaseShaderProgram::Translucency>(code.bytes[1] & 0x7);

    uint16_t tmp;
    if (isCommand) {
        commandId = code.bytes[2];

        switch (commandId) {
            case kCommandSetRenderTarget:
                rtCubeface = static_cast<CubeFace>(code.bytes[3]);
                memcpy(&rtId, code.bytes + 4, 4);
                break;
            case kCommandSetViewport:
                memcpy(&viewport.x, code.bytes + 3, 2);
                memcpy(&viewport.y, code.bytes + 5, 2);
                memcpy(&viewport.width, code.bytes + 7, 2);
                memcpy(&viewport.height, code.bytes + 9, 2);
                memcpy(&tmp, code.bytes + 11, 2);
                viewport.minZ = -1.0f + (tmp * 2.0f / 65535.0f);
                memcpy(&tmp, code.bytes + 13, 2);
                viewport.maxZ = -1.0f + (tmp * 2.0f / 65535.0f);
                break;
            case kCommandClear:
                clearFlags = static_cast<BaseRenderer::ClearFlags>(code.bytes[3]);
                memcpy(&clearColor,   code.bytes +  4, 4);
                memcpy(&clearDepth,   code.bytes +  8, 4);
                memcpy(&clearStencil, code.bytes + 12, 4);
                break;
            case kCommandDispatch:
                memcpy(&computeShaderId,       code.bytes +  3, 4);
                memcpy(&numGroupsX,            code.bytes +  7, 2);
                memcpy(&numGroupsY,            code.bytes +  9, 2);
                memcpy(&numGroupsZ,            code.bytes + 11, 2);
                memcpy(&dispatchParamsBlockId, code.bytes + 13, 2);
                break;
            default:
                assert(false);
                break;
        }
    } else {
        if (BaseShaderProgram::Opaque == translucency) {
            memcpy(&sortingOrder, code.bytes + 2, 1);

            memcpy(&materialId,   code.bytes + 3, 4);
            memcpy(&materialPass, code.bytes + 7, 1);

            subMeshIndex = (materialPass & 0x0F);
            materialPass >>= 4;

            memcpy(&meshId, code.bytes + 8, 4);

            memcpy(&tmp, code.bytes + 12, 2);
            depth = tmp / 65535.0f;

            memcpy(&paramsBlockId, code.bytes + 14, 2);
        } else {
            memcpy(&sortingOrder, code.bytes + 2, 1);

            memcpy(&tmp, code.bytes + 3, 2);
            depth = tmp / 65535.0f;

            memcpy(&materialId,   code.bytes + 5, 4);
            memcpy(&materialPass, code.bytes + 9, 1);

            subMeshIndex = (materialPass & 0x0F);
            materialPass >>= 4;

            memcpy(&meshId, code.bytes + 10, 4);

            memcpy(&paramsBlockId, code.bytes + 14, 2);
        }
    }
}

Key::operator KeyCode() const
{
    KeyCode code;

    code.bytes[0] = cameraDepth;
    code.bytes[1] = ((cameraLayer << 4) & 0xF0);
    code.bytes[1] |= (isCommand ? 0x00 : 0x08);

    if (isCommand)
        code.bytes[1] |= (sequenceNumber & 0x07);
    else
        code.bytes[1] |= (translucency & 0x07);

    uint16_t tmp;
    if (isCommand) {
        code.bytes[2] = commandId;

        switch (commandId) {
            case kCommandSetRenderTarget:
                code.bytes[3] = rtCubeface;
                memcpy(code.bytes + 4, &rtId, 4);
                break;
            case kCommandSetViewport:
                memcpy(code.bytes + 3, &viewport.x, 2);
                memcpy(code.bytes + 5, &viewport.y, 2);
                memcpy(code.bytes + 7, &viewport.width, 2);
                memcpy(code.bytes + 9, &viewport.height, 2);
                tmp = (uint16_t)floorf((viewport.minZ + 1.0f) * 0.5f * 65535.0f);
                memcpy(code.bytes + 11, &tmp, 2);
                tmp = (uint16_t)floorf((viewport.maxZ + 1.0f) * 0.5f * 65535.0f);
                memcpy(code.bytes + 13, &tmp, 2);
                break;
            case kCommandClear:
                code.bytes[3] = clearFlags;
                memcpy(code.bytes +  4, &clearColor, 4);
                memcpy(code.bytes +  8, &clearDepth, 4);
                memcpy(code.bytes + 12, &clearStencil, 4);
                break;
            case kCommandDispatch:
                memcpy(code.bytes +  3, &computeShaderId, 4);
                memcpy(code.bytes +  7, &numGroupsX, 2);
                memcpy(code.bytes +  9, &numGroupsY, 2);
                memcpy(code.bytes + 11, &numGroupsZ, 2);
                memcpy(code.bytes + 13, &dispatchParamsBlockId, 2);
                break;
            default:
                assert(false);
                break;
        }
    } else {
        if (BaseShaderProgram::Opaque == translucency) {
            memcpy(code.bytes + 2, &sortingOrder, 1);

            memcpy(code.bytes + 3, &materialId, 4);

            uint8_t passAndSubMesh = ((materialPass << 4) & 0xF0) | (subMeshIndex & 0x0F);
            memcpy(code.bytes + 7, &passAndSubMesh, 1);

            memcpy(code.bytes + 8, &meshId, 4);

            tmp = (uint16_t)floorf(depth * 65535.0f);
            memcpy(code.bytes + 12, &tmp, 2);

            memcpy(code.bytes + 14, &paramsBlockId, 2);
        } else {
            memcpy(code.bytes + 2, &sortingOrder, 1);

            tmp = (uint16_t)floorf(depth * 65535.0f);
            memcpy(code.bytes + 3, &tmp, 2);

            memcpy(code.bytes + 5, &materialId, 4);

            uint8_t passAndSubMesh = ((materialPass << 4) & 0xF0) | (subMeshIndex & 0x0F);
            memcpy(code.bytes + 9, &passAndSubMesh, 1);

            memcpy(code.bytes + 10, &meshId, 4);

            memcpy(code.bytes + 14, &paramsBlockId, 2);
        }
    }

    return code;
}

Key&
Key::Sequence(uint8_t _sequenceNumber)
{
    isCommand = true;
    sequenceNumber = _sequenceNumber;
    return (*this);
}

Key&
Key::ResetRenderTarget()
{
    assert(isCommand);
    commandId = kCommandSetRenderTarget;
    rtId = kResetRenderTarget;
    return (*this);
}

Key&
Key::SetRenderTarget(const SmartPtr<RenderTarget> &rt,
                     CubeFace cubeFace)
{
    assert(isCommand);
    commandId = kCommandSetRenderTarget;
    rtId = rt->GetId();
    rtCubeface = cubeFace;
    return (*this);
}

Key&
Key::SetViewport(const Viewport &_viewport)
{
    assert(isCommand);
    commandId = kCommandSetViewport;
    viewport = _viewport;
    return (*this);
}

Key&
Key::Clear(BaseRenderer::ClearFlags _clearFlags,
           uint32_t _clearColor,
           float _clearDepth,
           uint32_t _clearStencil)
{
    assert(isCommand);
    commandId = kCommandClear;
    clearFlags = _clearFlags;
    clearColor = _clearColor;
    clearDepth = _clearDepth;
    clearStencil = _clearStencil;
    return (*this);
}

Key&
Key::Dispatch(const WeakPtr<ComputeShader> &computeShader,
              int _numGroupsX, int _numGroupsY, int _numGroupsZ,
              uint16_t _paramsBlockId)
{
    assert(isCommand);
    commandId = kCommandDispatch;
    computeShaderId = computeShader->GetId();
    numGroupsX = _numGroupsX;
    numGroupsY = _numGroupsY;
    numGroupsZ = _numGroupsZ;
    dispatchParamsBlockId = _paramsBlockId;
    return (*this);
}

Key&
Key::DrawCall(uint8_t _sortingOrder,
              const WeakPtr<Material> &material,
              uint8_t _materialPass,
              const WeakPtr<Mesh> &mesh,
              uint8_t _subMeshIndex,
              uint16_t _paramsBlockId,
              float _depth)
{
    isCommand = false;
    translucency = material->GetShader()->GetProgram()->GetTranslucency();
    sortingOrder = _sortingOrder;
    materialId = material->GetId();
    materialPass = _materialPass;
    meshId = mesh->GetId();
    subMeshIndex = _subMeshIndex;
    paramsBlockId = _paramsBlockId;
    depth = Math::Clamp01(_depth);
    return (*this);
}

    } // namespace RHI
} // namespace Framework
