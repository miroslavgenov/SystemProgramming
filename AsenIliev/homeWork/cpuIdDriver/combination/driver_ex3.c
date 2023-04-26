#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>
#include <linux/proc_fs.h>

#include "driver_ex3.h"

MODULE_LICENSE("Dual BSD/GPL");

static int Device_Open = 0;	// Is device open?  

#define BUF_LEN 80		/* Max length of the message from the device */
static char msg[BUF_LEN];	/* The msg the device will give when asked */
static char *msg_Ptr = " ";

/* 
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 * after executing cat /dev/mycharfile
 */
static ssize_t hello_read(struct file *filp,	/* see include/linux/fs.h   */
			   char *buffer,	/* buffer to fill with data */
			   size_t length,	/* length of the buffer     */
			   loff_t * offset)
{
	/*
	 * Number of bytes actually written to the buffer 
	 */
	int bytes_read = 0;

	/*
	 * If we're at the end of the message, 
	 * return 0 signifying end of file 
	 */
	if (*msg_Ptr == 0)
		return 0;

	/* 
	 * Actually put the data into the buffer 
	 */
	while (length && *msg_Ptr) {

		/* 
		 * The buffer is in the user data segment, not the kernel 
		 * segment so "*" assignment won't work.  We have to use 
		 * put_user which copies data from the kernel data segment to
		 * the user data segment. 
		 */
		put_user(*(msg_Ptr++), buffer++);

		length--;
		bytes_read++;
	}

	/* 
	 * Most read functions return the number of bytes put into the buffer
	 */
	return bytes_read;
}


ssize_t hello_write(struct file *filep, const char __user *buffer, size_t len, loff_t * offset){
	printk(KERN_ALERT"writing to file\n");

	copy_from_user(msg,buffer,len);
	
	return len;
}

/* 
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int hello_open(struct inode *inode, struct file *file)
{

	if (Device_Open){
		return -EBUSY;
	}
	Device_Open++;

	msg_Ptr = msg;
	try_module_get(THIS_MODULE);

	return 0;
}

 static int hello_release(struct inode *inode, struct file *file)
{
	Device_Open--;		/* We're now ready for our next caller */

	/* 
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module. 
	 */
	module_put(THIS_MODULE);

	return 0;
}

/* 
 *  * This function is called whenever a process tries to do an ioctl on our
 *  * device file. We get two extra parameters (additional to the inode and file
 *  * structures, which all device functions get): the number of the ioctl called
 *  * and the parameter given to the ioctl function.
 *  *
 *  * If the ioctl is write or read/write (meaning output is returned to the
 *  * calling process), the ioctl call returns the output of this function.
 *  *
 *  */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
 static int device_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num,unsigned long ioctl_param)
#else
  static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
#endif

{
   printk(KERN_ALERT"Hello from ioctl\n");
   int i;
	char *temp;
	char ch;

	/* 
	 * Switch according to the ioctl called 
	 */
	switch (ioctl_num) {
	case IOCTL_SET_MSG:
		/* 
		 * Receive a pointer to a message (in user space) and set that
		 * to be the device's message.  Get the parameter given to 
		 * ioctl by the process. 
		 */
		temp = (char *)ioctl_param;

		/* 
		 * Find the length of the message 
		 */
		get_user(ch, temp);
		for (i = 0; ch && i < BUF_LEN; i++, temp++)
			get_user(ch, temp);

		hello_write(file, (char *)ioctl_param, i, 0);
		break;

	case IOCTL_GET_MSG:
		/* 
		 * Give the current message to the calling process - 
		 * the parameter we got is a pointer, fill it. 
		 */
		i = hello_read(file, (char *)ioctl_param, 99, 0);

		/* 
		 * Put a zero at the end of the buffer, so it will be 
		 * properly terminated 
		 */
		put_user('\0', (char *)ioctl_param + i);
		break;

	case IOCTL_GET_NTH_BYTE:
		/* 
		 * This ioctl is both input (ioctl_param) and 
		 * output (the return value of this function) 
		 */
		return msg[ioctl_param];
		break;
	}

	return 0;
   
   return 0;
}
struct file_operations fops = {
	.read = hello_read,
	.write = hello_write,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
       .ioctl = device_ioctl,
#else
       .unlocked_ioctl = device_ioctl,
#endif	
	.open = hello_open,
	.release = hello_release
};


///PROC

int	proc_open(struct inode *, struct file *){
    printk(KERN_ALERT"oppen\n");
    return 0;
}

int len=1;

ssize_t	proc_read(struct file *File, char __user * buffer, size_t length, loff_t * offs){
    printk(KERN_ALERT"read\n");
    
	// char *msg_Ptr = "Hello From Proc\n"; 
	// if(msg_Ptr[0]='\0'){
	// 	msg_Ptr=" ";
	// }

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

	
// PROCK END
struct proc_dir_entry* proc_dir;

static int __init hello_init(void){
	// File *fptr;
	// strncpy(message, "Hello world",1023);
	// message_bytes =strlen(message);

	//registration of the char device
	//0 will give filled in major number
	// module name
	//file operations

    proc_dir = proc_create(PROC_NAME,0666,NULL,&proc_ops);
    printk(KERN_ALERT"Hello proc\n");

	
	printk(KERN_ALERT "The device number is %d",MAJOR_NUMBER);
	register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &fops);

	
   return 0;
}

static void __exit hello_exit(void){
    proc_remove(proc_dir);	
	unregister_chrdev(MAJOR_NUMBER,DEVICE_NAME);

	printk(KERN_ALERT "Exit world\n");
}

module_init(hello_init);
module_exit(hello_exit);
