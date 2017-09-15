#pragma once

#include "Core/SmartPtr.h"
#include "Render/Resources/Resource.h"
#include "Render/RenderObjects.h"
#include "Render/Image/Image.h"

namespace Framework {

class Texture : public Resource {
    DeclareClassInfo;
protected:
    SmartPtr<RHI::TextureBuffer> buffer;
    Array<Image> images;

    virtual bool LoadImpl();
    virtual void UnloadImpl();
    virtual bool CloneImpl(WeakPtr<Resource> source);
    virtual uint32_t ComputeSize();

    void CreateFromImages();
public:
    Texture();
    virtual ~Texture();

    virtual uint32_t GetAccessModes() const;

    const SmartPtr<RHI::TextureBuffer>& GetBuffer() const;
    const Image& GetImage(uint8_t level = 0) const;
    // ToDo: SetImage (check width/height except if level == 0 - format doesn't matter, convert)
    //const Image::Image& GetCubeImage(CubeFace face, uint8_t level = 0) const;

    void Load(const RHI::TextureBufferDesc &desc);
    void Load(const SmartPtr<RHI::TextureBuffer> &_buffer);

    void GetPixels(Color *dstColors, uint8_t level = 0); // ToDo: refactor
    void SetPixels(const Color *srcColors, uint8_t level = 0); // ToDo: refactor
    void Apply(int32_t level = RHI::LockInfo::kGenerateMipmaps);
/*
    void GetCubePixels(CubeFace face, Color *dstColors, uint8_t level = 0);
    void SetCubePixels(CubeFace face, const Color *srcColors, uint8_t level = 0);
    void CubeApply(CubeFace face, uint8_t level = LockInfo::kGenerateMipmaps);
*/
    void LoadImages();
    void FreeImages();
};

inline const SmartPtr<RHI::TextureBuffer>&
Texture::GetBuffer() const
{
    return buffer;
}

} // namespace Framework
