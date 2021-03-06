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

typedef struct ancestry {
  pid_t ancestors[10];
  pid_t siblings[100];
  pid_t children[100];
} ancestry;

asmlinkage long (*ref_sys_cs3013_syscall2)(void);

/*
 * Replaces the system call cs3013_syscall2 and prints out target process'
 * children, siblings, and ancestors.
 * @param target_pid, the process id of the target process
 * @param response, the ancestry struct to fill out
 */
asmlinkage long new_sys_cs3013_syscall2(unsigned short *target_pid, ancestry *response) {
	// printk(KERN_INFO "insertion for syscall2 worked");

	struct task_struct* task_iterator;
	struct task_struct *parent_task;  
	int i = 0;
	unsigned short sib_pid;
	unsigned short cldn_pid;
	struct task_struct* p;
	unsigned short pid_cpy;
	ancestry out_val;

	if(copy_from_user(&pid_cpy, target_pid, sizeof(short))) return EFAULT;
	if(copy_from_user(&out_val, response, sizeof(ancestry))) return EFAULT;

	printk(KERN_INFO "tracing pid: %d\n", *target_pid);

	// get the task_struct of the target pid
	p = pid_task(find_vpid(*target_pid),PIDTYPE_PID);
	// if (p) get_task_struct(p);

	
	//iterate through children
	list_for_each_entry(task_iterator, &(p->children), sibling){
		cldn_pid = task_iterator->pid;
		printk(KERN_INFO "children pid: %hu\n",cldn_pid);
		out_val.children[i] = cldn_pid;
		i++;
	}
	
	// printk("og parent pid pid: %hu\n", task->parent->pid);
	printk(KERN_INFO "finding ancestors for current pid: %hu\n", p->pid);
	if (p->pid != 1) {
		printk(KERN_INFO "current task isn't init\n");
		i = 0;
		//iterate through siblings
		list_for_each_entry(task_iterator, &(p->sibling), sibling){
			sib_pid = task_iterator->pid;
			printk(KERN_INFO "sibling pid: %hu\n",sib_pid);
			out_val.siblings[i] = sib_pid;
			i++;
		}

		printk(KERN_INFO "finished siblings, onto parents\n");
		//iterate through ancestors
		parent_task = p->parent;
		i = 0;
		printk(KERN_INFO "this parent's pid is %hu\n", parent_task->pid);
		if (parent_task->pid == 1) {
			out_val.ancestors[i] = parent_task->pid;
			printk("parent pid: %hu\n", out_val.ancestors[i]);
      i++;
		}
	
		// printk("og parent pid: %hu\n", parent->pid);

		while (parent_task-> pid != 1) {
			out_val.ancestors[i] = parent_task->pid;
			printk("parent pid: %hu\n", out_val.ancestors[i]);
			parent_task = parent_task->parent;
			i++;
			if (parent_task->pid == 1) {
				out_val.ancestors[i] = parent_task->pid;
				printk("parent pid: %hu\n", out_val.ancestors[i]);
				break;
			}
		}
	}
	else
		printk(KERN_INFO "no ancestors, the current task is the init task\n");
	// do {
	// 	response->children[i] = parent->pid;
	// 	// printk(KERN_INFO "parent pid: %d", response->children[i]);
	// 	// i++;
	// } while (parent->parent != &init_task);

  	// return 1;
// }

  if(copy_to_user(response, &out_val, sizeof(ancestry))) return EFAULT;

  return 0;
}


/*
 * Finds the sys call table.
 * @return the address of the sys call table
 */
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

/*
 * Intercepts the system call cs3013_syscall2 and replaces
 * it with a custom function that prints out the target process'
 * children, siblings, and ancestors.
 */
static int __init interceptor_start(void) {
  /* Find the system call table */
  if(!(sys_call_table = find_sys_call_table())) {
    /* Well, that didn't work. 
       Cancel the module loading step. */
    return -1;
  }
  
  /* Store a copy of all the existing functions */
  ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];

  /* Replace the existing system calls */
  disable_page_protection();

  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;
  
  enable_page_protection();
  
  /* And indicate the load was successful */
  printk(KERN_INFO "Loaded interceptor!\n");

  return 0;
}


/*
 * Ends the interceptor by replacing the system call with its
 * correct pointer.
 */
static void __exit interceptor_end(void) {
  /* If we don't know what the syscall table is, don't bother. */
  if(!sys_call_table)
    return;
  
  /* Revert all system calls to what they were before we began. */
  disable_page_protection();
  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
  enable_page_protection();

  printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);