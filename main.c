/* 
 * File:   main.c
 * Author: exp10
 *
 * Created on June 6, 2020, 4:18 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "compressor.h"

/*
 * 
 */
int main(int argc, char** argv) {
    int16_t samples[48];
    
    process_compressor_16bit(samples, -15, 8, 2, 6.5, 432);
    while(1){
        
    }
    return (EXIT_SUCCESS);
}

