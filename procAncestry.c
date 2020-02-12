#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
// These values MUST match the unistd_32.h modifications:

#define __NR_open 5
#define __NR_close 6
#define __NR_read 3
#define __NR_sys_cs3013_syscall2 378

typedef struct ancestry {
	pid_t ancestors[10];
	pid_t siblings[100];
	pid_t children[100];
} ancestry;

long testCall(unsigned short pid){
	// unsigned short pid = getpid();
	// unsigned short pid = 1;
	ancestry* fam = (ancestry*)malloc(sizeof(ancestry));
	printf("getting ancestry of pid: %d\n", pid);
	long syscall_ret = (long) syscall(__NR_sys_cs3013_syscall2, &pid, fam);
	int i = 0;
	unsigned short sib_pid, chid_pid, anc_pid;
	for(;i < 100; i++){
		if((sib_pid = fam->siblings[i]) != 0) printf("Sibling PID: %hu\n", sib_pid);
	}
  for(i = 0;i < 100; i++){
    if((chid_pid= fam->children[i]) != 0) printf("Child PID: %hu\n", chid_pid);
  }
  for(i = 0;i < 10; i++){
    if((anc_pid = fam->ancestors[i]) != 0)printf("Ancestor PID: %hu\n", anc_pid);
  }
	return syscall_ret;
}

int main(int argc, char *argv[]) {
  if(argc!=2){
    printf("Please enter a single PID\n");
    exit(-1);
  }
	printf("The return values of the system calls are:\n");
	printf("\t__NR_syscall2: %ld\n", testCall(atoi(argv[1])));
	return 0;
}