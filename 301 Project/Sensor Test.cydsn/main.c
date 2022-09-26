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

uint16 goal = 10;
uint16 pwmIN1 = 100;
uint16 pwmIN2 = 100;



CY_ISR(isr_TC){
    count1 = QuadDec_1_GetCounter();
    count2 = QuadDec_2_GetCounter();
    
    count1 = count1 * 204;
    count1 = count1 / 5;
    count1 = count1 * 100;
    count1 = count1 / 228;
    
    count2 = count2 * 204;
    count2 = count2 / 5;
    count2 = count2 * 100;
    count2 = count2 / 228;
    
    speed1 = count1;
    speed2 = count2;
    
    // left start
    QuadDec_1_SetCounter(0);
    // left start
    QuadDec_2_SetCounter(0);
    
    Timer_1_Start();
}

int main(void){
    // Enable global interrupts as well as start and enable the isr.
    CyGlobalIntEnable;
    timer_clock_Start();
    Timer_1_Start();
    //isr_speed_timer_StartEx(isr_TC);
    //isr_speed_timer_Enable();
    
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
    
    while(1) {
        PWM_1_WriteCompare(150); 
        PWM_2_WriteCompare(150);
        CyDelay(6000);
        PWM_1_WriteCompare(200); 
        PWM_2_WriteCompare(200);
        CyDelay(6000);
        
        
        /*
        if(speed1 >= goal) {
        led_Write(0);
            pwmIN1 = pwmIN1 - 5;
        } else {
            led_Write(1);
            pwmIN1 = pwmIN1 + 5;
        }
        
        if(speed2 > goal) {
            pwmIN2 = pwmIN2 - 10;
        } else {
            pwmIN2 = pwmIN2 + 10;
        }
        
        PWM_1_WriteCompare(pwmIN1); 
        PWM_2_WriteCompare(pwmIN2);
         */
        
        /* STRAIGHT: 
         * MOVE forward
         *
         * CORRECTION:
         * WAIT UNTIL BC off
         * MOVE forward
         * UNTIL FL OR FR off ->
         * IF FL off TURN left UNTIL BC off
         * IF FR off TURN right UNTIL BC off
         *
         * LEFT:
         * WAIT UNTIL FL off
         * TURN left UNTIL CL off
         * MOVE forward UNTIL BC off
         * TURN left UNTIL FL on -> off
         * MOVE forward
         *
         * RIGHT:
         * WAIT UNTIL FR off
         * TURN right UNTIL CR off
         * MOVE forward UNTIL BC off
         * TURN right UNTIL FR on -> off
         * MOVE forward
         */
        
        // Implementation of a state machine to control the robot.
        

        }
    return 0;
        
}

/* [] END OF FILE */
