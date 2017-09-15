#pragma once

#include "Core/Memory/Memory.h"
#include "Core/Memory/Allocator.h"

namespace Framework {
        
class BlocksAllocator : public Allocator {
    DeclareClassInfo;
    DeclareAllocator(BlocksAllocator);
private:
    struct Page {
        uint32_t blockSize;
        uint16_t freeBlocks;
        uint16_t usedBlocks;
        Page     *next;
    };

    struct Header {
        Page *page;
    };

    Allocator *baseAllocator;
    uint32_t pageSize;
    uint32_t pageSizeNoHeader;
    Page *firstPage;
    uint32_t totalAllocated;

    Page* AllocateNewPage(uint32_t blockSize);
    Page* RefitPage(Page *page, uint32_t blockSize);
    Header* AllocateNewBlock(Page *page);
    void DeallocateBlock(Header *block);
    Page* FindPage(uint32_t blockSize);
public:
    BlocksAllocator(Allocator *allocator, uint32_t _pageSize);
    virtual ~BlocksAllocator();

    virtual void* Allocate(size_t size, size_t align);
    virtual void Free(void *pointer);

    virtual size_t GetAllocatedSize(void *pointer);
    virtual size_t GetTotalAllocated();
};
        
} // namespace Framework
