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
#define EXIT_LEFT 3
#define EXIT_RIGHT 4
#define CONTINUE_FROM_LEFT 5
#define CONTINUE_FROM_RIGHT 6

// Prototype declarations.
CY_ISR_PROTO(isr_eoc_1);
CY_ISR_PROTO(isr_timer_1);

volatile uint8 channel = 0;

uint32 i = 0;

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
    
    // Enable global interrupts as well as start and enable the isr.
    CyGlobalIntEnable;
    isr_eoc_1_StartEx(isr_eoc_1);
    isr_timer_1_StartEx(isr_timer_1);
    ADC_SAR_Seq_1_IRQ_Enable();
    //isr_eoc_1_Enable();
    //isr_eoc_2_StartEx(isr_eoc_2);
    //isr_eoc_2_Enable();
    //isr_timer_StartEx(isr_timer);
    //isr_timer_Enable();
    
    // Start the ADC and begin conversions (in free running mode so will continue to convert).
    ADC_SAR_Seq_1_Start();
    ADC_SAR_Seq_1_StartConvert();
    
    // Initialise sensor states.
    for (int i = 0; i < 5; i++) {
        sensor_state[i] = ON;
    }
    
    //PWM_1_WritePeriod(250);
    //PWM_1_Start();
    //PWM_1_WriteCompare(83);
                 
    //PWM_2_WritePeriod(250);
    //PWM_2_Start();
    //PWM_2_WriteCompare(90);
    Timer_1_Start();
    //turn_left();
    //stop();
    //turn_right();
        
    while(1) {
        
// If the conversion result is ready, put it into a variable and convert it into millivolts.
        ADC_SAR_Seq_1_IsEndConversion(ADC_SAR_Seq_1_WAIT_FOR_RESULT);
        ADCResult = ADC_SAR_Seq_1_GetResult16(channel);
        milliVoltReading = ADC_SAR_Seq_1_CountsTo_mVolts(ADCResult);
        
        if (milliVoltReading > maxValues[channel]) {
            maxValues[channel] = milliVoltReading;
        }
        
        if (reset == 1) {
            for (int i = 0; i < 5; i++) {
                pastValues[i] = maxValues[i];
                maxValues[i] = 0;
            }
            
            if (state == TURNING_LEFT) {
                if (sensor_state[FL] == OFF) {
                    state = EXIT_LEFT;
                }
                turn_left();
            } else if (state == EXIT_LEFT) {
                if (sensor_state[FR] == OFF) {
                    state = CONTINUE_FROM_LEFT;
                }
                move_forward();
            } else if (state == CONTINUE_FROM_LEFT) {
                if (sensor_state[BC] == OFF) {
                    state = FORWARD;
                }
                turn_right();
                // WHEN THE RIGHT SENSOR IS FIXED UNCOMMENT THE BELOW
            //} else if (state == TURNING_RIGHT) {
            //    if (sensor_state[FR] == OFF) {
            //        state = EXIT_RIGHT;
            //    }
            //    turn_right();
            //} else if (state == EXIT_RIGHT) {
            //    if (sensor_state[FL] == OFF) {
            //        state = CONTINUE_FROM_RIGHT;
            //    }
            //    move_forward();
            //} else if (state == CONTINUE_FROM_RIGHT) {
            //    if (sensor_state[BC] == OFF) {
            //        state = FORWARD;
            //    }
            //    turn_left();
            } else if (state == FORWARD) {
                if (sensor_state[FL] == OFF) {
                    turn_left();
                } else if (sensor_state[FR] == OFF) {
                    turn_right();
                    // WHEN THE RIGHT SENSOR IS FIXED, CHANGE sensor_state[CR] == OFF to sensor_state[CR] == ON
                } else if (sensor_state[FL] == ON && sensor_state[FR] == ON && sensor_state[CL] == OFF && sensor_state[CR] == OFF) {
                    stop();
                    state = TURNING_LEFT;
                    turn_left();
                    // WHEN THE RIGHT SENSOR IS FIXED UNCOMMENT THE BELOW
                //} else if (sensor_state[FL] == ON && sensor_state[FR] == ON && sensor_state[CL] == ON && sensor_state[CR] == OFF) {
                //    stop();
                //    state = TURNING_RIGHT;
                //    turn_right();
                } else {
                    move_forward();
                }
            }
                     
            reset = 0;
            
        }     
        
        // If the milliVolt reading is above the required threshold, perform the requested operation depending on the channel.
        if (pastValues[channel] >= 800) {
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
        
        // Implementation of a state machine to control the robot.

    }
    return 0;
               
}
    

/* [] END OF FILE */
