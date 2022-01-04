#include <cstddef>
#include <cstdint>

// Word size in bytes
const int wordSize = 4;

// Struct for storing object header
struct BlockHeader{
    // The ID of the last sweep this block was marked in
    int sweepID;
    // A pointer to the next block
    BlockHeader *next;
    // The number of bytes used by the block data
    size_t dataSize;
    // The number of bytes of block data that are pointers
    size_t pointerDataSize;
};

// The start of the heap initialised at the begining
static BlockHeader *heapStart = nullptr;
// The current end of the heap
static BlockHeader *heapTop = heapStart;
static BlockHeader *maxHeapAddress;

// Align a memory size to use a whole number of bytes
static inline size_t align(size_t dataSize){
    // TODO test
    return (dataSize + wordSize - 1) & ~(wordSize - 1);
}

// Calculate the size of the block header and block data combined
static inline size_t sizeForAlloc(size_t dataSize){
    return dataSize + sizeof(BlockHeader);
}

// Split a block into two smaller blocks,
// return a pointer to the block of the requested size
static BlockHeader *split(BlockHeader *block, size_t dataSize){
    BlockHeader *newBlock = block + sizeForAlloc(dataSize);
    newBlock->sweepID = block->sweepID;
    newBlock->next = block->next;
    block->next = newBlock;
    newBlock->dataSize = block->dataSize - sizeForAlloc(dataSize);
}

// Check if a block can be split with a given size
static inline bool canSplit(BlockHeader *block, size_t dataSize){
    return block->dataSize > dataSize + sizeof(BlockHeader);
}

// Block to start search on, initiliased in initHeap
static BlockHeader *searchStart;

// Check if there is a block that can be used,
// return it if there is and split the block if possible
static inline BlockHeader *nextFit(size_t dataSize, int sweepID){
    // Check atleast one object has been allocated
    if (heapTop == heapStart){
        return nullptr;
    }
    BlockHeader *currentBlock = searchStart;
    while(currentBlock->dataSize < dataSize 
        && !(currentBlock->sweepID == sweepID)){
            if (currentBlock->next == nullptr){
                currentBlock = heapStart;
            }
            else if (currentBlock->next == searchStart)
            {
                return nullptr;
            }
    }
    searchStart = currentBlock;
    if (canSplit(currentBlock,dataSize)){
        return split(currentBlock,dataSize);
    }
    return currentBlock;
}

BlockHeader *allocBlock(size_t dataSize, size_t pointerdataSize, 
    int sweepID){
    dataSize = align(dataSize);
    BlockHeader *block = nextFit(dataSize, sweepID);
    if (block){
        block->pointerDataSize = pointerdataSize;
        return block;
    }
    if (heapTop == maxHeapAddress){
        return nullptr;
    }
    BlockHeader *newBlock = heapTop;
    newBlock->sweepID = sweepID;
    newBlock->dataSize = dataSize;
    newBlock->pointerDataSize = pointerdataSize;
    newBlock->next = newBlock + dataSize + sizeof(BlockHeader); 
    heapTop = newBlock->next;
    return newBlock;
}

// Initilise the heap
void initHeap(BlockHeader* start, BlockHeader* end){
    heapStart = start;
    heapTop = heapStart;
    searchStart = heapStart;
    maxHeapAddress = end;
}