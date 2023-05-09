/*
*	Resources:
*	https://tldp.org/LDP/lkmpg/2.6/html/index.html
*	https://lwn.net/Kernel/LDD3/
*	https://www.kernel.org/doc/Documentation/ioctl/ioctl-number.txt
*/



/*
	contains a great many definitions of symbols and functions needed by load-
	able modules.

	module_init(function_name)
	module_exit(function_name)
*/
#include <linux/module.h>

/*
	for initialization and cleanup functions
	__init function_name
	__exit function_name

*/

#include <linux/init.h>

#include <linux/kernel.h>

#include <linux/fs.h>

#include <linux/version.h>

#include <asm/uaccess.h>

#include <linux/ioctl.h>

#include <linux/proc_fs.h>

#include <linux/types.h>

#include "test_char.h"




static int Device_Open = 0;	// Is device open?  

#define EXIT_SUCCESS 0
#define BUF_LEN 80		/* Max length of the message from the device */
static char msg[BUF_LEN];	/* The msg the device will give when asked */

// default stoinost na msg_Ptr v slychai che ne e izpulnen izpulnimiqt fail
// koito zapisva cpu manufacturer id v char driver  
static char *msg_Ptr = " ";


/*
struct file;
represents an open file. It is created by
the kernel on open and is passed to any function that operates on the file, until
the last close. After all instances of the file are closed, the kernel releases the data
structure.


loff_t f_pos;
The current reading or writing position. loff_t is a 64-bit value on all platforms
(long long in gcc terminology). The driver can read this value if it needs to know
the current position in the file but should not normally change it; read and write
should update a position using the pointer they receive as the last argument
instead of acting on filp->f_pos directly. The one exception to this rule is in the
llseek method, the purpose of which is to change the file position.


dev_t i_rdev;
For inodes that represent device files, this field contains the actual device number.

struct cdev *i_cdev;
struct cdev is the kernel’s internal structure that represents char devices; this
field contains a pointer to that structure when the inode refers to a char device
file.

*/

/* 
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 * after executing cat /dev/mycharfile


For both methods, filp is the file pointer
count is the size of the requested data transfer.

The buff argument points to the user buffer holding the data to be written or
the empty buffer where the newly read data should be placed.

Finally, offp is a pointer
to a “long offset type” object that indicates the file position the user is accessing. The
return value is a “signed size type



 */


/*

The read Method

The return value for read is interpreted by the calling application program:

• If the value equals the count argument passed to the read system call, the
requested number of bytes has been transferred. This is the optimal case.

• If the value is positive, but smaller than count, only part of the data has been
transferred. This may happen for a number of reasons, depending on the device.
Most often, the application program retries the read. For instance, if you read
using the fread function, the library function reissues the system call until com-
pletion of the requested data transfer.

• If the value is 0, end-of-file was reached (and no data was read).

• A negative value means there was an error. The value specifies what the error
was, according to <linux/errno.h>. Typical values returned on error include -EINTR
(interrupted system call) or -EFAULT (bad address).

What is missing from the preceding list is the case of “there is no data, but it may
arrive later.” In this case, the read system call should block. We’ll deal with blocking
input in Chapter 6.

The scull code takes advantage of these rules. In particular, it takes advantage of the
partial-read rule. Each invocation of scull_read deals only with a single data quan-
tum, without implementing a loop to gather all the data; this makes the code shorter
and easier to read.

If the reading program really wants more data, it reiterates the
call. If the standard I/O library (i.e., fread) is used to read the device, the application
won’t even notice the quantization of the data transfer.

If the current read position is greater than the device size, the read method of scull
returns 0 to signal that there’s no data available (in other words, we’re at end-of-file).

This situation can happen if process A is reading the device while process B opens it
for writing, thus truncating the device to a length of 0. Process A suddenly finds itself
past end-of-file, and the next read call returns 0.

*/


