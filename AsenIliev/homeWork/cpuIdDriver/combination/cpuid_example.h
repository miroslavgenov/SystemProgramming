#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <cpuid.h>
#include <string.h>

#define CPU_IS_SUPPORTED 1
#define CPU_IS_NOT_SUPPORTED 0
#define TOTAL_NUMBER_OF_ADDRS_WITH_ASCII_VALUE 3

char* get_cpu_manifacturer_id_string(int cpu_support_status, unsigned int* addrs_with_stored_ascii_value[]);
int is_cpu_supported(int );
char *convert_and_concat_the_manifacturer_id_string(unsigned int* addrs_with_stored_ascii_value[]);