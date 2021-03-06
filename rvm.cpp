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
#include <stdio.h>
using namespace std;

extern void rvm_truncate_log(rvm_t rvm)
{
    if(verbose_enabled!=0)
        cout << endl << "truncate" << endl;
    char* buf;
    ifstream log(rvm->directory + "/log");
    if (!log.is_open()) return;
    string name;
    int count = 0;
    while (!log.eof() && count < 100) // in case of an infinite loop
    {
        count++;
        log >> name;
        if (log.eof()) break;
        // cout << "name: " << name << endl;
        int size = 0;
        int offset = 0;
        log >> offset;
        log >> size;
        // cout << offset << " " << size << endl;
        buf = (char*) malloc(size * sizeof(char));
        log.ignore(1, EOF);
        log.read(buf, size);
        log.ignore(size * 10, '\n');
        ofstream bak(rvm->directory + "/" + name, ios::in |ios::out);
        // cout << "offset : " << offset << endl; 
        bak.seekp(offset);
        // cout << "p : " << bak.tellp() << endl;
        bak.write(buf, size);
        bak.close();
        // cout << buf << endl;
        free(buf);
    }
    log.close();
    string dir = rvm->directory + "/log";
    ofstream log_out(dir);
    log_out.close();
}

extern rvm_t rvm_init(const char *directory)
{
    if(verbose_enabled!=0)
        cout << endl << "rvm init" << endl;
	rvm_t rvm = (rvm_t) malloc(sizeof(struct rvm_info));
	rvm->directory = directory;
    rvm->map = new unordered_map<string, void*>();
    rvm->busy = new unordered_map<void*, int>();
	mkdir(directory, 0755);
	return rvm;
}

void readFromFile(char* buf, string fileName)
{
    ifstream bak(fileName);
    bak.seekg(0, bak.end);
    int size = bak.tellg();
    bak.seekg(0, bak.beg);
    bak.read(buf, size);
    bak.close();
}

extern void *rvm_map(rvm_t rvm, const char *segname, int size_to_create)
{
    if(verbose_enabled!=0)
        cout << endl << "rvm mapping" << endl;
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
            	// cout<<"Find file!"<<endl;
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
			ifstream in(path, ifstream::ate | ifstream::binary);
    		int file_length = in.tellg();
    		if(size_to_create>file_length)
            {
    			truncate(path, size_to_create);
    		}
            rvm_truncate_log(rvm);
            char* seg_mem = (char*)malloc(size_to_create * sizeof(char));
            (*(rvm->map))[segname] = seg_mem;
            string fileDir = rvm->directory + "/" + segname;
            readFromFile(seg_mem, fileDir);
            return (void*)seg_mem;
		}else{
			// try to map the same segment twice, error
			cout<<"Error, trying to map the same segment twice!"<< endl;
			return NULL;
		}
	}else{
    	// new a file and malloc a memory
		ofstream outfile(path);
		outfile.seekp(size_to_create - 1);
        void* seg_mem = malloc(size_to_create * sizeof(char));
        (*(rvm->map))[segname] = seg_mem;
        return seg_mem;
	}
}
extern void rvm_unmap(rvm_t rvm, void *segbase)
{
    if(verbose_enabled!=0)
        cout << endl << "rvm unmapping" << endl;
    int erased = 0;
    for (auto it = rvm->map->begin();it != rvm->map->end(); ++it)
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
    // check if the segment is mapped or not
    if(verbose_enabled!=0)
        cout << endl << "rvm destory" << endl;
    if(rvm->map->find(segname)!=rvm->map->end()){
        cout<<"Error: trying to destory before unmapping!"<<endl;
        return;
    }
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
                isExit = true;
                break;
            }
        } 
    }else{ 
        cout << "Error opening directory" << endl;
        return; 
    }
    if(isExit){
        char* path = new char[strlen(directory)];
        strcpy (path,directory);
        strcat (path,"/");
        strcat (path,segname);
        if( remove(path) != 0 )
            cout<< "Error deleting file" <<endl;
        else
            cout<<"File successfully deleted"<<endl;
    }
}
extern trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases)
{
    if(verbose_enabled!=0)
        cout << endl << "begin transaction" << endl;
    for (int i = 0; i < numsegs; i++)
    {
        if (rvm->busy->count(segbases + i) && (*(rvm->busy))[segbases + i])
        {
            return (trans_t) -1;
        }
    }
    for (int i = 0; i < numsegs; i++)
    {
        (*(rvm->busy))[segbases + i] = 1;
    }
    trans_t trans = (trans_t)malloc(sizeof(struct transaction_struct));
    trans->numsegs = numsegs;
    trans->rvm = rvm;
    trans->segbases = segbases;
    trans->modified_segs = new vector<void*>();
    trans->size = new vector<int>();
    trans->offset = new vector<int>();
    return trans;
}

extern void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size)
{
    if(verbose_enabled!=0)
        cout <<endl<< "about to modify" << tid->numsegs << endl;
    rvm_t rvm = tid->rvm;
    int confirm = 0;
    for (int i = 0; i < tid->numsegs; i++)
    {
        if (segbase == *(tid->segbases + i))
        {
            confirm = 1;
        }
    }
    if (!confirm) return;
    tid->modified_segs->push_back(segbase);
    tid->size->push_back(size);
    tid->offset->push_back(offset);
}

void release_trans(trans_t tid)
{
    for (int i = 0; i < tid->numsegs; i++)
    {
        (*(tid->rvm->busy))[tid->segbases + i] = 0;
    }
    delete(tid->modified_segs);
    delete(tid->offset);
    delete(tid->size);
    free(tid);
}

extern void rvm_commit_trans(trans_t tid)
{
    if(verbose_enabled!=0)
        cout << endl << "rvm commit transaction" << endl;
    rvm_t rvm = tid->rvm;
    ofstream log;
    log.open(rvm->directory + "/log");
    int n = tid->modified_segs->size();

    for (int i = 0; i < n; i++)
    {
        string name;
        for (auto it = rvm->map->begin();it != rvm->map->end(); ++it)
        {
            if (it->second == (*(tid->modified_segs))[i])
            {
                name = it->first;
                break;
            }
        }
        char* base = (char*)(*(tid->modified_segs))[i];
        log << name << " " << (*(tid->offset))[i] << " " << (*(tid->size))[i] << endl;
        log.write(base + (*(tid->offset))[i], (*(tid->size))[i]);
        log << endl;
    }
    log.close();
    release_trans(tid);

}
extern void rvm_abort_trans(trans_t tid)
{
    if(verbose_enabled!=0)
        cout << endl << "abort transaction" << endl;
    rvm_t rvm = tid->rvm;
    rvm_truncate_log(rvm);
    int n = tid->modified_segs->size();
    for (int i = 0; i < n; i++)
    {
        string name;
        for (auto it = rvm->map->begin();it != rvm->map->end(); ++it)
        {
            if (it->second == (*(tid->modified_segs))[i])
            {
                name = it->first;
                break;
            }
        }
        readFromFile((char*)(*(tid->modified_segs))[i], rvm->directory + "/" + name);
    }
    release_trans(tid);
}
extern void rvm_verbose(int enable_flag)
{
    verbose_enabled = enable_flag;
}