static ssize_t hello_read(struct file *filp,	/* see include/linux/fs.h   */
			   char *buffer,	/* buffer to fill with data */
			   size_t count,	/* count of the buffer     */
			   loff_t * offset)
{
	/*
	 * Number of bytes actually written to the buffer 
	 */
	int bytes_read = 0;

	/*
	 * If we're at the end of the message, 
	 * return EXIT_SUCCESS signifying end of file 
	 */
	if (*msg_Ptr == 0)
		return EXIT_SUCCESS;

	/* 
	 * Actually put the data into the buffer 
	 */
	while (count && *msg_Ptr) {

		/* 
		 * The buffer is in the user data segment, not the kernel 
		 * segment so "*" assignment won't work.  We have to use 
		 * put_user which copies data from the kernel data segment to
		 * the user data segment. 
		 */


		
	/*
		Although these functions behave like normal memcpy functions, a little extra care
		must be used when accessing user space from kernel code.

		The role of the two functions is not limited to copying data to and from user-space:
they also check whether the user space pointer is valid. If the pointer is invalid, no copy
is performed; if an invalid address is encountered during the copy, on the other hand,
only part of the data is copied. In both cases, the return value is the amount of mem-
ory still to be copied. The scull code looks for this error return, and returns -EFAULT to
the user if it’s not 0.

The topic of user-space access and invalid user space pointers is somewhat advanced
and is discussed in Chapter 6.

	Whatever the amount of data the methods transfer, they should generally update the
file position at *offp to represent the current file position after successful comple-
tion of the system call.

see
Figure 3-2. The arguments to read


Both the read and write methods return a negative value if an error occurs. A return
value greater than or equal to 0, instead, tells the calling program how many bytes
have been successfully transferred. If some data is transferred correctly and then an
error happens, the return value must be the count of bytes successfully transferred
	*/

		put_user(*(msg_Ptr++), buffer++);

		count--;
		bytes_read++;
	}

	/* 
	 * Most read functions return the number of bytes put into the buffer
	 */
	return bytes_read;
}



/*
The write Method
write, like read, can transfer less data than was requested, according to the following
rules for the return value:
• If the value equals count, the requested number of bytes has been transferred.

• If the value is positive, but smaller than count, only part of the data has been
transferred. The program will most likely retry writing the rest of the data.

• If the value is 0, nothing was written. This result is not an error, and there is no
reason to return an error code. Once again, the standard library retries the call to
write. We’ll examine the exact meaning of this case in Chapter 6, where block-
ing write is introduced.

• A negative value means an error occurred; as for read, valid error values are
those defined in <linux/errno.h>.

Unfortunately, there may still be misbehaving programs that issue an error message
and abort when a partial transfer is performed. This happens because some program-
mers are accustomed to seeing write calls that either fail or succeed completely,
which is actually what happens most of the time and should be supported by devices
as well. This limitation in the scull implementation could be fixed, but we didn’t
want to complicate the code more than necessary.
*/

ssize_t hello_write(struct file *filp, const char __user *buffer, size_t len, loff_t * offset){
	printk(KERN_ALERT"writing to file\n");

	copy_from_user(msg,buffer,len);
	
	return len;
}

/* 
Called when a process tries to open the device file, like
"cat /dev/mycharfile"


The open Method
The open method is provided for a driver to do any initialization in preparation for
later operations. In most drivers, open should perform the following tasks:
• Check for device-specific errors (such as device-not-ready or similar hardware
problems)
• Initialize the device if it is being opened for the first time
• Update the f_op pointer, if necessary
• Allocate and fill any data structure to be put in filp->private_data 


The only real operation performed on the device is truncating it to a length of 0 when
the device is opened for writing. This is performed because, by design, overwriting a
scull device with a shorter file results in a shorter device data area. This is similar to
the way opening a regular file for writing truncates it to zero length. The operation
does nothing if the device is opened for reading

 */
