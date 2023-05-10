#include <stdio.h>
#include <fcntl.h>		/* open */
#include <unistd.h>		/* close(fd) */
#include <sys/ioctl.h>		/* ioctl */

#include "cpuid_helper.h"
#include "test_char.h"

/// @brief файлут отворен ли е ?
/// @param  файлов дескриптор
/// @return 1 или 0
int is_file_open(int );


void
main(void){	

    
	unsigned int eax = 0, ebx, ecx ,edx;
    
    /*https://en.wikipedia.org/wiki/CPUID
    __get_cpuid: This returns the CPU's manufacturer ID string – 
    a twelve-character ASCII string stored in EBX, EDX, ECX (in that order).
    */
    int cpu_support_status = __get_cpuid(eax, &eax, &ebx, &edx, &ecx);

    unsigned int* addrs_with_stored_ascii_value[TOTAL_NUMBER_OF_ADDRS_WITH_ASCII_VALUE] = {
       &ebx, &ecx, &edx
    };
	
	int file_desc;

    char* msg = should_get_cpu_manufacturer_id_string(cpu_support_status,addrs_with_stored_ascii_value);

    /*
    open() отваря файл посочен на директорията.Ако файлът е отворен успешно връща 
    дескриптор, който не е негативна стойност.

    open() създава нов файлов дескриптор, записфа файловият offset и файловото състояние.

    аргументи на фунцията
    __file директория до файлът
    __oflag трябва да включва някои от следните modes:
        O_RDONLY read-only
        O_WRONLY write-only
        O_RDWR read/write    
    */
    file_desc = open(DEVICE_NAME, O_WRONLY);
	
    if (is_file_open(file_desc)) {

        /*
        ioctl(входно-изходен контрол) е системно извикване, с което може да се четът/записват данни.
        __fd файлов дескриптор над който ще се извършват операции
        
        __request идентифицира коя команда да бъде изпълнена.С тези
        команди може да се попира дата.

        ... това са аргументите/дата/структура/... обикновенно указател

        В зависимост от __request зависи как ще бъде третирани аргументите.
        Някои команди не приемат аргументи, други приемат(int,pointer,...)
        Чрез ioctl устройството е способно да обмени данни с user space.

        */
        ioctl(file_desc, IOCTL_SET_MSG, msg);
    }else{
		printf("Can't open device file: %s\n", DEVICE_NAME);
    }
    

	close(file_desc);

}

int is_file_open(int fd_status){
    return fd_status >= 0;
}