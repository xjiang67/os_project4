/* basic.c - test that basic persistency works */

#include "rvm.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define TEST_STRING1 "hello, world"
#define TEST_STRING2 "bleg!"
#define OFFSET2 1000


/* proc1 writes some data, commits it, then exits */
void proc1() 
{
    rvm_t rvm;
    char *seg;
    void *segs[1];
    trans_t trans;
    
    rvm = rvm_init("rvm_segments");
    
    rvm_destroy(rvm, "testseg");
    
    segs[0] = (char *) rvm_map(rvm, "testseg", 10000);
    seg = (char *) segs[0];
    /* write some data and commit it */
    trans = rvm_begin_trans(rvm, 1, segs);
    rvm_about_to_modify(trans, seg, 0, 100);
    sprintf(seg, TEST_STRING1);
    
    rvm_about_to_modify(trans, seg, OFFSET2, 100);
    sprintf(seg+OFFSET2, TEST_STRING1);
    
    rvm_commit_trans(trans);
    
    /* start writing some different data, but abort */
    trans = rvm_begin_trans(rvm, 1, segs);
    rvm_about_to_modify(trans, seg, 0, 100);
    sprintf(seg, TEST_STRING2);

     abort();
}


/* proc2 opens the segments and reads from them */
void proc2() 
{
     char* segs[1];
     rvm_t rvm;
     
     rvm = rvm_init("rvm_segments");

     segs[0] = (char *) rvm_map(rvm, "testseg", 10000);
     if(strcmp(segs[0], TEST_STRING1)) {
	  printf("ERROR: first hello not present\n");
      printf("%s", segs[0]);
	  exit(2);
     }
     if(strcmp(segs[0]+OFFSET2, TEST_STRING1)) {
	  printf("ERROR: second hello not present\n");
	  exit(2);
     }

     printf("OK\n");
     exit(0);
}


int main(int argc, char **argv)
{
     int pid;

     pid = fork();
     if(pid < 0) {
	  perror("fork");
	  exit(2);
     }
     if(pid == 0) {
	  proc1();
	  exit(0);
     }

     waitpid(pid, NULL, 0);

     proc2();

     return 0;
}
