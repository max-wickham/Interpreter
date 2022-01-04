// Create a new sweep id
// Iterate through each pointer in the current scope
// For each pointer mark the object with the sweep id
// Travel to each pointer that the object has a reference to 
// Iterate with this method until all reachable objects have been marked with the sweep id

#include <cstdint>
#include "memory.cpp"
#include "allocator.cpp"

// Integer representing an ID for the current sweep
int sweepID = 0;

void mark(Block *block){

}

void sweep (int32_t memory[1024], int pointerStackStart, int pointerStackSize){
    for(int i = 0; i < pointerStackSize; i++){
        mark((Block*)memory[pointerStackStart + i]);
    }
}
