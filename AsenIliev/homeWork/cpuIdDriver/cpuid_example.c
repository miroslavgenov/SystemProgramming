#include <stdio.h>
#include <stdlib.h>
#include <cpuid.h>

#define CPU_IS_SUPPORTED 1
#define CPU_IS_NOT_SUPPORTED 0

int is_cpu_supported(int );
void should_print_manifacturer_id_string(int , unsigned int* []);
void print_the_cpu_manufacturer_id_string(unsigned int* []);
void convert_the_value_to_char_and_print(unsigned int );

int
main(void)
{   
    /* Return cpuid data for requested cpuid leaf, as found in returned
   eax, ebx, ecx and edx registers.  The function checks if cpuid is
   supported and returns 1 for valid cpuid information or 0 for
   unsupported cpuid leaf.  All pointers are required to be non-null.  */
    // funcion info of __get_cpuid(...)

    // https://en.wikipedia.org/wiki/CPUID
    // EAX=0: Highest Function Parameter and Manufacturer ID
    // This returns the CPU's manufacturer ID string – 
    // a twelve-character ASCII string stored in 
    // EBX, EDX, ECX (in that order). The
    //  highest basic calling parameter (the largest value that EAX can be set to before calling CPUID) 
    // is returned in EAX. 
    
    unsigned int eax = 0, ebx, ecx ,edx;
    int cpu_support_status = __get_cpuid(eax, &eax, &ebx, &edx, &ecx);

    unsigned int* addrs_with_stored_ascii_value[3] = {&ebx, &ecx, &edx};
    
    should_print_manifacturer_id_string(cpu_support_status, addrs_with_stored_ascii_value);

   return EXIT_SUCCESS;
}

void
should_print_manifacturer_id_string(int cpu_support_status, unsigned int* addresses[]){
        if(is_cpu_supported(cpu_support_status)){
            print_the_cpu_manufacturer_id_string(addresses);
            return;
        }

        printf("\nThe CPU is not supported\n");
}

int
is_cpu_supported(int cpu_support_status){
    return cpu_support_status == CPU_IS_SUPPORTED;
}

void
print_the_cpu_manufacturer_id_string(unsigned int* addresses[]){
    for(int i = 0; i < 3; i++){
        convert_the_value_to_char_and_print(*addresses[i]);
    }
}

void
convert_the_value_to_char_and_print(unsigned int value){
    char *ascii_string = (char*)&value;

    for(int i = 0; i < sizeof(int); i++){
        printf("%c", ascii_string[i]);
    }
}