#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/version.h>
#include <asm/uaccess.h>

#include <linux/proc_fs.h>


MODULE_LICENSE("Dual BSD/GPL");

int	proc_open(struct inode *, struct file *){
    printk(KERN_ALERT"oppen\n");
    return 0;
}

int len=1;

ssize_t	proc_read(struct file *File, char __user * buffer, size_t length, loff_t * offs){
    printk(KERN_ALERT"read\n");
    char *msg_Ptr = "AuthenticAmd\n"; 

    if(len){
        len=0;
    }else{
        len=1;
        return 0;
    }

    int remaining = copy_to_user(buffer,msg_Ptr,strlen(msg_Ptr));

	return length;

}

ssize_t	proc_write(struct file *, const char __user *, size_t, loff_t *){
    printk(KERN_ALERT"write\n");

    return 0;
}

int	proc_release(struct inode *, struct file *){
    printk(KERN_ALERT"releas\n");

    return 0;
}


struct proc_ops proc_ops = {
    .proc_open = proc_open,
    .proc_read = proc_read,
    .proc_write = proc_write,
    .proc_release = proc_release
};

struct proc_dir_entry* proc_dir;
static int __init hello_init(void){
    proc_dir = proc_create("proc_example",0666,NULL,&proc_ops);
    printk(KERN_ALERT"Hello proc\n");
	
   return 0;
}

static void __exit hello_exit(void){
    proc_remove(proc_dir);
	printk(KERN_ALERT "Exit world\n");
}

module_init(hello_init);
module_exit(hello_exit);