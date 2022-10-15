/* ========================================
 * decoders.c
 * 
 * count /3 per motor /19 per rev /4 dec resoulution /5 ms *204 mm wheel circumference
 * 
 * mojor problem is that this will be frequently updating the compare value to be used but the motot functions might not be using it fast enough
 * ========================================
*/

#include "decoders.h"
#include "motors.h"

#define FULL_TURN 228
#define QUART_TURN 57
#define HALF_TURN 114

#define L 0
#define R 1

uint8 compareValueL; // to be used in other files for setting the compare value for the proper speed
uint8 compareValueR;

uint8 toggle = 0;

uint8 adjustOff = 0;

CY_ISR_PROTO(decTimerISR);

CY_ISR(decTimerISR) {
    countL = QuadDec_L_GetCounter();
    countR = QuadDec_R_GetCounter();
        
    //QuadDec_L_SetCounter(0);
    //QuadDec_R_SetCounter(0);   
    
    resetQuad = 1;
}

void decoderInit(){
    
    resetQuad = 0;
    compareValueR = 45;
    compareValueL = 45;//compareValueR - 6;
    countTarget = 100;
    
    //Clock_Dec_Start(); not needed maybe???
    //Dec_Timer_Clock_Start();
    
    Dec_Timer_Start(); // start the timer for the decoder polls
    Dec_Timer_ISR_StartEx(decTimerISR); // start the isr for the timer
    
    QuadDec_L_Start();
    QuadDec_R_Start();
}

void left_decode_turn(){
    resetQuad = 0;
    Dec_Timer_ISR_Disable();
    abs_left_spot_turn();
    Dec_Timer_ISR_Enable();
    //move_forward();
}
void right_decode_turn(){
    resetQuad = 0;
    Dec_Timer_ISR_Disable();
    abs_right_spot_turn();
    Dec_Timer_ISR_Enable();
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
    
    // set forward speed
    //PWM_1_WriteCompare(compareValueL + 125);
    //PWM_2_WriteCompare(compareValueR + 125);
    PWM_1_WriteCompare(170);
    PWM_2_WriteCompare(163);
    
    // reset flags and counters
    QuadDec_R_SetCounter(0);
    QuadDec_L_SetCounter(0);
    uint8_t flags[2] = {1, 1};
    
    // while flags poll turn
    while (flags[L] || flags[R]) { // while less than two motors finished
        if(QuadDec_R_GetCounter() > QUART_TURN * 0.7){
            PWM_2_WriteCompare(125);
            flags[R] = 0;
        }
        if(QuadDec_L_GetCounter() > QUART_TURN * 0.7){
            PWM_1_WriteCompare(125);
            flags[L] = 0;
        }
    }
    
    //while(1);
    
    // set new speeds
    //PWM_1_WriteCompare(125 - compareValueL);
    //PWM_2_WriteCompare(125 + compareValueR);
    
    // reset counters and flags
    //QuadDec_R_SetCounter(0);
    //QuadDec_L_SetCounter(HALF_TURN + 500);
    //flags[L] = 1; flags[R] = 1;
    
    // while flags poll turn
    //while (flags[L] || flags[R]) { // while less than two motors finished
    //    if(QuadDec_R_GetCounter() > HALF_TURN){
    //        PWM_2_WriteCompare(125);
    //        flags[R] = 0;
    //    }
    //    if(QuadDec_L_GetCounter() < 500){
    //        PWM_1_WriteCompare(125);
    //        flags[L] = 0;
    //    }
    //}
    
    // stop motors
    //PWM_1_WriteCompare(125);
    //PWM_2_WriteCompare(125);
    
}

void abs_right_spot_turn() {
    
    // set speeds for advance
    //PWM_1_WriteCompare(compareValueL + 125);
    //PWM_2_WriteCompare(compareValueR + 125);
    PWM_1_WriteCompare(170);
    PWM_2_WriteCompare(163);
    
    // reset counters and flags
    QuadDec_R_SetCounter(0);
    QuadDec_L_SetCounter(0);
    uint8_t flags[2] = {1, 1};
    
    // poll motots
    while (flags[L] || flags[R]) { // while less than two motors finished
        if(QuadDec_R_GetCounter() > QUART_TURN * 0.8){
            PWM_2_WriteCompare(125);
            flags[R] = 0;
        }
        if(QuadDec_L_GetCounter() > QUART_TURN * 0.8){
            PWM_1_WriteCompare(125);
            flags[L] = 0;
        }
    }
    
    //while(1);
    
    // set new speeds for turn
    //PWM_1_WriteCompare(125 + compareValueL);
    //PWM_2_WriteCompare(125 - compareValueR);
    
    // reset counters and flags
    //QuadDec_L_SetCounter(0);
    //QuadDec_R_SetCounter(HALF_TURN + 500);
    //flags[L] = 1; flags[R] = 1;
    
    // poll motors
    //while (flags[L] || flags[R]) { // while less than two motors finished
    //    if(QuadDec_L_GetCounter() > HALF_TURN){
    //        PWM_1_WriteCompare(125);
    //        flags[L] = 0;
    //    }
    //    if(QuadDec_R_GetCounter() < 500){
    //        PWM_2_WriteCompare(125);
    //        flags[R] = 0;
    //    }
    //}
    
    // stop motor
    //PWM_1_WriteCompare(125);
    //PWM_2_WriteCompare(125);
    
}

void adjustSpeed() {
    
    if (resetQuad == 0 || adjustOff == 5){ led_1_Write(0); return; } 
    
    if (countL > countTarget){ // if speed greater than goal
        compareValueL = compareValueL - countTarget / 20; // decrease compare value
    } else if (countL < countTarget) {
        compareValueL = compareValueL + countTarget / 20; // otherwise increase it
    }
    
    if (countR > countTarget){
        compareValueR = compareValueR - countTarget / 20;
    } else if (countL < countTarget) {
        compareValueR = compareValueR + countTarget / 20;
    }
    
    //speedControlFlag = 0;
    
    PWM_1_WriteCompare(compareValueL + 125);  
    //PWM_2_Start();
    PWM_2_WriteCompare(compareValueR + 125);
    
        if (resetQuad == 1) {
            resetQuad = 0;
            QuadDec_L_SetCounter(0);
            QuadDec_R_SetCounter(0);
        }
        adjustOff++;
    
}

void calibrate(){
    PWM_1_WriteCompare(160);
    PWM_2_WriteCompare(160);
    
    CyDelay(5000);
    stop();
    
    uint16_t count1 = QuadDec_L_GetCounter();
    uint16_t count2 = QuadDec_R_GetCounter();
    
    double diff = (double) count1 / count2;
    diff = diff -1;
    int printer = diff * 100;
    
    for (int i=0; i<printer; i++){
        led_Write(1);
        CyDelay(500);
        led_Write(0);
        CyDelay(200);
    }
}

/* [] END OF FILE */
