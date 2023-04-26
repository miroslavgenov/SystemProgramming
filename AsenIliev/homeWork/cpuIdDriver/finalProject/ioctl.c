#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		/* open */
#include <unistd.h>		/* exit */
#include <sys/ioctl.h>		/* ioctl */

#include "cpuid_helper.h"
#include "char_driver.h"

void main(void)
{	

	
	unsigned int eax = 0, ebx, ecx ,edx;
    int cpu_support_status = __get_cpuid(eax, &eax, &ebx, &edx, &ecx);

    unsigned int* addrs_with_stored_ascii_value[TOTAL_NUMBER_OF_ADDRS_WITH_ASCII_VALUE] = {
       &ebx, &ecx, &edx
    };
	
	int file_desc, ret_val;

    char* msg = get_cpu_manufacturer_id_string(cpu_support_status,addrs_with_stored_ascii_value);
	file_desc = open(DEVICE_NAME, 0);
	if (file_desc < 0) {
		printf("Can't open device file: %s\n", DEVICE_NAME);
        return;
    }
    
    ioctl(file_desc, IOCTL_SET_MSG, msg);
	

	close(file_desc);

}
