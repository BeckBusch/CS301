/* ========================================
 * main.c
 * ========================================
*/

#include <stdio.h>

#include "project.h"
#include "map.h"

#include "motors.h"
#include "USBUART.h"
#include "decoders.h"
#include "pathfinding.h"

#define ON 1
#define OFF 0

#define L 0
#define R 1
#define J 2
#define T180 3
#define P 4
#define NULLDIR -1

// FL, FR, CL, CR, BC = front left, front right, center left, center right, back center respectively.
#define FL 0
#define FR 1
#define CL 2
#define CR 3
#define BC 4

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3
#define UNINITIALISED 4

// State values.
#define FORWARD 0
#define TURNING_LEFT 1
#define TURNING_RIGHT 2
#define TURNING_ENABLE 3
#define EXIT_LEFT 4
#define EXIT_RIGHT 5
#define STOPPED 6
#define STOPPED_LEFT 19
#define STOPPED_RIGHT 25
#define ENTER_LEFT 7
#define ENTER_RIGHT 50

// Enable left and right turning at appropriate times.
#define LEFT_ENABLE 0
#define LEFT_DISABLE 1
#define RIGHT_ENABLE 2
#define RIGHT_DISABLE 3

// Prototype declarations.
CY_ISR_PROTO(isr_eoc_1);
CY_ISR_PROTO(isr_timer_1);

volatile uint8 channel = 0;
// Start the cursor at 1 because we need to make sure that 0 is reserved for the distance after the last turn.
uint16 instructionCursor = 0;

extern uint8 adjustOff;

int8_t finalDistances[5];
int8_t firstDirections[5];
int8_t finalDirections[5];

uint16_t sourceArray[5];
uint16_t targetArray[5];

uint16 stopDuration = 0;

// Move these inside main???? (volatile may need to be kept outside, among others potentially)
uint32 i = 0;
uint16 turn_count = 0;

uint8 left_en_toggle = LEFT_ENABLE;
uint8 right_en_toggle = RIGHT_ENABLE;

uint16 ADCResult;
uint16 milliVoltReading;
uint8 state = FORWARD;
volatile uint8 reset = 0;

