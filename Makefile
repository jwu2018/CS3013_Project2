obj-m := antivirus.o procAncestry_kernel.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
CC=gcc

all: testcalls
	$(MAKE) -C $(KDIR) M=$(PWD) modules

testcalls: testcalls.o
	$(CC) -o $@ $^

testcalls.o: testcalls.c
	$(CC) -c $^

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm testcalls.o testcalls