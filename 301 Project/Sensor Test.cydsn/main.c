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
#include <stdint.h>

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

// Disable left and right turns at appropriate times.
#define DISABLE_LEFT_TURN 0
#define DISABLE_RIGHT_TURN 1
#define DISABLE_ALL 2
#define DISABLE_NONE 3

// Prototype declarations.
CY_ISR_PROTO(isr_eoc_1);
CY_ISR_PROTO(isr_timer_1);

volatile uint8 channel = 0;
volatile uint16 instructionCursor = 0;

// Move these inside main???? (volatile may need to be kept outside, among others potentially)
uint32 i = 0;
uint16 turn_count = 0;

uint8 disable_toggle = DISABLE_NONE;

uint16 ADCResult;
uint16 milliVoltReading;
uint8 state = FORWARD;
volatile uint8_t reset = 0;

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

    // Write debugging led high.
    led_Write(1);
    
    // Store the dimensions of the map so we can't go outside of it later on. Hard coding this.
    uint16_t xdim = 19, ydim = 15;
    uint16_t xydim = xdim * ydim;
    
    // Each zero can only be adjacent to 4 zeroes maximum.
    int16_t adjlist[xydim][4];
    for (uint16_t i = 0; i < xydim; i++) {
        for (uint8_t j = 0; j < 4; j++) {
            adjlist[i][j] = -1;
        }
    }
    uint16_t cnode;

    /* Construct the adjacency list.
       Numbered reading from left to right, top down. */
    for (uint16_t i = 0; i < ydim; i++) {
        for (uint16_t j = 0; j < xdim; j++) {
            // For loops go through rows, cols.
            if (array[i][j] == 0) {
                cnode = i * xdim + j; 
                if (i >= 1) {
                    if (array[i - 1][j] == 0) {
                        // Row above.
                        adjlist[cnode][0] = (i - 1) * xdim + j;
                    }
                } if (i <= ydim - 2) {
                    if (array[i + 1][j] == 0) {
                        // Row below.
                        adjlist[cnode][1] = (i + 1) * xdim + j;
                    }
                } if (j >= 1) {
                    if (array[i][j - 1] == 0) {
                        // Column left.
                        adjlist[cnode][2] = i * xdim + j - 1;
                    }
                } if (j <= xdim - 2) {
                    if (array[i][j + 1] == 0) {
                        // Column right.
                        adjlist[cnode][3] = i * xdim + j + 1;
                    }
                }
            }
        }
    }

    // Source x and y co-ordinates.
    uint16_t sxcord = 1;
    uint16_t sycord = 1;

    // Target x and y co-ordinates.
    uint16_t txcord = 9;//5;
    uint16_t tycord = 9;//13;

    // The offset value - if we are indexing starting at 0, this should be 0, if we are indexing starting at 1, this should be 1 etc.
    uint16_t offset = 0;

    // Calculation for the source and target co-ordinates.
    uint16_t source = ((sycord - offset) * xdim + sxcord - offset);
    uint16_t target = ((tycord - offset) * xdim + txcord - offset);
    
    uint16_t *finalPath = ASTAR(source, target, adjlist, xdim, ydim);
    int8_t *instructionSet = decode(finalPath, adjlist, xdim, target);
    
    // Write the debugging led low.
    led_Write(0);
    
    // Enable global interrupts as well as start and enable the isr.
    CyGlobalIntEnable;
    isr_eoc_1_StartEx(isr_eoc_1);
    isr_timer_1_StartEx(isr_timer_1);
    ADC_SAR_Seq_1_IRQ_Enable();
    
    // Init the decoders and related tools
    decoderInit();
    
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
    
    move_forward();
    //turn_left();
    //stop();
    //turn_right();

    int8_t nextInstruction = instructionSet[instructionCursor];
    
    while(1) {
        
        // If the conversion result is ready, put it into a variable and convert it into millivolts.
        ADC_SAR_Seq_1_IsEndConversion(ADC_SAR_Seq_1_WAIT_FOR_RESULT);
        ADCResult = ADC_SAR_Seq_1_GetResult16(channel);
        milliVoltReading = ADC_SAR_Seq_1_CountsTo_mVolts(ADCResult);

        /* disabled uart code
        char send[100];
        sprintf(send,"%d\r\n",milliVoltReading);
        USBUART_1_PutString(send);
        
        CyDelay(100);
        */
        
        // If we read in a value higher than the current maximum for this period, replace the corresponding value in the maxValues array.
        if (milliVoltReading > maxValues[channel]) {
            maxValues[channel] = milliVoltReading;
        }
        //led_Write(1);
        
        if (reset == 1) {
            reset = 0;
            if (sensor_state[FR] == ON) { led_Write(!led_Read()); }
            // Fill the pastValues array with the new set of values.
            for (int i = 0; i < 5; i++) {
                pastValues[i] = maxValues[i];
                maxValues[i] = 0;
            }

            if (nextInstruction == L) {
                // For left turning.
                disable_toggle = DISABLE_RIGHT_TURN;
            } else if (nextInstruction == R) {
                // For right turning.
                disable_toggle = DISABLE_LEFT_TURN;
            } else if (nextInstruction == J) {
                // For junctions.
                disable_toggle = DISABLE_ALL;
            } else if (nextInstruction == NULLDIR) {
                // For uninitialised or reset.
                disable_toggle = DISABLE_NONE;
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
            } else if (state == TURNING_LEFT) {
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
                } else if (sensor_state[FL] == ON && sensor_state[FR] == ON && ((sensor_state[CL] == ON && sensor_state[CR] == OFF) || (sensor_state[CL] == OFF && sensor_state[CR] == ON)) && disable_toggle == DISABLE_ALL) {
                    // Pass through the junction.
                    state = TURNING_ENABLE;
                    move_forward();
                } else if (sensor_state[FL] == ON && sensor_state[FR] == ON && sensor_state[CL] == OFF && sensor_state[CR] == ON && disable_toggle != DISABLE_LEFT_TURN) {
                    // Left turning.
                    state = TURNING_LEFT;
                    turn_left();
                } else if (sensor_state[FL] == ON && sensor_state[FR] == ON && sensor_state[CL] == ON && sensor_state[CR] == OFF && disable_toggle != DISABLE_RIGHT_TURN) {
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
            //reset = 0;
        }   
        
        // If the milliVolt reading is above the required threshold, perform the requested operation depending on the channel.
        if (pastValues[channel] >= 500) {
            // Change the position in the sensor_state array depending on the channel being read.
            if (channel == 0) {
                sensor_state[FL] = ON;
            } else if (channel == 1) {
                sensor_state[FR] = ON;
            } else if (channel == 2) {
                sensor_state[CL] = ON;
            } else if (channel == 3) {
                sensor_state[CR] = ON;
            } else if (channel == 4) {
                sensor_state[BC] = ON;
            }
        } else {
            if (channel == 0) {
                sensor_state[FL] = OFF;
            } else if (channel == 1) {
                sensor_state[FR] = OFF;
            } else if (channel == 2) {
                sensor_state[CL] = OFF;
            } else if (channel == 3) {
                sensor_state[CR] = OFF;
            } else if (channel == 4) {
                sensor_state[BC] = OFF;
            }
        }
    }
                
    return 0;
               
}

/* [] END OF FILE */
