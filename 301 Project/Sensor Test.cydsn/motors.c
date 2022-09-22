/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "project.h"
#include "motors.h"

/* STRAIGHT: 
 * MOVE forward
 *
 * CORRECTION:
 * WAIT UNTIL BC off THEN
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

void move_forward() {
    
    // Forward movement.
    PWM_1_Start();
    PWM_1_WriteCompare(83);  
    PWM_2_Start();
    PWM_2_WriteCompare(90);
    uint32 i = 0;
    while(i < 5000000) {
        i++;
    }
    
}

void turn_left() {
    
    // Left turning.
    //PWM_1_Stop();
    PWM_2_Start();
    PWM_2_WriteCompare(90);
    uint32 i = 0;
    while(i < 5000000) {
        i++;
    }

}

void turn_right() {
    
    // Right turning.
    //PWM_2_Stop();
    PWM_1_Start();
    PWM_1_WriteCompare(83);
    uint32 i = 0;
    while(i < 5000000) {
        i++;
    }
    
}    

void stop() {
    
    // Stop both motors.
    PWM_1_WriteCompare(40);
    PWM_2_WriteCompare(40);
    //CyDelay(100);
    
}

/* [] END OF FILE */
