/* ========================================
 * main.c
 * ========================================
*/

#include "project.h"
#include "motors.h"
#include "USBUART.h"
#include "decoders.h"
#include <stdio.h>
#include "pathfinding.h"

#include "map.h"

#define ON 1
#define OFF 0

#define L 0
#define R 1
#define J 2
#define NULLDIR -1

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
#define EXIT_LEFT 3
#define EXIT_RIGHT 4
#define TURNING_ENABLE 5
#define STOPPED 6

// Enable left and right turning at appropriate times.
#define LEFT_ENABLE 0
#define LEFT_DISABLE 1
#define RIGHT_ENABLE 2
#define RIGHT_DISABLE 3

// Prototype declarations.
CY_ISR_PROTO(isr_eoc_1);
CY_ISR_PROTO(isr_timer_1);

volatile uint8 channel = 0;
volatile uint16 instructionCursor = 0;

// Move these inside main???? (volatile may need to be kept outside, among others potentially)


uint16 ADCResult;
uint16 milliVoltReading;
uint8 state = FORWARD;
volatile uint8 reset = 0;

uint16 maxValues[5] = {0};
uint16 pastValues[5] = {0};
volatile uint16 count = 0;
uint8 sensor_state[5];

CY_ISR(isr_eoc_1) {
    // Change the ADC channel each time an eoc interrupt is generated.
    if (channel == 4) {
        channel = 0;
    } else {
        channel++;
    }   
}

CY_ISR(isr_timer_1) {
    reset = 1;
}

