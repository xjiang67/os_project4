/* trying to detect map the same segment twice test case*/
#include<string>
#include "rvm.h"
#include<iostream>
int main()
{
    rvm_t rvm = rvm_init("rvm_segments");
    char* segs[2];
    segs[0] = (char *) rvm_map(rvm, "testseg", 10000);
    segs[1] = (char *) rvm_map(rvm, "testseg", 10000);
    if(segs[1]==NULL){
        cout<<"OK"<<endl;
    }else{
        cout<<"ERROR: DID NOT DETECT MAP TWICE CASE!"<<endl;
    }
    return 0;
}
