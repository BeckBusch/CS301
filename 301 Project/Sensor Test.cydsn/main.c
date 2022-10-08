/* ========================================
 * main.c
 * ========================================
*/

#include "project.h"
#include "motors.h"
#include "USBUART.h"
#include "decoders.h"
#include <stdio.h>
#include "algorithm.h"
// ADD THIS

#define ON 1
#define OFF 0

#define L 0
#define R 1
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
#define STOPPED 5

// Disable left and right turns at appropriate times.
#define DISABLE_LEFT_TURN 0
#define DISABLE_RIGHT_TURN 1
#define NO_DISABLES 2

// Prototype declarations.
CY_ISR_PROTO(isr_eoc_1);
CY_ISR_PROTO(isr_timer_1);

volatile uint8 channel = 0;
volatile uint16 instructionCursor = 0;

uint32 i = 0;
uint16 turn_count = 0;

uint8 disable_toggle = 0;

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

int main(void){

    // Store the dimensions of the map so we can't go outside of it later on. Hard coding this.
    uint16_t xdim = 19, ydim = 15;
    uint16_t xydim = xdim * ydim;

    // Copy paste the map here - make sure to append backslashes after each line.
    uint16_t array[ydim][xdim];
    char arrayString[] =   "1111111111111111101\
                            1000001000000010001\
                            1110101010111010101\
                            1010000010100000101\
                            1010101110101110101\
                            1000100000100000101\
                            1011111110001110101\
                            1000100000100010101\
                            1110001111101010101\
                            1000100000101000001\
                            1011111110001111101\
                            1000001000101000101\
                            1111101011101010101\
                            0000000010000010001\
                            1111111111111111111";

    uint16_t i = 0;
    uint8_t j, k = 0;

    // Convert the string to an array so we can use it.
    while (arrayString[i] != '\0') {
        if (arrayString[i] == 48) {
            array[j][k] = 0;
            if (k == xdim - 1) {
                k = 0;
                j++;
            } else {
                k++;
            }
        } else if (arrayString[i] == 49) {
            array[j][k] = 1;
            if (k == xdim - 1) {
                k = 0;
                j++;
            } else {
                k++;
            }
        }
        i++;
    }

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
                if (array[i - 1][j] == 0) {
                    // Row above.
                    adjlist[cnode][0] = (i - 1) * xdim + j;
                } if (array[i + 1][j] == 0) {
                    // Row below.
                    adjlist[cnode][1] = (i + 1) * xdim + j;
                } if (array[i][j - 1] == 0) {
                    // Column left.
                    adjlist[cnode][2] = i * xdim + j - 1;
                } if (array[i][j + 1] == 0) {
                    // Column right.
                    adjlist[cnode][3] = i * xdim + j + 1;
                }
            }
        }
    }

    // Source x and y co-ordinates.
    uint16_t sxcord = 1;
    uint16_t sycord = 1;

    // Target x and y co-ordinates.
    uint16_t txcord = 5;
    uint16_t tycord = 13;

    // The offset value - if we are indexing starting at 0, this should be 0, if we are indexing starting at 1, this should be 1 etc.
    uint8_t offset = 0;

    // Calculation for the source and target co-ordinates.
    uint16_t source = ((sycord - offset) * xdim + sxcord - offset);
    uint16_t target = ((tycord - offset) * xdim + txcord - offset);

    uint16_t *finalPath = ASTAR(source, target, adjlist, xdim, ydim, array);
    int8_t *instructionSet = decode(finalPath, xdim, target);
    
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
        
        if (milliVoltReading > maxValues[channel]) {
            maxValues[channel] = milliVoltReading;
        }
        
        if (reset == 1) {
            for (int i = 0; i < 5; i++) {
                pastValues[i] = maxValues[i];
                maxValues[i] = 0;
            }

            if (nextInstruction == L) {
                disable_toggle = DISABLE_RIGHT_TURN;
            } else if (nextInstruction == R) {
                disable_toggle = DISABLE_LEFT_TURN;
            } else if (nextInstruction == NULLDIR) {
                disable_toggle = NO_DISABLES;
            }
            
            if (state == TURNING_LEFT) {
                if (sensor_state[FL] == OFF) {
                    state = EXIT_LEFT;
                }
                turn_left_sharp();
            } else if (state == EXIT_LEFT) {
                if (turn_count < 3) {
                    turn_left();
                    turn_count++;
                } else {
                    turn_count = 0;
                    instructionCursor++;
                    nextInstruction = instructionSet[instructionCursor];
                    state = FORWARD;
                }
            } else if (state == TURNING_RIGHT) {
                if (sensor_state[FR] == OFF) {
                    state = EXIT_RIGHT;
                }
                turn_right_sharp();
            } else if (state == EXIT_RIGHT) {
                if (turn_count < 3) {
                    turn_right();
                    turn_count++;
                } else {
                    turn_count = 0;
                    instructionCursor++;
                    nextInstruction = instructionSet[instructionCursor];
                    state = FORWARD;
                }
            } else if (state == FORWARD) {
                if (sensor_state[FL] == OFF && sensor_state[FR] == OFF && sensor_state[CL] == OFF && sensor_state[CR] == OFF && sensor_state[BC] == OFF) {
                    state = STOPPED;
                } else if (sensor_state[FL] == OFF) {
                    turn_left();
                    // disable_toggle = DISABLE_RIGHT_TURN;
                } else if (sensor_state[FR] == OFF) {
                    turn_right();
                    // disable_toggle = DISABLE_LEFT_TURN;
                } else if (sensor_state[FL] == ON && sensor_state[FR] == ON && sensor_state[CL] == OFF && sensor_state[CR] == ON && disable_toggle != DISABLE_LEFT_TURN) {
                    state = TURNING_LEFT;
                    turn_left();
                } else if (sensor_state[FL] == ON && sensor_state[FR] == ON && sensor_state[CL] == ON && sensor_state[CR] == OFF && disable_toggle != DISABLE_RIGHT_TURN) {
                    state = TURNING_RIGHT;
                    turn_right();
                } else {
                    move_forward();
                }
            } else if (state == STOPPED) {
                stop();
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
        
        // Implementation of a state machine    to control the robot.

    }
    return 0;
               
}
    

/* [] END OF FILE */
