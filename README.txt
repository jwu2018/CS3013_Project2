README
CS 3013 Project 2
Jeffrey Huang and Jyalu Wu


----------------------------------------------------------------
Phase 1: antivirus.c
----------------------------------------------------------------
Functions:
	new_sys_open
	new_sys_close
	new_sys_read
	find_sys_call_table
	disable_page_protection
	enable_page_protection
	interceptor_start
	interceptor_end
In order to intercept the open, close, and read system calls, we
first stored a copy of the existing functions from the system call
table and then replaced them with our custom functions in the
system call table. The custom open system call will print out a
message to the kernel specifying the user id and the file that was
opened. The custom close system call will print out a message to
the kernel specifying the user id and the file that was closed.
The read system call will print out a message to the kernel if
the string "zoinks" was found while parsing through the file.


----------------------------------------------------------------
Phase 2: procAncestry_kernel.c
----------------------------------------------------------------
Functions:						Structs:
	new_sys_cs3013_syscall2			ancestry
	find_sys_call_table
	disable_page_protection
	enable_page_protection
	interceptor_start
	interceptor_end
In order to intercept syscall2, we followed the same format as
phase 1 but for cs3013_syscall2 instead. The custom function we
made to replace that call takes in a target pid and stores the
pids of its children, siblings, and parents (if any) in a given
ancestry struct. It will also print out a list of the children,
siblings, and parents to the kernel. At the start of the function,
we find the corresponding task_struct of the target pid, which
can allow us to access the task's children, siblings, and parents.
To get the list of children, we loop through a linked list that
is a built-in field of task_struct and store the pid of each child.
To get the list of siblings, we do the same except with the built-
in sibling linked list. To get the list of ancestors, we keep
calling the parent field of the current task (and setting that
current task to the parent once we are finished) and storing the
pid of the parent.


----------------------------------------------------------------
procAncestry.c
----------------------------------------------------------------
Functions:			Structs:
	main				ancestry
	testCall
This file is an executable that corresponds with procAncestry_kernel.c.
It gets a target pid from the user in the command line and sends
it as an argument to new_sys_cs3013_syscall2 in procAncestry_kernel.c
which then gets the children, siblings, and ancestors of the
target pid. It then prints out the children, siblings, and ancestors.
