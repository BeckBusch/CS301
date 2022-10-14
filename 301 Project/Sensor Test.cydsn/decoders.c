/* ========================================
 * decoders.c
 * 
 * count /3 per motor /19 per rev /4 dec resoulution /5 ms *204 mm wheel circumference
 * 
 * mojor problem is that this will be frequently updating the compare value to be used but the motot functions might not be using it fast enough
 * ========================================
*/

#include "decoders.h"

#define FULL_TURN 228
#define QUART_TURN 57
#define HALF_TURN 114

#define L 0
#define R 1

/*
CY_ISR_PROTO(decTimerISR);

CY_ISR(decTimerISR) {
    countL = QuadDec_L_GetCounter();
    countR = QuadDec_R_GetCounter();
    
    // some code lol
    // haven't decided on good units yet
    
    speedL = countL;
    speedR = countR;
    
    QuadDec_L_SetCounter(0);
    QuadDec_R_SetCounter(0);
}*/

void decoderInit(){
    //compareValueL = 163;
    //compareValueR = 170;
    //goalSpeed = 65;
    
    //Clock_Dec_Start(); not needed maybe???
    // Dec_Timer_Clock_Start();
    
    //Dec_Timer_Start(); // start the timer for the decoder polls
    //Dec_Timer_ISR_StartEx(decTimerISR); // start the isr for the timer
    
    QuadDec_L_Start();
    QuadDec_R_Start();
}

void abs_left_turn() {  
    PWM_1_WriteCompare(125);
    PWM_2_WriteCompare(170);
    
    QuadDec_R_SetCounter(0);
    
    while(QuadDec_R_GetCounter() < FULL_TURN);
    PWM_2_WriteCompare(125);
}

void abs_right_turn() {    
    PWM_2_WriteCompare(125);
    PWM_1_WriteCompare(170);
    
    QuadDec_L_SetCounter(0);
    
    while(QuadDec_L_GetCounter() < FULL_TURN);
    
    PWM_1_WriteCompare(125);
}

void abs_left_spot_turn() {  
    PWM_1_WriteCompare(163);
    PWM_2_WriteCompare(170);
    
    QuadDec_R_SetCounter(0);
    QuadDec_L_SetCounter(0);
    uint8_t flags[2] = {1, 1};
    
    while (flags[L] || flags[R]) { // while less than two motors finished
        if(QuadDec_R_GetCounter() > QUART_TURN * 0.75){
            PWM_2_WriteCompare(125);
            flags[R] = 0;
        }
        if(QuadDec_L_GetCounter() > QUART_TURN * 0.75){
            PWM_1_WriteCompare(125);
            flags[L] = 0;
        }
    }
    while(1);
    PWM_1_WriteCompare(80);
    PWM_2_WriteCompare(170);
    
    QuadDec_R_SetCounter(0);
    QuadDec_L_SetCounter(HALF_TURN + 500);
    flags[L] = 1; flags[R] = 1;
    
    while (flags[L] || flags[R]) { // while less than two motors finished
        if(QuadDec_R_GetCounter() > HALF_TURN){
            PWM_2_WriteCompare(125);
            flags[R] = 0;
        }
        if(QuadDec_L_GetCounter() < 500){
            PWM_1_WriteCompare(125);
            flags[L] = 0;
        }
    }
    
    PWM_1_WriteCompare(125);
    PWM_2_WriteCompare(125);
}

void abs_right_spot_turn() {    
    PWM_2_WriteCompare(170);
    PWM_1_WriteCompare(163);
    
    QuadDec_R_SetCounter(0);
    QuadDec_L_SetCounter(0);
    uint8_t flags[2] = {1, 1};
    
    while (flags[L] || flags[R]) { // while less than two motors finished
        if(QuadDec_R_GetCounter() > QUART_TURN * 0.75){
            PWM_2_WriteCompare(125);
            flags[R] = 0;
        }
        if(QuadDec_L_GetCounter() > QUART_TURN * 0.75){
            PWM_1_WriteCompare(125);
            flags[L] = 0;
        }
    }

    PWM_1_WriteCompare(170);
    PWM_2_WriteCompare(80);
    
    QuadDec_L_SetCounter(0);
    QuadDec_R_SetCounter(HALF_TURN + 500);
    flags[L] = 1; flags[R] = 1;
    
    while (flags[L] || flags[R]) { // while less than two motors finished
        if(QuadDec_L_GetCounter() > HALF_TURN){
            PWM_1_WriteCompare(125);
            flags[L] = 0;
        }
        if(QuadDec_R_GetCounter() < 500){
            PWM_2_WriteCompare(125);
            flags[R] = 0;
        }
    }
    
    PWM_1_WriteCompare(125);
    PWM_2_WriteCompare(125);
        while(1);
}

void speedAdjust() {
    if (speedL > goalSpeed){ // if speed greater than goal
        compareValueL = compareValueL - 1; // decrease compare value
    } else {
        compareValueL = compareValueL + 1; // otherwise increase it
    }
    
    if (speedR > goalSpeed){
        compareValueR = compareValueR - 1;
    } else {
        compareValueR = compareValueR + 1;
    }
}


/* [] END OF FILE */
