#include <iostream>
using namespace std;

void test(){
    static int i = 0;
    cout << i << endl;
    i ++;
}

int main(){
    test();
    if (true){
        test();
    }
    test();
}