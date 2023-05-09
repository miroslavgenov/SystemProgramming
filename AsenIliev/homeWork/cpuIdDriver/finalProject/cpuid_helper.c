#include "cpuid_helper.h"

char*
should_get_cpu_manufacturer_id_string(int cpu_support_status, unsigned int* addrs_with_stored_ascii_value[]){
    if(is_cpu_supported(cpu_support_status)){
        return convert_and_concat_the_manifacturer_id_string(addrs_with_stored_ascii_value);
    }
    return "";
}

int
is_cpu_supported(int cpu_support_status){
    return cpu_support_status == CPU_IS_SUPPORTED;
}

char*
convert_and_concat_the_manifacturer_id_string(unsigned int* addrs_with_stored_ascii_value[]){
    int ascii_value;
    char* ascii_string;

    //TODO to test
    char *manifacturere_id_string = malloc(BYTES_FOR_MANIFACTURER_ID_STRING);

    for(int i = 0; i < TOTAL_NUMBER_OF_ADDRS_WITH_ASCII_VALUE ;i++){
        ascii_value = *addrs_with_stored_ascii_value[i];
        ascii_string = (char*)&ascii_value;

        strcat(manifacturere_id_string, ascii_string);
    }

    return manifacturere_id_string;
}