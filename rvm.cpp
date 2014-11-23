#include "rvm.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/stat.h>
extern rvm_t rvm_init(const char *directory){
	rvm_t rvms;
	rvm_t *rvm = &rvms;
	rvm->directory = directory;
	mkdir(directory, 0755);
	return *rvm;
}
extern void *rvm_map(rvm_t rvm, const char *segname, int size_to_create){

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
