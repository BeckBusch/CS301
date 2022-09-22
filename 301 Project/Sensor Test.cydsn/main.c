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




uint16 speed;
uint16 revs;
uint16 distance;

uint16 speed2;
uint16 revs2;

uint16 scale = 228;
uint16 counts[2] = {0};

uint32 goal = 160000;
uint16 pwmIN = 100;
uint16 pwmIN2 = 100;



CY_ISR(isr_TC){
   // left end
   counts[0] = QuadDec_1_GetCounter() * 100;
    // right end
   counts[1] = QuadDec_2_GetCounter() * 100;

    distance = counts[0] / scale;
    distance = revs * 204;
    //speed = speed / 100;
    speed = speed / 5;
    
    revs2 = counts[1] / scale;
    speed2 = revs2 * 204;
    //speed2 = speed2 / 100;
    speed2 = speed2 / 5;
    // speed ends up in mm per ms
    
    // left start
    //QuadDec_1_SetCounter(0);
    // left start
    //QuadDec_2_SetCounter(0);
    
    Timer_1_Start();
}

int main(void){
    // Enable global interrupts as well as start and enable the isr.
    CyGlobalIntEnable;
    //timer_clock_Start();
    //Timer_1_Start();
    
    //isr_speed_timer_StartEx(isr_TC);
    
 
    //isr_speed_timer_Enable();
    
    
    QuadDec_1_Start();
    QuadDec_1_Enable();
    QuadDec_2_Start();
    QuadDec_2_Enable();
        
    PWM_1_WritePeriod(250);
    PWM_1_Start();
    PWM_2_WritePeriod(250);
    PWM_2_Start();
    
    PWM_1_WriteCompare(83); 
    PWM_2_WriteCompare(90);
    
    
    QuadDec_1_SetCounter(0);
    QuadDec_2_SetCounter(0);
    
    while(1) {
       
      /*  
      if (speed < goal){
        led_Write(1);
        pwmIN = pwmIN + 5;
    } else {
        led_Write(0);
        pwmIN = pwmIN - 5;
    }
    if (speed2 < goal){
        pwmIN2 = pwmIN2 + 5;
    } else {
        pwmIN2 = pwmIN2 - 5;
    }*/
        
    counts[0] = QuadDec_1_GetCounter();
    // right end
   //counts[1] = QuadDec_2_GetCounter() * 100;

    revs = counts[0] / scale;
    speed = revs * 204;
    speed = speed / 100;
    //speed = speed / 5;
    
    //revs2 = counts[1] / scale;
    //speed2 = revs2 * 204;
    //speed2 = speed2 / 100;
    //speed2=  speed2 / 5;
    // speed ends up in mm per ms
    
    // left start
    //QuadDec_1_SetCounter(0);
    // left start
    //QuadDec_2_SetCounter(0);
     //
        
        
    
    uint32 dis = (QuadDec_1_GetCounter() /228);
    if (dis >= goal) {
        led_Write(1);
        PWM_1_WriteCompare(125); 
        PWM_2_WriteCompare(125);
    }
        
    
        
    //PWM_1_WriteCompare(pwmIN); 
    //PWM_2_WriteCompare(pwmIN2); 
        
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
