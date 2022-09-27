#include "project.h"
#include "motors.h"

#define ON 1
#define OFF 0

// FL, FR, CL, CR, BC = front left, front right, center left, center right, back center respectively.
#define FL 0
#define FR 1
#define CL 2
#define CR 3
#define BC 4

// State values.
#define FORWARD 0
#define TURNING_LEFT 1
#define TURNING_RIGHT 2
#define CORRECTION 3

// Prototype declarations.
CY_ISR_PROTO(isr_TC);

//uint16 scale = 228;
uint16 count1;
uint16 count2;
uint16 speed1;
uint16 speed2;

uint16 goal = 30;
uint16 pwmIN1 = 150;
uint16 pwmIN2 = 150;



CY_ISR(isr_TC){
    count1 = QuadDec_1_GetCounter();
    count2 = QuadDec_2_GetCounter();
    
    /*
    count1 = count1 * 204;
    count1 = count1 / 5;
    count1 = count1 * 100;
    count1 = count1 / 228;
    
    count2 = count2 * 204;
    count2 = count2 / 5;
    count2 = count2 * 100;
    count2 = count2 / 228;
    */
    
    count1 = count1 * 2000;
    count1 = count1 / 228;
    
    count2 = count2 * 2000;
    count2 = count2 / 228;
    
    speed1 = count1;
    speed2 = count2;
    
    
    
    if(speed1 >= goal) {
            pwmIN1 = pwmIN1 - 1;
        } else {
            pwmIN1 = pwmIN1 + 1;
        }
        
        if(speed2 > goal) {
            pwmIN2 = pwmIN2 - 1;
        } else {
            pwmIN2 = pwmIN2 + 1;
        }

        
        
    
    QuadDec_1_SetCounter(0);
    QuadDec_2_SetCounter(0);
    Timer_1_Start();
}

int main(void){
    // Enable global interrupts as well as start and enable the isr.
    CyGlobalIntEnable;
    timer_clock_Start();
    Timer_1_Start();
    isr_speed_timer_StartEx(isr_TC);
    isr_speed_timer_Enable();
    
    QuadDec_1_Start();
    QuadDec_1_Enable();
    QuadDec_2_Start();
    QuadDec_2_Enable();
        
    PWM_1_WritePeriod(200);
    PWM_1_Start();
    PWM_2_WritePeriod(200);
    PWM_2_Start();
    
    PWM_1_WriteCompare(pwmIN1); 
    PWM_2_WriteCompare(pwmIN2);
    
    QuadDec_1_SetCounter(0);
    QuadDec_2_SetCounter(0);
    
    //uint16 check = 3 * 19 * 4 * 2;
    
    while(1) {
        PWM_1_WriteCompare(pwmIN1); 
        PWM_2_WriteCompare(pwmIN2);
        
        
        /*
        if(speed1 >= goal) {
            pwmIN1 = pwmIN1 - 1;
        } else {
            pwmIN1 = pwmIN1 + 1;
        }
        
        if(speed2 > goal) {
            pwmIN2 = pwmIN2 - 1;
        } else {
            pwmIN2 = pwmIN2 + 1;
        }

        
        PWM_1_WriteCompare(pwmIN1); 
        PWM_2_WriteCompare(pwmIN2);
         
*/
        }
    return 0;
        
}

/* [] END OF FILE */
