// We need to define __KERNEL__ and MODULE to be in Kernel space
// If they are defined, undefined them and define them again:
#undef __KERNEL__
#undef MODULE

#define __KERNEL__ 
#define MODULE

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <asm/current.h>

unsigned long **sys_call_table;
long get_children(struct task_struct *target_task, struct ancestry *response);
// long cs3013syscall2(unsigned short *target_pid,struct ancestry *response);

struct ancestry {
	pid_t ancestors[10];
	pid_t siblings[100];
	pid_t children[100];
} ancestry;

asmlinkage long (*ref_sys_)(unsigned int fd);
// asmlinkage long (*ref_sys_read)(unsigned int fd, void* buf, size_t count);

asmlinkage long fill_ancestry_struct(unsigned short *target_pid, struct ancestry *response) {
	struct task_struct *p = current;

	// get the task_struct of the target pid
	read_lock(&tasklist_lock);
	p = find_task_by_vpid(*target_pid);
	if (p) get_task_struct(p);
	read_unlock(&tasklist_lock);

	// get the children
	get_children(p, response);
	// get the siblings
	// get_siblings(p, response);
	// get the ancestors
	// get_ancestry(p, response);

	// stack overflow says we need this line after using find_task_by_vpid
	put_task_struct(p);
  	return;
}

/*
 * Finds all of the siblings of the target process and stores them in the
 * siblings list of the corresponding ancestry struct.
 */
// asmlinkage long get_siblings(struct task_struct *target_task, struct ancestry *response) {

// }


/*
 * Finds all of the children of the target process and stores them in the
 * children list of the corresponding ancestry struct.
 */
 long get_children(struct task_struct *target_task, struct ancestry *response) {
 	pid_t childAncestors[10];
 	pid_t childSiblings[100];
 	pid_t childChildren[100];

 	for (int i = 0; i < 10; i++) {
 		childAncestors[i] = -1;
 		childSiblings[i] = -1;
 		childChildren[i] = -1;
 	}
 	for (int i = 10; i < 100; i++) {
 		childSiblings[i] = -1;
 		childChildren[i] = -1;
 	}
 	struct ancestry childAncestry = { childAncestors, childSiblings, childChildren };

	// get the youngest child
	struct task_struct *yChild = target_task->p_cptr;
	// get the pid of youngest child, store in response's children list
	(response->children)[0] = *yChild.pid;
	// get the youngest child's siblings
	// for each sibling, store pid in response's children list
	return 0;
}



/*
 * Finds all of the ancestors of the target process and stores them in the
 * ancestors list of the corresponding ancestry struct.
//  */
// asmlinkage long get_ancestry(struct task_struct *target_task, struct ancestry *response) {

// }


static unsigned long **find_sys_call_table(void) {
  unsigned long int offset = PAGE_OFFSET;
  unsigned long **sct;
  
  while (offset < ULLONG_MAX) {
    sct = (unsigned long **)offset;

    if (sct[__NR_close] == (unsigned long *) sys_close) {
      printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX\n",
       (unsigned long) sct);
      return sct;
    }
    
    offset += sizeof(void *);
  }
  
  return NULL;
}

static void disable_page_protection(void) {
  /*
    Control Register 0 (cr0) governs how the CPU operates.
    Bit #16, if set, prevents the CPU from writing to memory marked as
    read only. Well, our system call table meets that description.
    But, we can simply turn off this bit in cr0 to allow us to make
    changes. We read in the current value of the register (32 or 64
    bits wide), and AND that with a value where all bits are 0 except
    the 16th bit (using a negation operation), causing the write_cr0
    value to have the 16th bit cleared (with all other bits staying
    the same. We will thus be able to write to the protected memory.
    It's good to be the kernel!
  */
  write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
  /*
   See the above description for cr0. Here, we use an OR to set the 
   16th bit to re-enable write protection on the CPU.
  */
  write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
  /* Find the system call table */
  if(!(sys_call_table = find_sys_call_table())) {
    /* Well, that didn't work. 
       Cancel the module loading step. */
    return -1;
  }
  
  /* Store a copy of all the existing functions */
  // ref_sys_open = (void *)sys_call_table[__NR_open];
  // ref_sys_close = (void *)sys_call_table[__NR_close];
  // ref_sys_read = (void *)sys_call_table[__NR_read];

  /* Replace the existing system calls */
  disable_page_protection();

  // sys_call_table[__NR_open] = (unsigned long *)new_sys_open;
  // sys_call_table[__NR_close] = (unsigned long *)new_sys_close;
  // sys_call_table[__NR_read] = (unsigned long *)new_sys_read;
  
  enable_page_protection();
  
  /* And indicate the load was successful */
  printk(KERN_INFO "Loaded interceptor!");

  return 0;
}

static void __exit interceptor_end(void) {
  /* If we don't know what the syscall table is, don't bother. */
  if(!sys_call_table)
    return;
  
  /* Revert all system calls to what they were before we began. */
  disable_page_protection();
  // sys_call_table[__NR_open] = (unsigned long *)ref_sys_open;
  // sys_call_table[__NR_close] = (unsigned long *)ref_sys_close;
  // sys_call_table[__NR_read] = (unsigned long *)ref_sys_read;
  enable_page_protection();

  printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);