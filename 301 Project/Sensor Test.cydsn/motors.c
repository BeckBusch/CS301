/* ========================================
 * motors.c
 * ========================================
*/

#include "project.h"
#include "motors.h"
#include "decoders.h"

extern uint8 compareValueL;
extern uint8 compareValueR;

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
    //speedControlFlag = 1;
    // Forward movement.
    //PWM_1_Start();
    //PWM_1_WriteCompare(compareValueL + 125);  
    //PWM_2_Start();
    //PWM_2_WriteCompare(compareValueR + 125);
    PWM_1_WriteCompare(163);
    PWM_2_WriteCompare(168);
    
}

void turn_left() {
    
    // Left turning.
    PWM_1_WriteCompare(163);
    PWM_2_WriteCompare(175);

}

void turn_left_sharp() {
    
    // Left turning.
    PWM_1_WriteCompare(73);
    PWM_2_WriteCompare(170);

}

void turn_right() {
    
    // Right turning.
    PWM_2_WriteCompare(168);
    PWM_1_WriteCompare(168);
    
}

void turn_right_sharp() {
    
    // Left turning.
    PWM_1_WriteCompare(163);
    PWM_2_WriteCompare(80);

}

void stop() {
    // Stop both motors.
    PWM_1_WriteCompare(125);
    PWM_2_WriteCompare(125);
    //CyDelay(100);
    
}

/* [] END OF FILE */
