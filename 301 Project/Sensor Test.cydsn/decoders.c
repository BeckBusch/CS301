/* ========================================
 * decoders.c
 * 
 * count /3 per motor /19 per rev /4 dec resoulution /5 ms *204 mm wheel circumference
 * 
 * mojor problem is that this will be frequently updating the compare value to be used but the motot functions might not be using it fast enough
 * ========================================
*/

#include "decoders.h"

#define FULL_TURN 220 //228
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
    
    ; // start the isr for the timer
    
    QuadDec_L_Start();
    QuadDec_R_Start();
}

void resetCounter() {
    QuadDec_L_SetCounter(0);
    QuadDec_R_SetCounter(0);
}

void abs_left_turn() {
    

}

void abs_180() {
    // set new speeds
    PWM_1_WriteCompare(88);
    PWM_2_WriteCompare(170);
    
    // reset counters and flags
    QuadDec_R_SetCounter(0);
    QuadDec_L_SetCounter(205 + 500);
    uint8_t flags[2] = {1, 1};;
    
    // while flags poll turn
    while (flags[L] || flags[R]) { // while less than two motors finished
        if(QuadDec_R_GetCounter() > 205){
            PWM_2_WriteCompare(125);
            flags[R] = 0;
        }
        if(QuadDec_L_GetCounter() < 500){
            PWM_1_WriteCompare(125);
            flags[L] = 0;
        }
    }
}

void abs_right_turn() {    
    PWM_2_WriteCompare(125);
    PWM_1_WriteCompare(170);
        
    if (QuadDec_L_GetCounter() < FULL_TURN);
    
    PWM_1_WriteCompare(125);
}

void rotate() {
    // rotate both wheels in opposite directions.
    PWM_1_WriteCompare(170);
    PWM_2_WriteCompare(80);
    
    // reset flags and counters
    QuadDec_R_SetCounter(0);
    QuadDec_L_SetCounter(0);
    uint8_t flags[2] = {1, 1};
    
    // CHANGE THIS DISTANCE TO DO A 180!!!
    // while flags poll turn
    while (flags[L] || flags[R]) { // while less than two motors finished
        if(QuadDec_R_GetCounter() > QUART_TURN){
            PWM_2_WriteCompare(125);
            flags[R] = 0;
        }
        if(QuadDec_L_GetCounter() > QUART_TURN){
            PWM_1_WriteCompare(125);
            flags[L] = 0;
        }
    }
}

void move_unit() {
    // set forward speed
    PWM_1_WriteCompare(163);
    PWM_2_WriteCompare(170);
    
    // reset flags and counters
    QuadDec_R_SetCounter(0);
    QuadDec_L_SetCounter(0);
    uint8_t flags[2] = {1, 1};
    
    // while flags poll turn
    while (flags[L] || flags[R]) { // while less than two motors finished
        if(QuadDec_R_GetCounter() > 10){//QUART_TURN
            PWM_2_WriteCompare(125);
            flags[R] = 0;
        }
        if(QuadDec_L_GetCounter() > 10 ){//{QUART_TURN
            PWM_1_WriteCompare(125);
            flags[L] = 0;
        }
    }
    
    PWM_1_WriteCompare(125);
    PWM_2_WriteCompare(125);
}

void abs_left_spot_turn() {
    
    // set forward speed
    PWM_1_WriteCompare(163);
    PWM_2_WriteCompare(170);
    
    // reset flags and counters
    QuadDec_R_SetCounter(0);
    QuadDec_L_SetCounter(0);
    uint8_t flags[2] = {1, 1};
    
    // while flags poll turn
    while (flags[L] || flags[R]) { // while less than two motors finished
        if(QuadDec_R_GetCounter() > QUART_TURN){
            PWM_2_WriteCompare(125);
            flags[R] = 0;
        }
        if(QuadDec_L_GetCounter() > QUART_TURN){
            PWM_1_WriteCompare(125);
            flags[L] = 0;
        }
    }
    
    // set new speeds
    PWM_1_WriteCompare(87);
    PWM_2_WriteCompare(170);
    
    // reset counters and flags
    QuadDec_R_SetCounter(0);
    QuadDec_L_SetCounter(HALF_TURN + 500);
    flags[L] = 1; flags[R] = 1;
    
    // while flags poll turn
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
    
    // stop motors
    //PWM_1_WriteCompare(125);
    //PWM_2_WriteCompare(125);

    //while(1);//hang
}

void abs_right_spot_turn() {
    // set speeds for advance
    PWM_2_WriteCompare(170);
    PWM_1_WriteCompare(163);
    
    // reset counters and flags
    QuadDec_R_SetCounter(0);
    QuadDec_L_SetCounter(0);
    uint8_t flags[2] = {1, 1};
    
    // poll motots
    while (flags[L] || flags[R]) { // while less than two motors finished
        if(QuadDec_R_GetCounter() > QUART_TURN){
            PWM_2_WriteCompare(125);
            flags[R] = 0;
        }
        if(QuadDec_L_GetCounter() > QUART_TURN){
            PWM_1_WriteCompare(125);
            flags[L] = 0;
        }
    }

    // set new speeds for turn
    PWM_1_WriteCompare(163);
    PWM_2_WriteCompare(80);
    
    // reset counters and flags
    QuadDec_L_SetCounter(0);
    QuadDec_R_SetCounter(HALF_TURN + 500);
    flags[L] = 1; flags[R] = 1;
    
    // poll motors
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
    
    // stop motor
    //PWM_1_WriteCompare(125);
    //PWM_2_WriteCompare(125);
    
    //while(1);//hang
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
