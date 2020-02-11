#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

long testCall1 ( void) {return (long) syscall(__NR_open, "zoinks.txt", 0);}

long testCall2 ( void) {return (long) syscall(__NR_close);}

long testCall3 ( void) {return (long) syscall(__NR_read);}

long testCall4 (void){
	// unsigned short pid = getpid();
	unsigned short pid = 1;
	ancestry* fam = (ancestry*)malloc(sizeof(ancestry));
	printf("getting ancestry of pid: %d\n", pid);
	long syscall_ret = (long) syscall(__NR_sys_cs3013_syscall2, &pid, fam);
	int i = 0;
	unsigned short sib_pid, chid_pid;
	for(;i < 100; i++){
		if((sib_pid = fam->siblings[i]) == 0 && (chid_pid = fam->children[i])==0)break;
		else printf("sib_pid: %hu, chid_pid: %hu\n", sib_pid, chid_pid);
	}
	return syscall_ret;
}

int main () {
	printf("The return values of the system calls are:\n");
	printf("\t__NR_open: %ld\n", testCall1());
	printf("\t__NR_read: %ld\n", testCall3());
	printf("\t__NR_close: %ld\n", testCall2());
	printf("\t__NR_syscall2: %ld\n", testCall4());
	return 0;
}