#include "allocator.cpp"
#include <iostream>
using namespace std;

void test1(){
    word_t *pointer = allocBlock(8);
    word_t *pointer2 = allocBlock(16);
    cout << pointer << endl;
}

int main(){
    test1();
}