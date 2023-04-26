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
#define MAJOR_NUMBER 487

/*  * The name of the device file 
  */
#define DEVICE_NAME "char_driver_CT4872"

// mknod DEVICE_NAME c MAJOR_NUMBER MINOR_NUMBER

#define PROC_NAME "cpuidCT4872"

/* 
 * Set the message of the device driver 
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
 */
#define IOCTL_GET_NTH_BYTE _IOWR(MAJOR_NUMBER, 2, int)
/* 
 * The IOCTL is used for both input and output. It 
 * receives from the user a number, n, and returns 
 * Message[n]. 
 */




#endif
