/* ========================================
 * decoders.h
 * ========================================
*/

#ifndef DECODERS_H_
#define DECODERS_H_

#include "project.h"
    
volatile uint8 compareValueL; // to be used in other files for setting the compare value for the proper speed
volatile uint8 compareValueR;
    
volatile uint8_t speedControlFlag;
    
uint8 goalSpeed; // in decoder count per timer period
    
volatile uint16 countL;
volatile uint16 countR;
volatile uint16 speedL;
volatile uint16 speedR;
    
void decoderInit();
void speedAdjust();

void left_decode_turn();
void right_decode_turn();

void abs_left_turn();
void abs_right_turn();
void abs_left_spot_turn();
void abs_right_spot_turn();

void calibrate();

/* [] END OF FILE */
#endif /* DECODERS_H_ */
