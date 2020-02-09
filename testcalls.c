#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
// These values MUST match the unistd_32.h modifications:

#define __NR_open 5
#define __NR_close 6
#define __NR_read 3

long testCall1 ( void) {return (long) syscall(__NR_open);}

long testCall2 ( void) {return (long) syscall(__NR_close);}

long testCall3 ( void) {return (long) syscall(__NR_read);}

int main () {
	printf("The return values of the system calls are:\n");
	printf("\t__NR_open: %ld\n", testCall1());
	printf("\t__NR_read: %ld\n", testCall3());
	printf("\t__NR_close: %ld\n", testCall2());
	return 0;
}