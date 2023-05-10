/*
Header file with ioctl definitions.

The declarations here have to be in a header file, because
they need to be known both to the kernel module
and the process calling ioctl
*/

#ifndef DRIVER_EXAMPLE_H
#define DRIVER_EXAMPLE_H

#include <linux/ioctl.h>

/* 
The major device number. We can't rely on dynamic 
registration any more, because ioctls need to know 
it. 
*/

//главният номер идентифицира драйвера, свързан с устройството.
#define MAJOR_NUMBER 487

//името на устройството
#define DEVICE_NAME "char_driver_CT4872"

//името на /proc файлът
#define PROC_NAME "cpuidCT4872"


/*
_IOR(type,nr,datatype) (за четене на данни от драйверът)
_IOWR(type,nr,datatype) ( за двупосочни(четене/запис) трансфери)



Аргументи на _IOR,_IOWR
Пръви аргумент е идентифицираща буква или число.
Някои устройства ползват major number като идентификатор това е добре
докато той е уникален.

Вторият аргумент е поредица от числа за да може да се различи всеки ioctl

Третият аргумент е  типът на данните, които ще отиват към устройството(e.g.  'int' or 'struct foo').

NOTE!
Do NOT use sizeof(arg) as the third argument as this results in your ioctl thinking
it passes an argument of type size_t.
*/


/* 
Set the message of the device driver
*/
#define IOCTL_SET_MSG _IOR(MAJOR_NUMBER, 0, char *)

/* 
Get the message of the device driver 
*/
#define IOCTL_GET_MSG _IOR(MAJOR_NUMBER, 1, char *)
/* 
This IOCTL is used for output, to get the message 
of the device driver. However, we still need the 
buffer to place the message in to be input, 
as it is allocated by the process.
*/

/* 
Get the n'th byte of the message 
*/
#define IOCTL_GET_NTH_BYTE _IOWR(MAJOR_NUMBER, 2, int)
/* 
The IOCTL is used for both input and output. It 
receives from the user a number, n, and returns 
Message[n]. 
*/



#endif