#include "Render/Resources/DDSLoader.h"
#include "Core/Collections/Array.h"
#include "Core/Memory/Memory.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Memory/ScratchAllocator.h"
#include "Core/IO/FileServer.h"
#include "Core/IO/BitStream.h"
#include "Core/Log.h"

/*
 * D3 - DDS loading
 * 09-04-09: Created!
 * 16-04-09: Now vertically flips DDS files. Wasn't that easy, since you need to flip each 4x4 pixel block.
 * FUTURE:
 * - Add max mipmap level (if DDS file doesn't contain all mipmap levels)
 * (C) 2009 Cruden BV
 */

// Magic
#define MAGIC_DDS 0x20534444      // DDS<space>

// DXT<n> id's
#define ID_DXT1   0x31545844
#define ID_DXT3   0x33545844
#define ID_DXT5   0x35545844

namespace Framework {
    namespace DDS {

DDSLoader::DDSLoader()
{
    mipmapLevels=0;
    wid=hgt=0;
    format=0;
    compressionType=0;
    internalFormat=ImageFormat::InvalidFormat;
    blockSize=0;
}

DDSLoader::~DDSLoader()
{ }

static void ConvertARGB2RGBA(unsigned char *a,int n)
//static void ConvertBGRA2RGBA(char *a,int n)
{
    // In hex dump: BGRA -> RGBA
    int i;
    unsigned char t;
    n/=4;
    for(i=0;i<n;i++)
    {
        t=a[2];
        a[2]=a[0];
        a[0]=t;
        a+=4;
    }
}

static void FlipDXT1BlockFull(unsigned char *data)
// A DXT1 block layout is:
// [0-1] color0.
// [2-3] color1.
// [4-7] color bitmap, 2 bits per pixel.
// So each of the 4-7 bytes represents one line, flipping a block is just
// flipping those bytes.
// Note that http://src.chromium.org/viewvc/chrome/trunk/src/o3d/core/cross/bitmap_dds.cc?view=markup&pathrev=21227
// contains an error in the last line: data[6]=data[5] is a bug!
{
    //return;
    unsigned char tmp;
    
    tmp=data[4];
    data[4]=data[7];
    data[7]=tmp;
    
    tmp=data[5];
    data[5]=data[6];
    data[6]=tmp;
    
    //data[4]=data[5]=data[6]=data[7]=0;
}

static void FlipDXT3BlockFull(unsigned char *block)
// Flips a full DXT3 block in the y direction.
{
    // A DXT3 block layout is:
    // [0-7]  alpha bitmap, 4 bits per pixel.
    // [8-15] a DXT1 block.
    
    // We can flip the alpha bits at the byte level (2 bytes per line).
    unsigned char tmp = block[0];
    block[0] = block[6];
    block[6] = tmp;
    tmp = block[1];
    block[1] = block[7];
    block[7] = tmp;
    tmp = block[2];
    block[2] = block[4];
    block[4] = tmp;
    tmp = block[3];
    block[3] = block[5];
    block[5] = tmp;
    
    // And flip the DXT1 block using the above function.
    FlipDXT1BlockFull(block + 8);
}
/*
static void QByteDump(unsigned char *a,int n)
{
    int i,j,bits;
    char buf[1024],*d;
    
    qdbg("Bitdump at %p",a);
    bits=0;
    d=buf;
    for(i=0;i<n;i++)
    {
        for(j=0;j<8;j++)
        {
            if(a[i]&(1<<j))*d++='1';
            else           *d++='0';
            if((bits%12)==11)*d++='.';
            bits++;
        }
    }
    *d=0;
    qdbg("%s\n",buf);
}*/
static void FlipDXT5BlockFull(unsigned char *block)
// From http://src.chromium.org/viewvc/chrome/trunk/src/o3d/core/cross/bitmap_dds.cc?view=markup&pathrev=21227
// Original source contained bugs; fixed here.
{
    // A DXT5 block layout is:
    // [0]    alpha0.
    // [1]    alpha1.
    // [2-7]  alpha bitmap, 3 bits per pixel.
    // [8-15] a DXT1 block.
    
    // The alpha bitmap doesn't easily map lines to bytes, so we have to
    // interpret it correctly.  Extracted from
    // http://www.opengl.org/registry/specs/EXT/texture_compression_s3tc.txt :
    //
    //   The 6 "bits" bytes of the block are decoded into one 48-bit integer:
    //
    //     bits = bits_0 + 256 * (bits_1 + 256 * (bits_2 + 256 * (bits_3 +
    //                   256 * (bits_4 + 256 * bits_5))))
    //
    //   bits is a 48-bit unsigned integer, from which a three-bit control code
    //   is extracted for a texel at location (x,y) in the block using:
    //
    //       code(x,y) = bits[3*(4*y+x)+1..3*(4*y+x)+0]
    //
    //   where bit 47 is the most significant and bit 0 is the least
    //   significant bit.
    //QByteDump(block+2,6);
    
    // From Chromium (source was buggy)
    unsigned int line_0_1 = block[2] + 256 * (block[3] + 256 * block[4]);
    unsigned int line_2_3 = block[5] + 256 * (block[6] + 256 * block[7]);
    // swap lines 0 and 1 in line_0_1.
    unsigned int line_1_0 = ((line_0_1 & 0x000fff) << 12) |
    ((line_0_1 & 0xfff000) >> 12);
    // swap lines 2 and 3 in line_2_3.
    unsigned int line_3_2 = ((line_2_3 & 0x000fff) << 12) |
    ((line_2_3 & 0xfff000) >> 12);
    block[2] = line_3_2 & 0xff;
    block[3] = (line_3_2 & 0xff00) >> 8;
    block[4] = (line_3_2 & 0xff0000) >> 16;
    block[5] = line_1_0 & 0xff;
    block[6] = (line_1_0 & 0xff00) >> 8;
    block[7] = (line_1_0 & 0xff0000) >> 16;
    
    //QByteDump(block+2,6);
    
    // And flip the DXT1 block using the above function.
    FlipDXT1BlockFull(block+8);
}

#ifdef OBS
static void FlipDXT35BlockOLD(char *data)
// Flip one 4x4 block for DXT3/5
{
    char tmp;
    
    tmp=data[4];
    data[4]=data[7];
    data[7]=tmp;
    
    tmp=data[5];
    data[5]=data[6];
    data[6]=tmp;
    
    // 2nd row
    tmp=data[12];
    data[12]=data[15];
    data[15]=tmp;
    
    tmp=data[13];
    data[13]=data[14];
    data[14]=tmp;
    
#ifdef OBS
    // Original 1-9-09
    char tmp;
    
    tmp=data[4];
    data[4]=data[7];
    data[7]=tmp;
    
    tmp=data[5];
    data[5]=data[6];
    data[6]=tmp;
    
    // 2nd row
    tmp=data[12];
    data[12]=data[15];
    data[15]=tmp;
    
    tmp=data[13];
    data[13]=data[14];
    data[14]=tmp;
#endif
}
#endif

bool DDSLoader::Load(const char *filename, Array<Image> &images)
{
    int magic;
    int i;
    unsigned char *temp=0;

    BitStream stream(Memory::GetAllocator<ScratchAllocator>());
    if (0 == FileServer::Instance()->ReadOnly(filename, stream)) {
        stream >> magic;
        if (MAGIC_DDS == magic) {
            // Direct3D 9 format
            D3D_SurfaceDesc2 header;
            stream >> header;

            // Remember info for users of this object
            wid=header.dwWidth;
            hgt=header.dwHeight;
            mipmapLevels=header.dwMipMapCount;

            //int nBytes;

            // Number of pixels
            //nBytes=header.dwHeight*header.dwWidth;
            // Block size default
            blockSize=16;

            // DXT5?
            if (header.ddpfPixelFormat.dwFlags & DDPF_FOURCC) {
                // Compressed
                format=DDS_FORMAT_YCOCG;      // ???????? Not right
                unsigned int fourCC;
                fourCC=header.ddpfPixelFormat.dwFourCC;
                if(fourCC==ID_DXT1)
                {
                    blockSize=8;
                    internalFormat = ImageFormat::DXT1_RGB;
                } else if(fourCC==ID_DXT3)
                {
                    internalFormat = ImageFormat::DXT3_RGBA;
                } else if(fourCC==ID_DXT5)
                {
                    // DXT5
                    internalFormat = ImageFormat::DXT5_RGBA;
                } else
                {
                    char buf[5];
                    buf[0]=fourCC&255;
                    buf[1]=(fourCC>>8)&255;
                    buf[2]=(fourCC>>16)&255;
                    buf[3]=(fourCC>>24)&255;
                    buf[4]=0;
                    Log::Instance()->Write(Log::Warning, "DDSLoader:Load(%s); unknown compressed format (%s)", filename, buf);
                    goto fail;
                }
                // DXT5 compression
                //bytes/=4;
            } else if(header.ddpfPixelFormat.dwRGBBitCount==32)
            {
                format=DDS_FORMAT_RGBA8;
                internalFormat = ImageFormat::A8R8G8B8;
                // Calculate bytes for highest mipmap level
                //bytes=header.dwHeight*header.dwWidth*header.ddpfPixelFormat.dwRGBBitCount/8;
                //nBytes=nBytes*header.ddpfPixelFormat.dwRGBBitCount/8;
            } else //if(header.ddpfPixelFormat.dwRGBBitCount!=32)
            {
                Log::Instance()->Write(Log::Warning, "DDSLoader:Load(%s); unknown DDS format (rgb bitcount not 32, not DXT5)", filename);
                goto fail;
            }

            images.SetCapacity(header.dwMipMapCount);

            // Read all mipmap levels
            int w,h;
            w=wid;
            h=hgt;
            for(i=0;i<header.dwMipMapCount&&i<MAX_MIPMAP_LEVEL;i++)
            {
                // Deduce # of bytes
                // Close to the higher mipmap levels, wid or hgt may become 0; keep things at 1
                if(w==0)w=1;
                if(h==0)h=1;

                Image level(Memory::GetAllocator<MallocAllocator>(), internalFormat, w, h);

                if(format==DDS_FORMAT_RGBA8)
                {
                    // RGBA8
                    //nBytes=w*h*4;
                } else
                {
                    // DXTx
                    //nBytes=((w+3)/4)*((h+3)/4)*blockSize;
                    if(i==0)
                    {
                        // Create temp buffer to flip DDS
                        temp=(unsigned char*)Memory::GetAllocator<ScratchAllocator>().Allocate(level.GetSize(), 1);
                    }
                }
                
                //bytes[i]=nBytes;
                /*pixels[i]=(unsigned char*)qalloc(nBytes);
                if(!pixels[i])
                {
                    qerr("DDDS:Load(%s); out of memory for mipmap level %d",fname,i);
                    goto fail;
                }*/
                
                if(format!=DDS_FORMAT_RGBA8)
                {
                    // First read in temp buffer
                    stream.ReadBytes(temp, level.GetSize());

                    // Flip & copy to actual pixel buffer
                    int j,widBytes,k;
                    unsigned char *s,*d;
                    widBytes=((w+3)/4)*blockSize;
                    s=temp;
                    //d=pixels[i]+((h+3)/4-1)*widBytes;
                    d=(unsigned char*)uintptr_t(level.GetPixels())+((h+3)/4-1)*widBytes;
                    //int count=0;
                    for(j=0;j<(h+3)/4;j++)
                    {
                        memcpy(d,s,widBytes);
                        if(blockSize==8)
                        {
                            for(k=0;k<widBytes/blockSize;k++)
                                FlipDXT1BlockFull(d+k*blockSize);
                        } else
                        {
                            // DXT3, DXT5
                            if(compressionType==DDS_COMPRESS_BC2)
                            {
                                // DXT3
                                for(k=0;k<widBytes/blockSize;k++)
                                    FlipDXT3BlockFull((unsigned char*)d+k*blockSize);
                            } else
                            {
                                // DXT5
                                for(k=0;k<widBytes/blockSize;k++)
                                    FlipDXT5BlockFull((unsigned char*)d+k*blockSize);
                            }
                        }
                        s+=widBytes;
                        d-=widBytes;
                        //count+=widBytes;
                        //qdbg("j=%d/%d - count=%d (total %d)\n",j,h,count,nBytes); QWait(1);
                    }
                    //      count=123;
                    //f->Read(pixels[i],nBytes);
                } else
                {
                    // RGBA8
                    if(format==DDS_FORMAT_RGBA8)
                    {
                        ConvertARGB2RGBA((unsigned char*)level.GetPixels(), level.GetSize());
                    }
                }
                images.PushBack(level);
                // Next level is smaller
                w/=2;
                h/=2;
            }
            // Release temp buffer
            Memory::GetAllocator<ScratchAllocator>().Free(temp);

			return true;
		}
	}

	return false;
fail:
    return false;
}

    } // namespace DDS
} // namespace Framework
