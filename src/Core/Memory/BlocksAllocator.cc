#include "Core/Memory/BlocksAllocator.h"

namespace Framework {
        
DefineClassInfo(Framework::BlocksAllocator, Framework::Allocator);
DefineAllocator(Framework::BlocksAllocator);

BlocksAllocator::Page*
BlocksAllocator::AllocateNewPage(uint32_t blockSize)
{/*
    assert(blockSize <= pageSize);
    uint32_t numBlocks = pageSize / blockSize,
             numFlags  = (numBlocks / 32) + (numBlocks % 32 > 0 ? 1 : 0);

    uint32_t newPageSize = sizeof(Page) + numFlags * 4;
    newPageSize += (numBlocks * blockSize);
*/
    assert(blockSize < pageSize);
    uint32_t numBlocks = pageSizeNoHeader / blockSize,
             numFlags  = (numBlocks / 32) + (numBlocks % 32 > 0 ? 1 : 0);

    uint32_t newPageSize = sizeof(Page) + numFlags * 4;
    newPageSize += (numBlocks * blockSize);
    /*if*/while (newPageSize > pageSize) {
        --numBlocks;
        newPageSize -= blockSize;
        //assert((newPageSize - blockSize) <= pageSize);
    }

    Page *newPage = static_cast<Page*>(baseAllocator->Allocate(pageSize, 1));//newPageSize));
    newPage->blockSize = blockSize;
    newPage->freeBlocks = numBlocks;
    newPage->usedBlocks = 0;
    newPage->next = firstPage;
    firstPage = newPage;

    uint32_t *it  = (uint32_t*)(uintptr_t(newPage) + sizeof(Page)),
             *end = it + numFlags;

    for (; it < end; ++it)
        *it = 0xffffffff;

    return newPage;
}

BlocksAllocator::Page*
BlocksAllocator::RefitPage(Page *page, uint32_t blockSize)
{
    assert(0 == page->usedBlocks);
    assert(blockSize < pageSize);
    uint32_t numBlocks = pageSizeNoHeader / blockSize,
             numFlags  = (numBlocks / 32) + (numBlocks % 32 > 0 ? 1 : 0);

    uint32_t newPageSize = sizeof(Page) + numFlags * 4;
    newPageSize += (numBlocks * blockSize);
    if (newPageSize > pageSize) {
        --numBlocks;
        assert((newPageSize - blockSize) <= pageSize);
    }

    page->blockSize = blockSize;
    page->freeBlocks = numBlocks;

    uint32_t *it  = (uint32_t*)(uintptr_t(page) + sizeof(Page)),
             *end = it + numFlags;

    for (; it < end; ++it)
        *it = 0xffffffff;

    return page;
}

BlocksAllocator::Header*
BlocksAllocator::AllocateNewBlock(Page *page)
{
    assert(page->freeBlocks > 0);
    --page->freeBlocks;
    ++page->usedBlocks;

    uint32_t numBlocks = pageSizeNoHeader / page->blockSize,
             numFlags  = (numBlocks / 32) + (numBlocks % 32 > 0 ? 1 : 0);

    uint32_t *it  = (uint32_t*)(uintptr_t(page) + sizeof(Page)),
             *end = it + numFlags;

    int blockIndex = 0, offset;
    bool found = false;
    for (; it < end; ++it) {
        for (offset = 0; offset < 32; ++offset) {
            if ((1 << offset) & *it) {
                found = true;
                break;
            }
        }

        if (found) {
            blockIndex += offset;
            *it &= ~(1 << offset);
            break;
        }

        blockIndex += 32;
    }

    assert(found && blockIndex < numBlocks);

    Header *header = (Header*)(uintptr_t(end) + uintptr_t(page->blockSize) * uintptr_t(blockIndex));
    header->page = page;

    totalAllocated += page->blockSize;

    assert((uintptr_t(header) + uintptr_t(page->blockSize) - uintptr_t(page)) <= pageSize);

    return header;
}

void
BlocksAllocator::DeallocateBlock(Header *block)
{
    assert(block->page->usedBlocks > 0);
    ++block->page->freeBlocks;
    --block->page->usedBlocks;

    uint32_t numBlocks = pageSizeNoHeader / block->page->blockSize,
             numFlags  = (numBlocks / 32) + (numBlocks % 32 > 0 ? 1 : 0);

    uint32_t *it  = (uint32_t*)(uintptr_t(block->page) + sizeof(Page)),
             *end = it + numFlags;

    uint32_t blockIndex = (uintptr_t(block) - uintptr_t(end)) / block->page->blockSize;
    assert(0 == ((uintptr_t(block) - uintptr_t(end)) % block->page->blockSize));

    while (blockIndex >= 32) {
        ++it;
        blockIndex -= 32;
    }

    assert(it < end && 0 == (*it & (1 << blockIndex)));
    *it |= (1 << blockIndex);

    totalAllocated -= block->page->blockSize;
}

BlocksAllocator::Page*
BlocksAllocator::FindPage(uint32_t blockSize)
{
    Page *page = firstPage, *blankPage = nullptr;
    while (page != nullptr) {
        if (page->freeBlocks > 0 && page->blockSize == blockSize)
            return page;
        if (nullptr == blankPage && 0 == page->usedBlocks)
            blankPage = page;
        page = page->next;
    }

    if (nullptr == blankPage) {
        page = firstPage;
        while (page != nullptr) {
            if (page->freeBlocks > 0 && page->blockSize >= blockSize && page->blockSize <= blockSize * 2)
                return page;
            page = page->next;
        }
    } else {
        return this->RefitPage(blankPage, blockSize);
    }

    return nullptr;
}

BlocksAllocator::BlocksAllocator(Allocator *allocator, uint32_t _pageSize)
: baseAllocator(allocator),
  pageSize(_pageSize),
  pageSizeNoHeader(_pageSize - sizeof(Page) - 4),
  firstPage(nullptr),
  totalAllocated(0)
{ }

BlocksAllocator::~BlocksAllocator()
{
    Page *page = firstPage, *tmp;
    while (page != nullptr) {
        tmp = page;
        page = page->next;
        assert(0 == tmp->usedBlocks);
        baseAllocator->Free(tmp);
    }
}

void*
BlocksAllocator::Allocate(size_t size, size_t align)
{
    void *d = nullptr;

    size_t ts = Allocator::GetAlignedSize<Header>(size, align);

    Page *page = this->FindPage(ts);
    if (nullptr == page)
        page = this->AllocateNewPage(ts);

    Header *h = this->AllocateNewBlock(page);
    d = Allocator::GetDataFromPointer<Header>(h, align);

    Allocator::FillPadding(h, d);

    return d;
}

void
BlocksAllocator::Free(void *pointer)
{
    if (nullptr == pointer)
        return;

    Header *h = Allocator::GetPointerFromData<Header>(pointer);
    this->DeallocateBlock(h);
}

size_t
BlocksAllocator::GetAllocatedSize(void *pointer)
{
    return Allocator::GetPointerFromData<Header>(pointer)->page->blockSize;
}

size_t
BlocksAllocator::GetTotalAllocated()
{
    return totalAllocated;
}

} // namespace Framework
