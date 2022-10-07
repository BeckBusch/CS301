/* ========================================
 * decoders.c
 * 
 * count /3 per motor /19 per rev /4 dec resoulution /5 ms *204 mm wheel circumference
 * 
 * mojor problem is that this will be frequently updating the compare value to be used but the motot functions might not be using it fast enough
 * ========================================
*/

#include "decoders.h"

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
}

void decoderInit(){
    compareValueL = 163;
    compareValueR = 170;
    goalSpeed = 65;
    
    //Clock_Dec_Start(); not needed maybe???
    // Dec_Timer_Clock_Start();
    
    Dec_Timer_Start(); // start the timer for the decoder polls
    Dec_Timer_ISR_StartEx(decTimerISR); // start the isr for the timer
    
    QuadDec_L_Start();
    QuadDec_R_Start();
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