static int hello_open(struct inode *inode, struct file *filp)
{

	if (Device_Open){
		return -EBUSY;
	}
	Device_Open++;

	//TODO: create msg_Ptr_Proc and get the msg so them read the char dev the msg stays in the proc file 
	msg_Ptr = msg;
	try_module_get(THIS_MODULE);

	return EXIT_SUCCESS;
}


/*
The release Method
The role of the release method is the reverse of open. Sometimes you’ll find that the
method implementation is called device_close instead of device_release. Either
way, the device method should perform the following tasks:
• Deallocate anything that open allocated in filp->private_data
• Shut down the device on last close

*/

 static int hello_release(struct inode *inode, struct file *filp)
{
	Device_Open--;		/* We're now ready for our next caller */

	/* 
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module. 
	 */
	module_put(THIS_MODULE);

	return EXIT_SUCCESS;
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
/* 
LINUX_VERSION_CODE This macro expands to the binary representation of the kernel version, one byte
for each part of the version release number.
With this information, you can (almost) easily deter-
mine what version of the kernel you are dealing with.


KERNEL_VERSION(major,minor,release)
This is the macro used to build an integer version code from the individual num-
bers that build up a version number.
This macro is very useful when you need to compare the
current version and a known checkpoint.


*/ 



/*

The /proc filesystem is a special, software-created filesystem that is used by the ker-
nel to export information to the world.
They can be written to as well as read from.
Most of the time, however, /proc entries are read-
only files. 




*/


/*
The inode and filp pointers are the values corresponding to the file descriptor fd
passed on by the application and are the same parameters passed to the open
method. The cmd argument is passed from the user unchanged, and the optional arg
argument is passed in the form of an unsigned long, regardless of whether it was
given by the user as an integer or a pointer. If the invoking program doesn’t pass a
third argument, the arg value received by the driver operation is undefined.
*/

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
 static int device_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num,unsigned long ioctl_param)
#else
  static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
#endif

{
   printk(KERN_ALERT"Hello from ioctl\n");

	/*
put_user(datum, ptr)
These macros write the datum to user space; they are relatively fast and should be
called instead of copy_to_user whenever single values are being transferred. The
macros have been written to allow the passing of any type of pointer to put_user,
as long as it is a user-space address. The size of the data transfer depends on the
type of the ptr argument and is determined at compile time using the sizeof and
typeof compiler builtins. As a result, if ptr is a char pointer, one byte is trans-
ferred, and so on for two, four, and possibly eight bytes.

		put_user checks to ensure that the process is able to write to the given memory
address. It returns 0 on success, and -EFAULT on error.


get_user(local, ptr)
These macros are used to retrieve a single datum from user space. They behave
like put_user and __put_user, but transfer data in the opposite direction. The
value retrieved is stored in the local variable local; the return value indicates
whether the operation succeeded.


If an attempt is made to use one of the listed functions to transfer a value that does
not fit one of the specific sizes, the result is usually a strange message from the com-
piler, such as “conversion to non-scalar type requested.” In such cases, copy_to_user
or copy_from_user must be used

	*/

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
		i = hello_read(file, (char *)ioctl_param, strlen((char *)ioctl), 0);

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

	return EXIT_SUCCESS;
   
   return EXIT_SUCCESS;
}


/*

fundamental driver operations

This way we can connect the driver operations to the device numbers

The structure is collection of pointer functions, each pointer function have different functionality.
The
operations are mostly in charge of implementing the system calls and are therefore,
named open, read, and so on.

Each field in the structure must point to the function in the
driver that implements a specific operation, or be left NULL for unsupported opera-
tions. The exact behavior of the kernel when a NULL pointer is specified is different
for each function.

*/

/*
inode structure;
is used by the kernel internally to represent files. Therefore, it is
different from the file structure that represents an open file descriptor. There can be
numerous file structures representing multiple open descriptors on a single file, but
they all point to a single inode structure.
The inode structure contains a great deal of information about the file. As a general
rule, only two fields of this structure are of interest for writing driver code:
*/