uint16 maxValues[5] = { 0 };
uint16 pastValues[5] = { 0 };
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
    // init debugging led
    led_1_Write(1);

    //int8_t instructionSet[9] = {L, L, J, R, J, L, R, L, L};

    // Write algorithimdebugging led high.
    led_Write(1);

    // ALGORITHM CODE BEGINS HERE!!!
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
    
    sourceArray[0] = 1;
    sourceArray[1] = 1;
    sourceArray[2] = 1;
    sourceArray[3] = 1;
    sourceArray[4] = 1;
    
    targetArray[0] = 1;
    targetArray[1] = 1;
    targetArray[2] = 1;
    targetArray[3] = 1;
    targetArray[4] = 1;

    // The below commented out code is for single values only
    //// Source x and y co-ordinates.
    //uint16_t sxcord = 1;
    //uint16_t sycord = 1;

    //// Target x and y co-ordinates.
    //uint16_t txcord = 16;
    //uint16_t tycord = 13;

    //// The offset value - if we are indexing starting at 0, this should be 0, if we are indexing starting at 1, this should be 1 etc.
    //uint16_t offset = 0;

    //// Calculation for the source and target co-ordinates.
    //uint16_t source = ((sycord - offset) * xdim + sxcord - offset);
    //uint16_t target = ((tycord - offset) * xdim + txcord - offset);
    
    // Initialise return arrays. instructionSet is 2 greater to avoid indexing errors.
    uint16_t finalPath[xydim];
    int8_t instructionSet[xydim + 3];
    
    // Set all the values for the instruction set to be UNINITIALISED.
    for (uint16_t u = 0; u < xydim + 3; u++) {
        instructionSet[u] = UNINITIALISED;
    }
    
    // We need to allocate sufficient space.
    int8_t instructionArray[5 * xydim];
    
    // Set all the values for the instruction array to be UNINITIALISED.
    for (uint16_t u = 0; u < 5 * xydim; u++) {
        instructionArray[u] = UNINITIALISED;
    }
    
    // k is used as an index for the final instruction array. Only incremented when there is a valid (not UNINITIALISED instruction in the corresponding instruction set).
    uint16_t k = 0;
    
    // For each path that needs finding, solve it and store it in a final array of values.
    for (uint8_t i = 0; i < 5; i++) {
        ASTAR(finalPath, sourceArray[i], targetArray[i], adjlist, xdim, ydim);  
        decode(instructionSet, finalPath, adjlist, xdim, targetArray[i]);
        
        // Append the final distances and directions for the current path to the final arrays.
        finalDistances[i] = instructionSet[0];
        firstDirections[i] = instructionSet[1];
        finalDirections[i] = instructionSet[2];
            
        // Reset the buffer arrays passed in to the algorithm and decoder functions.
        for (uint16_t j = 0; j < xydim; j++) {
            finalPath[j] = 0;

            if (instructionSet[j + 3] != UNINITIALISED) {
                // Fill the final instruction array with values from the current instruction set.
                instructionArray[k] = instructionSet[j + 3];
                k++;
            }
            
            // Reset the instruction to UNINITIALISED for this value.
            instructionSet[j + 3] = UNINITIALISED;
            
        }
        
        // Append the additional distance required to reach the food to the end of the current path.
        for (uint8_t m = 0; m < finalDistances[i]; m++) {
            instructionArray[k] = P;
            k++;
        }
        
        // If the current iteration is not for the final path.
        if (i < 5) {
            // If the first direction for the next path is not the same as the final direction for the current path, then we need to change the current orientation of the robot before moving on.
            if (finalDirections[i] != firstDirections[i + 1]) {
                
            // Append an additional direction if we need to change after reaching the food item.
            switch (firstDirections[i + 1]) 
            {
                case NORTH:
                    if (finalDirections[i] == EAST) {
                        instructionArray[k] = L;
                    } else if (finalDirections[i] == WEST) {
                        instructionArray[k] = R;
                    } else if (finalDirections[i] == SOUTH) {
                        instructionArray[k] = T180;
                    }
                    break;

                case EAST:
                    if (finalDirections[i] == NORTH) {
                        instructionArray[k] = R;
                    } else if (finalDirections[i] == SOUTH) {
                        instructionArray[k] = L;
                    } else if (finalDirections[i] == WEST) {
                        instructionArray[k] = T180;
                    }
                    break;

                case SOUTH:
                    if (finalDirections[i] == EAST) {
                        instructionArray[k] = R;
                    } else if (finalDirections[i] == WEST) {
                        instructionArray[k] = L;
                    } else if (finalDirections[i] == NORTH) {
                        instructionArray[k] = T180;
                    }
                    break;

                case WEST:
                    if (finalDirections[i] == NORTH) {
                        instructionArray[k] = L;
                    } else if (finalDirections[i] == SOUTH) {
                        instructionArray[k] = R;
                    } else if (finalDirections[i] == EAST) {
                        instructionArray[k] = T180;
                    }
                    break;

                default:
                    break;
            }
            k++;
            
            }
                
        }
    }
    
    // ALGORITHM CODE ENDS HERE!!!
    // Write the algorithim debugging led low.
    led_Write(0);

    // Enable global interrupts as well as start and enable the isr.
    CyGlobalIntEnable;
    isr_eoc_1_StartEx(isr_eoc_1);
    isr_timer_1_StartEx(isr_timer_1);
    ADC_SAR_Seq_1_IRQ_Enable();

    decoderInit();
    PWM_1_Start();
    PWM_2_Start();
    Timer_1_Start();

    // Start the ADC and begin conversions (in free running mode so will continue to convert).
    ADC_SAR_Seq_1_Start();
    ADC_SAR_Seq_1_StartConvert();

    // Initialise sensor states.
    for (int i = 0; i < 5; i++) {
        sensor_state[i] = ON;
    }
    
    int8_t nextInstruction;
    nextInstruction = instructionArray[instructionCursor];
    
    // init debugging led low
    led_1_Write(0);
    //reset = 0;
    //move_forward();
    //move_forward();
    //while(adjustOff != 5) {
    //    adjustSpeed();
    //}
    //volatile uint16 count = 0;
    //while(count < 40) {
    //    if (reset == 1) {
    //        count++;
    //        reset = 0;
    //    }
    //}
    //led_Write(1);  
    //while(1) {
    //    adjustSpeed();            
    //}
    
    while (1) {
        
        // Call to adjust the speed - it only executes if the relevant interrupt has set a flag high, otherwise it just returns.
        //adjustSpeed();
        
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

            } else if (nextInstruction == R) {
                // For right turning.
                left_en_toggle = LEFT_DISABLE;
                right_en_toggle = RIGHT_ENABLE;

            } else if (nextInstruction == J) {
                // For junctions.
                left_en_toggle = LEFT_DISABLE;
                right_en_toggle = RIGHT_DISABLE;

            } else if (nextInstruction == NULLDIR) {
                // For uninitialised or reset.
                left_en_toggle = LEFT_ENABLE;
                right_en_toggle = RIGHT_ENABLE;

            }

            if (state == TURNING_ENABLE) {
                // If we have passed the junction, return to the forward state.
                if (sensor_state[CL] == ON && sensor_state[CR] == ON) {
                    // Go to the next movement instruction.
                    instructionCursor++;
                    nextInstruction = instructionSet[instructionCursor];
                    // Toggle state change led
                    led_2_Write(!led_2_Read());
                    state = FORWARD;
                }
                move_forward();
            } else if (state == ENTER_LEFT) {
                led_2_Write(!led_2_Read());
                abs_right_spot_turn();
                state = STOPPED_LEFT;
            } else if (state == TURNING_LEFT) {
                if (sensor_state[BC] == OFF) {
                    //left_decode_turn();
                    // Go to the next movement instruction.
                    instructionCursor++;
                    nextInstruction = instructionSet[instructionCursor];
                    state = FORWARD;
                }
                turn_left_sharp();
                //abs_left_spot_turn();
            } else if (state == ENTER_RIGHT) {
                led_2_Write(!led_2_Read());
                abs_right_spot_turn();
                state = STOPPED_RIGHT;
            } else if (state == TURNING_RIGHT) {
                if (sensor_state[BC] == OFF) {
                    //right_decode_turn();
                    // Go to the next movement instruction.
                    instructionCursor++;
                    nextInstruction = instructionSet[instructionCursor];
                    state = FORWARD;
                }
                turn_right_sharp();
            } else if (state == FORWARD) {
                // Default state of forward movement.
                if (sensor_state[FL] == OFF && sensor_state[FR] == OFF && sensor_state[CL] == OFF && sensor_state[CR] == OFF && sensor_state[BC] == OFF) {
                    // If a shadow is hovered over the robot (all sensors), it will stop moving.
                    state = STOPPED;
                } else if (sensor_state[CL] == ON && sensor_state[CR] == ON && ((sensor_state[FL] == ON && sensor_state[FR] == OFF) || (sensor_state[FL] == OFF && sensor_state[FR] == ON)) && (left_en_toggle == LEFT_DISABLE && right_en_toggle == RIGHT_DISABLE)) {
                    // Pass through the junction.
                    state = TURNING_ENABLE;
                } else if (sensor_state[FL] == OFF && sensor_state[FR] == ON && left_en_toggle == LEFT_ENABLE) {
                    // Left turning.
                    state = ENTER_LEFT;
                } else if (sensor_state[FL] == ON && sensor_state[FR] == OFF && right_en_toggle == RIGHT_ENABLE) {
                    // Right turning.
                    state = ENTER_RIGHT;
                } else if (sensor_state[CL] == OFF) {
                    // Correct to the left.
                    turn_left();
                } else if (sensor_state[CR] == OFF) {
                    // Correct to the right.
                    turn_right();
                } else {
                    // If no other condition has been met, continue to move forward.
                    move_forward();
                }
            } else if (state == STOPPED) {
                // Stop moving.
                // If all the light sensors are on, start moving again.
                if (sensor_state[FL] == ON || sensor_state[FR] == ON || sensor_state[CL] == ON || sensor_state[CR] == ON || sensor_state[BC] == ON) {
                    state = FORWARD;
                }
            } else if (state == STOPPED_LEFT) {
                stop();
                stopDuration++;
                if (stopDuration > 100) {
                    stopDuration = 0;
                    state = TURNING_LEFT;
                }
            } else if (state == STOPPED_RIGHT) {
                stop();
                stopDuration++;
                if (stopDuration > 100) {
                    stopDuration = 0;
                    state = TURNING_RIGHT;
                }
            }

            reset = 0;
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
