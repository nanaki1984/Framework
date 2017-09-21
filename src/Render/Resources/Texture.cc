#include "Render/Resources/Texture.h"
#include "Core/Collections/Array.h"
#include "Core/IO/Path.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Memory/BlocksAllocator.h"
#include "Render/Resources/DDSLoader.h"

namespace Framework {

DefineClassInfoWithFactory(Framework::Texture, Framework::Resource);

Texture::Texture()
: images(Memory::GetAllocator<BlocksAllocator>())
{ }

Texture::~Texture()
{ }

uint32_t
Texture::GetAccessModes() const
{
    return ReadOnly | Writable;
}

bool
Texture::LoadImpl()
{
    String ext = Path::GetFileExtension(filename);
    if (ext == "dds") {
        DDS::DDSLoader ddsLoader;
        if (ddsLoader.Load(filename.AsCString(), images)) {
            this->CreateFromImages();
            this->FreeImages();

            return true;
        } else {
            return false;
        }
    }
    return false;
}

void
Texture::UnloadImpl()
{
    buffer.Reset();
    images.Clear();
}

bool
Texture::CloneImpl(WeakPtr<Resource> source)
{
    Texture *tex = source.Cast<Texture>();

    bool imagesWereFreed = tex->images.IsEmpty();
    if (imagesWereFreed)
        tex->LoadImages();

    images = tex->images;
    this->CreateFromImages();

    if (imagesWereFreed) {
        tex->FreeImages();
        this->FreeImages();
    }

    return true;
}

uint32_t
Texture::ComputeSize()
{
    return buffer->GetSize();
}

void
Texture::CreateFromImages()
{
    assert(!isLoaded);

    RHI::TextureBufferDesc desc;
    Memory::Zero(&desc);

    desc.width = images.Front().GetWidth();
    desc.height = images.Front().GetHeight();
    desc.format = images.Front().GetFormat();
    desc.type = RHI::BaseTextureBuffer::Texture2D;
    desc.mipmapsRangeMin = 0;

    Image *img = images.Begin();
    while (!img->IsAllocated()) {
        ++img;
        ++desc.mipmapsRangeMin;
    }

    desc.mipmapsRangeMax = desc.mipmapsRangeMin - 1;
    while (img->IsAllocated() && img < images.End()) {
        ++img;
        ++desc.mipmapsRangeMax;
    }

    buffer = SmartPtr<RHI::TextureBuffer>::MakeNew<BlocksAllocator>(desc);

    RHI::LockInfo lockInfo;
	Memory::Zero(&lockInfo);
    for (uint8_t lvl = desc.mipmapsRangeMin; lvl <= desc.mipmapsRangeMax; ++lvl) {
        lockInfo.mipmapLevel = lvl;
        buffer->Upload(lockInfo, images[lvl].GetPixels());
    }
}

void
Texture::Load(const RHI::TextureBufferDesc &desc)
{
    if (isLoaded)
        this->Unload();

    buffer = SmartPtr<RHI::TextureBuffer>::MakeNew<BlocksAllocator>(desc);
    images.Resize(desc.mipmapsRangeMax + 1);

    for (uint8_t lvl = desc.mipmapsRangeMin; lvl <= desc.mipmapsRangeMax; ++lvl) {
        images[lvl] = Image(Memory::GetAllocator<MallocAllocator>(),
                            buffer->GetFormat(),
                            RHI::MipmapSize(buffer->GetWidth(), lvl),
                            RHI::MipmapSize(buffer->GetHeight(), lvl));
    }

    isLoaded = true;
}

void
Texture::Load(const SmartPtr<RHI::TextureBuffer> &_buffer)
{
    if (isLoaded)
        this->Unload();

    buffer = _buffer;
}

void
Texture::GetPixels(Color *dstColors, uint8_t level)
{
    ImageFormat format = buffer->GetFormat();
    if (format.GetFlags() & ImageFormat::Compressed)
        return;

    if (images.IsEmpty())
        this->LoadImages();

    Image &img = images[level];
    assert(img.IsAllocated());
    img.GetPixels(dstColors, img.GetWidth() * img.GetHeight());
}

void
Texture::SetPixels(const Color *srcColors, uint8_t level)
{
    ImageFormat format = buffer->GetFormat();
    if (format.GetFlags() & ImageFormat::Compressed)
        return;

    if (images.IsEmpty())
        this->LoadImages();

    Image &img = images[level];
    assert(img.IsAllocated());
    img.SetPixels(srcColors, img.GetWidth() * img.GetHeight());
}

void
Texture::Apply(int32_t level)
{
    assert(isLoaded);
    if (images.IsEmpty())
        return;

    ImageFormat format = buffer->GetFormat();
    if (format.GetFlags() & ImageFormat::Compressed)
        return;

    RHI::LockInfo lockInfo;
    Memory::Zero(&lockInfo);
    lockInfo.mipmapLevel = level;
    buffer->Upload(lockInfo, images[RHI::LockInfo::kGenerateMipmaps == level ? 0 : level].GetPixels());
}

void
Texture::LoadImages()
{
    assert(isLoaded);
    if (!images.IsEmpty())
        return;
    
    uint8_t lvlMin, lvlMax;
    buffer->GetMipmapsRange(lvlMin, lvlMax);
    
    images.Clear();
    images.Resize(lvlMax + 1);
    
    RHI::LockInfo lockInfo;
    Memory::Zero(&lockInfo);
    for (uint8_t lvl = lvlMin; lvl <= lvlMax; ++lvl) {
        images[lvl] = Image(Memory::GetAllocator<MallocAllocator>(),
                            buffer->GetFormat(),
                            RHI::MipmapSize(buffer->GetWidth(), lvl),
                            RHI::MipmapSize(buffer->GetHeight(), lvl));
        
        lockInfo.mipmapLevel = lvl;
        buffer->Download(lockInfo, images[lvl].GetPixels());
    }
}

void
Texture::FreeImages()
{
    images.Clear();
}

const Image&
Texture::GetImage(uint8_t level) const
{
    assert(!images.IsEmpty());
    return images[level];
}

bool
Texture::PrepareForRendering(RenderQueue *renderQueue)
{
    if (RenderResource<RHI::TextureRenderData>::PrepareForRendering(renderQueue))
        clientRenderData.buffer = buffer;

    return true;
}

} // namespace Framework
