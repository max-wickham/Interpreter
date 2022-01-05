// Create a new sweep id
// Iterate through each pointer in the current scope
// For each pointer mark the object with the sweep id
// Travel to each pointer that the object has a reference to 
// Iterate with this method until all reachable objects have been marked with the sweep id

#include <cstdint>
#include "allocator.cpp"

// Integer representing an ID for the current sweep
static int sweepID = 0;

// Mark an object and all child objects as alive
void mark(BlockHeader *block){
    // Prevent loops
    if(block == nullptr){
        return;
    }
    if(block->sweepID == sweepID){
        return;
    }
    block->sweepID = sweepID;
    // mark each child object
    for(int i = 0; i < block->numPointers; i++){
        // defreference the pointer to the pointer to the child object
        mark(*((BlockHeader **)(block + sizeof(BlockHeader) + i)));
    }
}

// Marks every alive object with a new ID and returns that ID
int sweep (const BlockHeader *memoryStart, const int &pointerStackStart, const int &pointerStackSize){
    sweepID += 1;
    // Iterate through every pointer currently in scope and mark them
    for(int i = 0; i < pointerStackSize; i++){
        mark((BlockHeader*)(pointerStackStart + memoryStart + i));
    }
    return sweepID;
}