struct file_operations fops = {
	/*
	The first file_operations field is not an operation at all; it is a pointer to the
module that “owns” the structure. This field is used to prevent the module from
being unloaded while its operations are in use. Almost all the time, it is simply
initialized to THIS_MODULE
	*/
	.owner = THIS_MODULE,
/* Used to retrieve data from the device. A null pointer in this position causes the
read system call to fail with -EINVAL (“Invalid argument”). A nonnegative return
value represents the number of bytes successfully read (the return value is a
“signed size” type, usually the native integer type for the target platform).
*/
	.read = hello_read,

/*
Sends data to the device. If NULL, -EINVAL is returned to the program calling the
write system call. The return value, if nonnegative, represents the number of
bytes successfully written.
*/
	.write = hello_write,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
/*
The ioctl system call offers a way to issue device-specific commands (such as for-
matting a track of a floppy disk, which is neither reading nor writing). Addition-
ally, a few ioctl commands are recognized by the kernel without referring to the
fops table. If the device doesn’t provide an ioctl method, the system call returns
an error for any request that isn’t predefined (-ENOTTY, “No such ioctl for
device”).
*/
       .ioctl = device_ioctl,
#else
/*
*/
       .unlocked_ioctl = device_ioctl,
#endif	

/*
Though this is always the first operation performed on the device file, the driver
is not required to declare a corresponding method. If this entry is NULL, opening
the device always succeeds, but your driver isn’t notified.
*/
	.open = hello_open,

/*
This operation is invoked when the file structure is being released. Like open,
release can be NULL.*
*/
	.release = hello_release
};


///PROC
int	proc_open(struct inode *, struct file *){
    printk(KERN_ALERT"oppen\n");
    return EXIT_SUCCESS;
}

int len=1;

/*
When a process reads from your /proc file, the kernel allocates a page of memory (i.e.,
PAGE_SIZE bytes) where the driver can write data to be returned to user space.

when process access the /proc file it will copy the message and put it in to user space

*/
ssize_t	proc_read(struct file *filp, char __user * buffer, size_t length, loff_t * offs){
    printk(KERN_ALERT"read\n");
 

    if(len){
        len=0;
    }else{
        len=1;
        return EXIT_SUCCESS;
    }


    int remaining = copy_to_user(buffer,msg_Ptr,strlen(msg_Ptr));

	return length;

}

ssize_t	proc_write(struct file *, const char __user *, size_t, loff_t *){
    printk(KERN_ALERT"write\n");

    return EXIT_SUCCESS;
}

int	proc_release(struct inode *, struct file *){
    printk(KERN_ALERT"releas\n");

    return EXIT_SUCCESS;
}

/*
The /proc filesystem is a special, software-created filesystem that is used by the ker-
nel to export information to the world.
They can be written to as well as read from.
Most of the time, however, /proc entries are read-
only files. 
*/
struct proc_ops proc_ops = {
    .proc_open = proc_open,
    .proc_read = proc_read,
    .proc_write = proc_write,
    .proc_release = proc_release
};

	
// PROCK END

struct proc_dir_entry* proc_dir;


