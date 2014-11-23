#include<string>
#include "rvm.h"
#include<iostream>
int main()
{
    rvm_t rvm = rvm_init("bak");
    std::cout << rvm.directory << std::endl;
}
