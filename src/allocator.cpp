#include <cstddef>
#include <cstdint>
#include <unistd.h>
#include <utility>
#include <iostream>

/* Word size, 4 bytes for esp32 */
using word_t = intptr_t;

/* Allocated block of memory */
struct Block
{
    // Object header
    size_t size;
    bool used;
    Block *next;
    // Data
    word_t data[1];
};

/* Heap start */
static Block *heapStart = nullptr;

/* Top of heap */
static auto top = heapStart;

/* Align size by word size */
static inline size_t align(size_t n)
{
    return (n + sizeof(word_t) - 1) & ~(sizeof(word_t) - 1);
}

/*
 * Returns total allocation size, 
 * word_t data[1] reserves one word automatically so this needs to be removed
*/
static inline size_t allocSize(size_t size)
{
    return size + sizeof(Block) - sizeof(std::declval<Block>().data);
}

/* Request memory from OS */
static Block *requestFromOS(size_t size)
{
    // Current heap break.
    auto block = (Block *)sbrk(0); // (1)

    // OOM.
    if (sbrk(allocSize(size)) == (void *)-1)
    { // (2)
        return nullptr;
    }

    return block;
}

/* 
 * Splits the block into two, 
 * returns the pointer to the sub-block of the requested size. 
*/
static Block *split(Block *block, size_t size)
{
    size_t newSize = allocSize(size);
    Block *newBlock = block + newSize;
    newBlock->used = false;
    // subtract header size and data size from old size
    newBlock->size = block->size - size - allocSize(0);
    block->size = size;
    newBlock->next = block->next;
    block->next = newBlock;
    return block;
}

/* Check if this block can be split. */
static const size_t min_size = align(sizeof(Block) - sizeof(std::declval<Block>().data));
static inline bool canSplit(Block *block, size_t size)
{
    return block->size - allocSize(size) >= min_size;
}

/* Allocates a block from the list, splitting if needed. */
static Block *listAllocate(Block *block, size_t size)
{
    // Split the larger block, reusing the free part.
    if (canSplit(block, size))
    {
        block = split(block, size);
    }

    block->used = true;
    block->size = size;

    return block;
}

/* Previously found block. Updated in `nextFit` */
static Block *searchStart = heapStart;

/* Reset the heap to the original position. */
static void resetHeap()
{
    // Already reset.
    if (heapStart == nullptr)
    {
        return;
    }

    // Roll back to the beginning.
    brk(heapStart);

    heapStart = nullptr;
    top = nullptr;
    searchStart = nullptr;
}

/*
 * Next-fit algorithm.
 *
 * Returns the next free block which fits the size.
 * Updates the `searchStart` of success.
*/
static inline Block *nextFit(size_t size)
{
    Block *currentBlock = searchStart;
    if (!currentBlock)
    {
        return nullptr;
    }
    while (currentBlock->size < size && !currentBlock->used)
    {
        // Check if reached the end of the linked list
        if (currentBlock->next == nullptr)
        {
            currentBlock = heapStart;
        }
        // Check if returned to the start of the loop
        else if (currentBlock->next == searchStart)
        {
            return nullptr;
        }
    }
    return listAllocate(currentBlock, size);
}

/*
 * Tries to find a block that fits.
*/
static Block *findBlock(size_t size)
{
    return nextFit(size);
}

/*Returns the object header */
static Block *getHeader(word_t *data)
{
    return (Block *)((char *)data + sizeof(std::declval<Block>().data) -
                     sizeof(Block));
}

/* Allocates a block of memory of (at least) `size` bytes */
word_t *allocBlock(size_t size)
{
    size = align(size);

    // ---------------------------------------------------------
    // 1. Search for an available free block:

    if (auto block = findBlock(size))
    {
        return block->data;
    }

    // ---------------------------------------------------------
    // 2. If block not found in the free list, request from OS:

    auto block = requestFromOS(size);

    block->size = size;
    block->used = true;

    // Init heap.
    if (heapStart == nullptr)
    {
        heapStart = block;
    }

    // Chain the blocks.
    if (top != nullptr)
    {
        top->next = block;
    }

    top = block;

    // User payload:
    return block->data;
}

/* Frees a previously allocated block */
void freeBlock(word_t *data)
{
    auto block = getHeader(data);
    block->used = false;
}