/*
* Often, as you look at the kernel API, you will encounter function names starting with
* a double underscore (__). Functions so marked are generally a low-level component
* of the interface and should be used with caution.

Initialization functions should be declared static, since they are not meant to be visi-
ble outside the specific file
The __init token in the
definition it is a hint to the kernel that the given function is
used only at initialization time.

*/
static int __init hello_init(void){
	//registration of the char device
	//0 will give filled in major number
	// module name
	//file operations


/*
Within the kernel, the dev_t type (defined in <linux/types.h>) is used to hold device
numbers—both the major and minor parts.

MKDEV turn the major number and the minor number into dev_t type
*/
int register_status;

// static unsigned int DEVICE_MAJOR_NUMBER = 0;


/*
*	The printk function is defined in the Linux kernel and made available to modules; it
*	behaves similarly to the standard C library function printf. The kernel needs its own
*	printing function because it runs by itself, without the help of the C library. The
*	module can call printk because, after insmod has loaded it, the module is linked to
*	the kernel and can access the kernel’s public symbols.


*	The string KERN_ALERT is the priority of the message.

/usr/src/linux-hwe-5.19-headers-5.19.0-41/include/linux/kern_levels.h
	KERN_EMERG	 0  system is unusable 
	KERN_ALERT	 1  action must be taken immediately 
	KERN_CRIT	 2  critical conditions 
	KERN_ERR	 3  error conditions 
	KERN_WARNING	 4  warning conditions 
	KERN_NOTICE	 5  normal but significant condition 
	KERN_INFO	 6  informational 
	KERN_DEBUG	 7  debug-level messages 

	*The priority is high in this module, because a message with the default
	*priority might not show up anywhere
*/
    printk(KERN_ALERT"Hello proc\n");

	
	printk(KERN_ALERT "The device number is %d",MAJOR_NUMBER);
	
	

	/*
	creating the proc file
	name - name of the file
	mode is the protection mask for the file 0 is default mode
	parent - indicates the directory in which the
file should be created (if base is NULL, the file is created in the /proc root)

	proc_ops - file operations
	*/
    proc_dir = proc_create(PROC_NAME,0666,NULL,&proc_ops);

	/*

		Allocating device number

		major number  -  if 0 then major number will be choosen randomly from the ones
		that are currently free

		name - the device which will be asociated with the major number 		

		As with most kernel functions, the return value will be 0
		if the allocation was successfully performed. In case of error, a negative error code
	*/

/*
	The Older Way of registering chrdev, but new code should not use it.

*/
	register_status =  register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &fops);


	//TODO: if have_error_registering then unregister
	if(register_status < 0){
		printk(KERN_ALERT"Cant register chardev\nError : %d",register_status);
	}
	
	return EXIT_SUCCESS;
}

/*
If your module does not define a cleanup function, the kernel does not allow it to be
unloaded.
*/

static void __exit hello_exit(void){
    proc_remove(proc_dir);	
	

	/*
		Freeing Device number the older way

	*/
	unregister_chrdev(MAJOR_NUMBER,DEVICE_NAME);


	printk(KERN_ALERT "Exit world\n");
}

/*
*	In the official kernel distribution,
*	only an authorized user can load modules; the system call init_module checks if the
*	invoking process is authorized to load a module into the kernel. 

*function hello_init will be invoked
*when the module is loaded into the kernel

*the task of the module’s initialization function is
*to prepare for later invocation of the module’s
*functions;

This macro adds a special section to the mod-
ule’s object code stating where the module’s initialization function is to be found.
Without this definition, your initialization function is never called.

*/
module_init(hello_init);

/*
*function hello_exit will be invoked
*when the module is removed from kernel

*The module’s exit function (hello_exit) gets invoked just before
*the module is unloaded.

*The exit function of a module
*must carefully undo everything the init function built up
*/
module_exit(hello_exit);


/*
*	MODULE_LICENSE is used to tell the
*	kernel that this module bears a free license; without such a declaration, the kernel
*	complains when the module is loaded.

* The GPL allows
* anybody to redistribute, and even sell, a product covered by the GPL, as long as the
* recipient has access to the source and is able to exercise the same rights. Addition-
* ally, any software product derived from a product covered by the GPL must, if it is
* redistributed at all, be released under the GPL.
*/
MODULE_LICENSE("GPL");


MODULE_AUTHOR("Miroslav Genov CT4872");	/* Who wrote this module? */
MODULE_DESCRIPTION("Creates character device driver and file in proc.Gets the message from the device driver and puts it in the proc file");	/* What does this module do */