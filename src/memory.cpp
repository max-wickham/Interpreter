#include <cstdint>

// block of memory, split into stack, pointer stack and heap
int32_t memory[1024];

// pointer stack start
int pointerStackStart = 0;
int pointerStackSize = 0;
// heap start, inclusive, must be initiliased at the start of program
int heapEnd = 0;

