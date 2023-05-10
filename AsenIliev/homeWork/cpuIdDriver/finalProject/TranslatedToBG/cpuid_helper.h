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
* @brief Ако CPU се поддържа, връща низ на производителя, в противен случай връща празен низ
* @param cpu_support_status this това е стойността, която се връща от  __get_cpuid(eax,ebx,ecx,edx)
* @param addrs_with_stored_ascii_value масив, който съдържа ascii стойност
* @return string 
*/
char* should_get_cpu_manufacturer_id_string(int cpu_support_status, unsigned int* addrs_with_stored_ascii_value[]);

/*
* @brief Проверява дали състоянието на процесора се поддържа
* @param cpu_support_status this това е стойността, която се връща от  __get_cpuid(eax,ebx,ecx,edx)
* @return 0 or 1
*/
int is_cpu_supported(int );


/*
*   @brief Преобразува ASCII стойността в низ
*   @param addrs_with_stored_ascii_value масив със съхранени ascii стойности
*   @return низ
*/
char *convert_and_concat_the_manifacturer_id_string(unsigned int* addrs_with_stored_ascii_value[]);