int main(void) {
    uint32 i = 0;
    uint16 turn_count = 0;

    uint8 left_en_toggle = LEFT_ENABLE;
    uint8 right_en_toggle = RIGHT_ENABLE;

    // Write debugging led high.
    led_Write(1);
    //led_1_Write(1);
    //led_2_Write(1);
    int8_t instructionSet[9] = {R, L, J, R, J, L, R, L, L};
    
    // Enable global interrupts as well as start and enable the isr.
    CyGlobalIntEnable;
    isr_eoc_1_StartEx(isr_eoc_1);
    isr_timer_1_StartEx(isr_timer_1);
    ADC_SAR_Seq_1_IRQ_Enable();
    
    // Start the ADC and begin conversions (in free running mode so will continue to convert).
    ADC_SAR_Seq_1_Start();
    ADC_SAR_Seq_1_StartConvert();
    
    // Initialise sensor states.
    for (int i = 0; i < 5; i++) {
        sensor_state[i] = ON;
    }
    
    PWM_1_Start();
    PWM_2_Start();
    stop();
    Timer_1_Start();
    
    int8_t nextInstruction;
    nextInstruction = instructionSet[instructionCursor];
    
    // Write the debugging led low.
    led_Write(0);
      //move_forward();
    while(1) {
        
        // If the conversion result is ready, put it into a variable and convert it into millivolts.
        ADC_SAR_Seq_1_IsEndConversion(ADC_SAR_Seq_1_WAIT_FOR_RESULT);
        ADCResult = ADC_SAR_Seq_1_GetResult16(channel);
        milliVoltReading = ADC_SAR_Seq_1_CountsTo_mVolts(ADCResult);
        
        // If we read in a value higher than the current maximum for this period, replace the corresponding value in the maxValues array.
        if (milliVoltReading > maxValues[channel]) {
            maxValues[channel] = milliVoltReading;
        }
                           
        if (reset == 1) {
            
            // Fill the pastValues array with the new set of values.
            for (int i = 0; i < 5; i++) {
                pastValues[i] = maxValues[i];
                maxValues[i] = 0;
            }
            
            if (nextInstruction == L) {
                // For left turning.
                left_en_toggle = LEFT_ENABLE;
                right_en_toggle = RIGHT_DISABLE;
                led_Write(!led_Read());
            } else if (nextInstruction == R) {
                // For right turning.
                left_en_toggle = LEFT_DISABLE;
                right_en_toggle = RIGHT_ENABLE;
                led_Write(!led_Read());
            } else if (nextInstruction == J) {
                // For junctions.
                left_en_toggle = LEFT_DISABLE;
                right_en_toggle = RIGHT_DISABLE;
                led_Write(!led_Read());
            } else if (nextInstruction == NULLDIR) {
                // For uninitialised or reset.
                left_en_toggle = LEFT_ENABLE;
                right_en_toggle = RIGHT_ENABLE;
                led_Write(!led_Read());
            }
            
            
            if (state == TURNING_ENABLE) {
                // If we have passed the junction, return to the forward state.
                if (sensor_state[CL] == ON && sensor_state[CR] == ON) {
                    // Go to the next movement instruction.
                    instructionCursor++;
                    nextInstruction = instructionSet[instructionCursor];
                    state = FORWARD;
                }
                move_forward();
            } if (state == TURNING_LEFT) {
                // Exit left if we have done (most of) the 90 degree turn.
                if (sensor_state[FL] == OFF) {
                    state = EXIT_LEFT;
                }
                turn_left_sharp();
            } else if (state == EXIT_LEFT) {
                // Continue turning for a bit so that we don't just drive off the path.
                if (turn_count < 3) {
                    turn_left();
                    turn_count++;
                } else {
                    turn_count = 0;
                    // Go to the next movement instruction.
                    instructionCursor++;
                    nextInstruction = instructionSet[instructionCursor];
                    state = FORWARD;
                }
            } else if (state == TURNING_RIGHT) {
                // Exit right if we have done (most of) the 90 degree turn.
                if (sensor_state[FR] == OFF) {
                    state = EXIT_RIGHT;
                }
                turn_right_sharp();
            } else if (state == EXIT_RIGHT) {
                // Continue turning for a bit so that we don't just drive off the path.
                if (turn_count < 3) {
                    turn_right();
                    turn_count++;
                } else {
                    turn_count = 0;
                    // Go to the next movement instruction.
                    instructionCursor++;
                    nextInstruction = instructionSet[instructionCursor];
                    state = FORWARD;
                }
            } else if (state == FORWARD) {
                // Default state of forward movement.
                if (sensor_state[FL] == OFF && sensor_state[FR] == OFF && sensor_state[CL] == OFF && sensor_state[CR] == OFF && sensor_state[BC] == OFF) {
                    // If a shadow is hovered over the robot (all sensors), it will stop moving.
                    state = STOPPED;
                } else if (sensor_state[FL] == OFF) {
                    // Correct to the left.
                    turn_left();
                } else if (sensor_state[FR] == OFF) {
                    // Correct to the right.
                    turn_right();
                } else if (sensor_state[FL] == ON && sensor_state[FR] == ON && ((sensor_state[CL] == ON && sensor_state[CR] == OFF) || (sensor_state[CL] == OFF && sensor_state[CR] == ON)) && (left_en_toggle == LEFT_DISABLE && right_en_toggle == RIGHT_DISABLE)) {
                    // Pass through the junction.
                    state = TURNING_ENABLE;
                    move_forward();
                } else if (sensor_state[FL] == ON && sensor_state[FR] == ON && sensor_state[CL] == OFF && sensor_state[CR] == ON && left_en_toggle == LEFT_ENABLE) {
                    // Left turning.
                    state = TURNING_LEFT;
                    turn_left();
                } else if (sensor_state[FL] == ON && sensor_state[FR] == ON && sensor_state[CL] == ON && sensor_state[CR] == OFF && right_en_toggle == RIGHT_ENABLE) {
                    // Right turning.
                    state = TURNING_RIGHT;
                    turn_right();
                } else {
                    // If no other condition has been met, continue to move forward.
                    move_forward();
                }
            } else if (state == STOPPED) {
                // Stop moving.
                stop();
                // If all the light sensors are on, start moving again.
                if (sensor_state[FL] == ON || sensor_state[FR] == ON || sensor_state[CL] == ON || sensor_state[CR] == ON || sensor_state[BC] == ON) {
                    state = FORWARD;
                }
            }
            
            reset = 0;   
        }
        
        // If the milliVolt reading is above the required threshold, perform the requested operation depending on the channel.
        if (pastValues[channel] >= 500) {
            // Change the position in the sensor_state array depending on the channel being read.
            if (channel == 0) {
                sensor_state[CL] = ON;
            } else if (channel == 1) {
                sensor_state[CR] = ON;
            } else if (channel == 2) {
                sensor_state[FL] = ON;
            } else if (channel == 3) {
                sensor_state[FR] = ON;
            } else if (channel == 4) {
                sensor_state[BC] = OFF;
            }
        } else {
            if (channel == 0) {
                sensor_state[CL] = OFF;
            } else if (channel == 1) {
                sensor_state[CR] = OFF;
            } else if (channel == 2) {
                sensor_state[FL] = OFF;
            } else if (channel == 3) {
                sensor_state[FR] = OFF;
            } else if (channel == 4) {
                sensor_state[BC] = OFF;
            }
        }
                               
    }
    return 0;
               
}

/* [] END OF FILE */
