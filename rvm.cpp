#include "rvm.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <unistd.h> 
#include <cstring>
using namespace std;
extern rvm_t rvm_init(const char *directory){
	rvm_t rvms;
	rvm_t *rvm = &rvms;
	rvm->directory = directory;
	mkdir(directory, 0755);
	return *rvm;
}
extern void *rvm_map(rvm_t &rvm, const char *segname, int size_to_create){
	cout<<"map size: "<<rvm.map.count(segname)<<endl;
	const char* directory = rvm.directory.c_str();
	DIR *dir = opendir(directory);
	bool isExit = false;
	if(dir) 
    { 
        struct dirent *ent; 
        while((ent = readdir(dir)) != NULL) 
        { 
            if(strcmp(ent->d_name, segname) == 0){
            	cout<<"Find file!"<<endl;
            	isExit = true;
            	break;
            }
        } 
    }else{ 
        cout << "Error opening directory" << endl;
        return NULL; 
    }

    closedir(dir);
    // cout<<"isExit:" <<isExit<<endl;
    char* path = new char[strlen(directory)];
    strcpy (path,directory);
    strcat (path,"/");
    strcat (path,segname);
    // cout<<"path:"<<path<<endl;
	if(isExit){
		// if file in disk
		if(rvm.map.find(segname)==rvm.map.end()){
			// if not mapped
			// extend the file size
			cout<<"map not exits"<<endl;
			ifstream in(path, ifstream::ate | ifstream::binary);
    		int file_length = in.tellg();
    		if(size_to_create>file_length){
    			truncate(path, size_to_create);
    		}
		}else{
			// try to map the same segment twice, error
			cout<<"Error, trying to map the same segment twice!"<< endl;
			return NULL;
		}
	}else{
    	// new a file and malloc a memory
		ofstream outfile(path);
		outfile.seekp(size_to_create - 1);
	}
	void* seg_mem = malloc(size_to_create);
	rvm.map[segname] = seg_mem;
    return seg_mem;	
}
extern void rvm_unmap(rvm_t rvm, void *segbase){

}
extern void rvm_destroy(rvm_t rvm, const char *segname){

}
extern trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases){
	

}
extern void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size){

}
extern void rvm_commit_trans(trans_t tid){

}
extern void rvm_abort_trans(trans_t tid){

}
extern void rvm_truncate_log(rvm_t rvm){

}
extern void rvm_verbose(int enable_flag){

}
