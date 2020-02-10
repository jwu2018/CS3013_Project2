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
	unsigned short pid = getpid();
	ancestry* fam = (ancestry*)malloc(sizeof(ancestry));
	printf("getting ancestry of pid: %d\n", pid);

	return (long) syscall(__NR_sys_cs3013_syscall2, &pid, fam);
}

int main () {
	printf("The return values of the system calls are:\n");
	printf("\t__NR_open: %ld\n", testCall1());
	printf("\t__NR_read: %ld\n", testCall3());
	printf("\t__NR_close: %ld\n", testCall2());
	printf("\t__NR_syscall2: %ld\n", testCall4());
	return 0;
}