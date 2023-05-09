#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <cpuid.h>
#include <string.h>

#define CPU_IS_SUPPORTED 1
#define CPU_IS_NOT_SUPPORTED 0
#define TOTAL_NUMBER_OF_ADDRS_WITH_ASCII_VALUE 3
#define BYTES_FOR_MANIFACTURER_ID_STRING 255

//TODO to test
// char* manifacturere_id_string;


/*
* @brief If CPU is supported return manifacturer string otherwise returns empty string
* @param cpu_support_status this is the value which is returned from __get_cpuid(eax,ebx,ecx,edx)
* @param addrs_with_stored_ascii_value array which contains asii value
* @return string 
*/
char* should_get_cpu_manufacturer_id_string(int cpu_support_status, unsigned int* addrs_with_stored_ascii_value[]);

/*
* @brief It checks if the CPU status is supported
* @param cpu_support_status this is the value which is returned from __get_cpuid(eax,ebx,ecx,edx)
* @return 0 or 1
*/
int is_cpu_supported(int );


/*
*   @brief It converts the ASCII value into string
*   @param addrs_with_stored_ascii_value array with stored ascii values 
*   @return string
*/
char *convert_and_concat_the_manifacturer_id_string(unsigned int* addrs_with_stored_ascii_value[]);
