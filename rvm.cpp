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
extern rvm_t rvm_init(const char *directory)
{
	rvm_t rvm = (rvm_t) malloc(sizeof(struct rvm_info));
	rvm->directory = directory;
    rvm->map = new unordered_map<string, void*>();
    rvm->busy = new unordered_map<void*, int>();
	mkdir(directory, 0755);
	return rvm;
}
extern void *rvm_map(rvm_t rvm, const char *segname, int size_to_create)
{
	cout<<"map size: "<<rvm->map->size()<<endl;
	const char* directory = rvm->directory.c_str();
	DIR *dir = opendir(directory);
	bool isExit = false;
	if(dir) 
    { 
        struct dirent *ent; 
        while((ent = readdir(dir)) != NULL) 
        { 
            if(strcmp(ent->d_name, segname) == 0)
            {
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
	if(isExit)
    {
		// if file in disk
		if(rvm->map->find(segname)==rvm->map->end())
        {
			// if not mapped
			// extend the file size
			cout<<"map not exits"<<endl;
			ifstream in(path, ifstream::ate | ifstream::binary);
    		int file_length = in.tellg();
    		if(size_to_create>file_length)
            {
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
	(*rvm->map)[segname] = seg_mem;
    return seg_mem;	
}
extern void rvm_unmap(rvm_t rvm, void *segbase)
{
    int erased = 0;
    for ( auto it = rvm->map->begin(); it != rvm->map->end(); ++it )
    {
        if (it->second == segbase)
        {
            rvm->map->erase(it);
            cout << rvm->map->size() << endl;
            cout << "erase!" << endl;
            erased = 1;
            break;
        }
    }
    if (!erased)
    {
        cout << "Nothing to erase!";
    }
    free(segbase);
}
extern void rvm_destroy(rvm_t rvm, const char *segname)
{
    // Remember to release map as well!

}
extern trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases)
{
    for (int i = 0; i < numsegs; i++)
    {
        if (rvm->busy->count(segbases + i) && (*rvm->busy)[segbases + i])
        {
            return (trans_t) -1;
        }
    }
    for (int i = 0; i < numsegs; i++)
    {
        (*rvm->busy)[segbases + i] = 1;
    }
    trans_t trans = (trans_t)malloc(sizeof(struct transaction_struct));
    trans->numsegs = numsegs;
    trans->rvm = rvm;
    trans->segbases = segbases;
}
extern void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size)
{

}

extern void rvm_commit_trans(trans_t tid)
{

}
extern void rvm_abort_trans(trans_t tid)
{
    for (int i = 0; i < tid->numsegs; tid++)
    {
        (*tid->rvm->busy)[tid->segbases + i] = 0;
    }
    free(tid);
}
extern void rvm_truncate_log(rvm_t rvm)
{

}
extern void rvm_verbose(int enable_flag)
{

}
