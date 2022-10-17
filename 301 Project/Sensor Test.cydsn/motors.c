/* ========================================
 * motors.c
 * ========================================
*/

#include "project.h"
#include "motors.h"

#define normalLeft 163
#define normalRight 168

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
    
    // Forward movement
    PWM_1_WriteCompare(normalLeft);
    PWM_2_WriteCompare(normalRight);
    
}

void turn_left() {
    
    // Left turning.
    PWM_1_WriteCompare(normalLeft-8); //normalLeft - 8
    PWM_2_WriteCompare(normalRight);

}

void turn_right() {
    
    // Right turning.
    PWM_2_WriteCompare(normalRight-7);//7
    PWM_1_WriteCompare(normalRight);
    
}

void stop() {
    
    // Stop both motors.
    PWM_1_WriteCompare(125);
    PWM_2_WriteCompare(125);
    
}

/* [] END OF FILE */
