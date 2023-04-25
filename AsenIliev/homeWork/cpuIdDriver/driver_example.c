#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/version.h>
#include <asm/uaccess.h>



MODULE_LICENSE("Dual BSD/GPL");

// mknod cpuid_Miroslav_Genov_CT4872 c 487 0 
#define DEVICE_NAME "cpuid_Miroslav_Genov_CT4872"
#define MAJOR_NUMBER 487


static int Device_Open = 0;	// Is device open?  
int char_dev_num;

#define BUF_LEN 80		/* Max length of the message from the device */
static char msg[BUF_LEN];	/* The msg the device will give when asked */
static char *msg_Ptr;

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



/* 
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int hello_open(struct inode *inode, struct file *file)
{
	static int counter = 0;

	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	sprintf(msg, "I already told you %d times Hello world!\n", counter++);
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

struct file_operations fops = {
	.read = hello_read,
	.open = hello_open,
	.release = hello_release
};



static int __init hello_init(void){

	// strncpy(message, "Hello world",1023);
	// message_bytes =strlen(message);

	//registration of the char device
	//0 will give filled in major number
	// module name
	//file operations

	
	printk(KERN_ALERT "The device number is %d",MAJOR_NUMBER);
	register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &fops);

	
   return 0;
}

static void __exit hello_exit(void){
	unregister_chrdev(MAJOR_NUMBER,DEVICE_NAME);
	printk(KERN_ALERT "Exit world\n");
}

module_init(hello_init);
module_exit(hello_exit);
