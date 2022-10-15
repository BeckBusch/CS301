/* ========================================
 * motors.c
 * ========================================
*/

#include "project.h"
#include "motors.h"
#include "decoders.h"

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
    speedControlFlag = 1;
    // Forward movement.
    PWM_1_Start();
    PWM_1_WriteCompare(compareValueL + 125);  
    PWM_2_Start();
    PWM_2_WriteCompare(compareValueR + 125);
    
}

void turn_left() {
    
    // Left turning.
    PWM_1_WriteCompare(125);
    PWM_2_Start();
    PWM_2_WriteCompare(160);

}

void turn_right() {
    
    // Right turning.
    PWM_2_WriteCompare(125);
    PWM_1_Start();
    PWM_1_WriteCompare(153);
    
}

void stop() {
    speedControlFlag = 0;
    // Stop both motors.
    PWM_1_WriteCompare(125);
    PWM_2_WriteCompare(125);
    //CyDelay(100);
    
}

/* [] END OF FILE */
