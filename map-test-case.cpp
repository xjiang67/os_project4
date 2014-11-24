/*trying to detect:
1.destory before unmap test cases
2.map a segment already exits but with smaller size
*/
#include<string>
#include "rvm.h"
#include<iostream>
#include <fstream>
int main()
{
    rvm_t rvm = rvm_init("rvm_segments");
    char* segs[1];
    segs[0] = (char *) rvm_map(rvm, "testseg", 10000);
    rvm_unmap(rvm, segs[0]);
    segs[0] = (char *) rvm_map(rvm, "testseg", 15000);

    ifstream in("rvm_segments/testseg", ifstream::ate | ifstream::binary);
    int file_length = in.tellg();
    if(file_length!=15000){
        cout<<"FIlE LENGTH ERROR!"<<endl;
    }else{
        cout<<"FILE LENGTH OK!"<<endl;
    }

    /*report error when trying to destory segment before unmap*/

    rvm_destroy(rvm,"testseg");
    if(rvm->map->find("testseg")!=rvm->map->end()){
        cout<<"DESTORY OK!"<<endl;
    }else{
        cout<<"DESTORY ERROR!"<<endl;
    }
    rvm_unmap(rvm, segs[0]);
    if(rvm->map->find("testseg")!=rvm->map->end()){
        cout<<"UNMAP ERROR!"<<endl;
    }else{
        cout<<"UNMAP OK!"<<endl;
    }
    return 0;
}
