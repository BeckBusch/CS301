/* ========================================
 * decoders.h
 * ========================================
*/

#ifndef DECODERS_H_
#define DECODERS_H_

#include "project.h"
            
volatile uint8_t resetQuad;
    
uint16 countTarget; // in decoder count per timer period
    
volatile uint16 countL;
volatile uint16 countR;
    
void decoderInit();
void adjustSpeed();

void left_decode_turn();
void right_decode_turn();

void abs_left_turn();
void abs_right_turn();
void abs_left_spot_turn();
void abs_right_spot_turn();

void calibrate();

/* [] END OF FILE */
#endif /* DECODERS_H_ */
