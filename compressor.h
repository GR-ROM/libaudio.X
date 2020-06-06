/* 
 * File:   compressor.h
 * Author: exp10
 *
 * Created on June 6, 2020, 4:19 PM
 */

#ifndef COMPRESSOR_H
#define	COMPRESSOR_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
void process_compressor_16bit(int16_t *samples, float threshold, float ratio, float makeUpGain, float attack, float release);


#ifdef	__cplusplus
}
#endif

#endif	/* COMPRESSOR_H */

