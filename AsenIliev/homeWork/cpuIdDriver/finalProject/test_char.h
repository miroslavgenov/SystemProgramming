/*
 *  *  char_driver.h - the header file with the ioctl definitions.
 *  *
 *  *  The declarations here have to be in a header file, because
 *  *  they need to be known both to the kernel module
 *  *  (in char_driver.c) and the process calling ioctl (ioctl.c)
 *  */

#ifndef DRIVER_EXAMPLE_H
#define DRIVER_EXAMPLE_H

#include <linux/ioctl.h>

/* 
 *  * The major device number. We can't rely on dynamic 
 *  * registration any more, because ioctls need to know 
 *  * it. 
 *  */

/*
Traditionally, the major number identifies the driver associated with the device.

*/

#define MAJOR_NUMBER 487

/*  
* The name of the device file 
*/
#define DEVICE_NAME "char_driver_CT4872"

// mknod DEVICE_NAME c MAJOR_NUMBER MINOR_NUMBER

#define PROC_NAME "cpuidCT4872"


/*
Arguments to _IOR,_IOWR
The first argument to _IO, _IOW, _IOR, or _IOWR is an identifying letter
or number from the table below.  Because of the large number of drivers,
many drivers share a partial letter with other drivers.

The second argument to _IO, _IOW, _IOR, or _IOWR is a sequence number
to distinguish ioctls from each other.



The third argument to _IOW,
_IOR, or _IOWR is the type of the data going into the kernel or coming
out of the kernel (e.g.  'int' or 'struct foo').  NOTE!  Do NOT use
sizeof(arg) as the third argument as this results in your ioctl thinking
it passes an argument of type size_t.

Some devices use their major number as the identifier; this is OK, as
long as it is unique.  Some devices are irregular and don't follow any
convention at all.

*/


/* 
 * Set the message of the device driver

    _IOR   an ioctl with read parameters  (copy_to_user)

    _IOR(type,nr,datatype) (for reading data from the
driver)

 */
#define IOCTL_SET_MSG _IOR(MAJOR_NUMBER, 0, char *)

/* 
 * Get the message of the device driver 

 */
#define IOCTL_GET_MSG _IOR(MAJOR_NUMBER, 1, char *)
/* 
 * This IOCTL is used for output, to get the message 
 * of the device driver. However, we still need the 
 * buffer to place the message in to be input, 
 * as it is allocated by the process.
 */

/* 
 * Get the n'th byte of the message 

     _IOWR  an ioctl with both write and read parameters

     IOWR(type,nr,datatype) (for bidirectional transfers)
 */
#define IOCTL_GET_NTH_BYTE _IOWR(MAJOR_NUMBER, 2, int)
/* 
 * The IOCTL is used for both input and output. It 
 * receives from the user a number, n, and returns 
 * Message[n]. 
 */




#endif
