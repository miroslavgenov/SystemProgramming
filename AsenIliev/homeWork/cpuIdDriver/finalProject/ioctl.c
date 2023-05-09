#include <stdio.h>
#include <fcntl.h>		/* open */
#include <unistd.h>		/* close(fd) */
#include <sys/ioctl.h>		/* ioctl */

#include "cpuid_helper.h"
#include "test_char.h"


/*
ioctl system call, which is
a common interface used for device control 

*/

int is_file_open(int fd_status){
    return fd_status >= 0;
}


void
main(void){	

    
	unsigned int eax = 0, ebx, ecx ,edx;
    
    /*
    https://en.wikipedia.org/wiki/CPUID
    This returns the CPU's manufacturer ID string – 
    a twelve-character ASCII string stored in EBX, EDX, ECX (in that order).
    */
    int cpu_support_status = __get_cpuid(eax, &eax, &ebx, &edx, &ecx);

    unsigned int* addrs_with_stored_ascii_value[TOTAL_NUMBER_OF_ADDRS_WITH_ASCII_VALUE] = {
       &ebx, &ecx, &edx
    };
	
	int file_desc;

    char* msg = should_get_cpu_manufacturer_id_string(cpu_support_status,addrs_with_stored_ascii_value);

    //TODO to test
    //The  open() opens the file specified by pathname.
    //returns descriptor non negative if the file were opened successfully
    /*
    
    A call to open() creates a new open file description, an entry in
       the system-wide table of open files.  The open file description
       records the file offset and the file status flags (see below).  A
       file descriptor is a reference to an open file description; this
       reference is unaffected if pathname is subsequently removed or
       modified to refer to a different file.  For further details on
       open file descriptions, see NOTES.

          The argument flags must include one of the following access
       modes: O_RDONLY, O_WRONLY, or O_RDWR.  These request opening the
       file read-only, write-only, or read/write, respectively.

    
    */
    file_desc = open(DEVICE_NAME, O_WRONLY);
	
    //TODO comment what is happening here
    if (is_file_open(file_desc)) {
        //TODO comment what is happening here

        // printf("\nfd: %d\n",file_desc);

        /*
        ioctl is a system call that acts on a file
        descriptor; it receives a number that identifies a command to be performed and
        (optionally) another argument, usually a pointer.
        
        These commands can copy relevant data structures from the driver to user space where you
        can examine them.

        The dots in the prototype represent
            not a variable number of arguments but a single optional argument, traditionally
            identified as char *argp.

            The actual nature of the third argument depends on the specific con-
trol command being issued (the second argument). Some commands take no argu-
ments, some take an integer value, and some take a pointer to other data. Using a
pointer is the way to pass arbitrary data to the ioctl call; the device is then able to
exchange any amount of data with user space.

        */
        ioctl(file_desc, IOCTL_SET_MSG, msg);
    }else{
		printf("Can't open device file: %s\n", DEVICE_NAME);
    }
    

	close(file_desc);

}
