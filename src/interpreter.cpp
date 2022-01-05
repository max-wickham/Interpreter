#include <cstdint>

// block of memory, split into code, stack, pointer stack and heap
static int32_t memory[1024];

// stack start index, must be initiliased at the start of program
static int stackStartIndex;
// pointer stack start index, must be initiliased at the start of program
static int pointerStackStartIndex;
static int pointerStackSize = 0;
// heap start index, inclusive, must be initiliased at the start of program
static int heapStartIndex;


inline void initMem(){
    // TODO initialise memory, loading the code and start indexes 
}

inline void interpretCode(){
    // init program Counter to start of memory
    int32_t *programCounter = memory;
    int32_t word = *programCounter;
    int32_t opcode = word & 4278190080;
    switch (opcode)
    {
    case 0: // Break
        return;
    case 1: //     
    default:
        break;
    }

}


int main(){
    initMem();
    interpretCode();
}