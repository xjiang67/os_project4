#include<string>
#include "rvm.h"
#include<iostream>
int main()
{
    rvm_t rvm = rvm_init("bak");
    std::cout << rvm.directory << std::endl;
    char* segs[0];
    segs[0] = (char *) rvm_map(rvm, "testseg", 10000);
    segs[0] = (char *) rvm_map(rvm, "testseg", 10000);
    return 0;
}
