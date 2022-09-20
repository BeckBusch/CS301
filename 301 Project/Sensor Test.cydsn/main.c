#include "project.h"
#include "motors.c"

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
CY_ISR_PROTO(isr_eoc_1);
CY_ISR_PROTO(isr_eoc_2);
CY_ISR_PROTO(isr_timer);

uint8 channel = 0;

uint16 ADCResult;
uint16 milliVoltReading;
uint16 count = 0;
uint16 state = FORWARD;

uint8 sensor_state[5];

CY_ISR(isr_eoc_1){
    // Change the ADC channel each time an eoc interrupt is generated.
    if (channel == 4) {
        channel = 0;
    } else {
        channel++;
    }
}

int main(void){
    
    // Enable global interrupts as well as start and enable the isr.
    CyGlobalIntEnable;
    isr_eoc_1_StartEx(isr_eoc_1);
    //isr_eoc_1_Enable();
    //isr_eoc_2_StartEx(isr_eoc_2);
    //isr_eoc_2_Enable();
    //isr_timer_StartEx(isr_timer);
    //isr_timer_Enable();
    
    // Start the ADC and begin conversions (in free running mode so will continue to convert).
    ADC_SAR_Seq_1_Start();
    ADC_SAR_Seq_1_StartConvert();
    
    PWM_1_WritePeriod(250);
    PWM_1_Start();
    PWM_1_WriteCompare(80);
    
    PWM_2_WritePeriod(250);
    PWM_2_Start();
    PWM_2_WriteCompare(80);
    //Timer_1_Start();
        
    while(1) {
           
        // If the conversion result is ready, put it into a variable and convert it into millivolts.
        ADC_SAR_Seq_1_IsEndConversion(ADC_SAR_Seq_1_WAIT_FOR_RESULT);
        ADCResult = ADC_SAR_Seq_1_GetResult16(channel);
        milliVoltReading = ADC_SAR_Seq_1_CountsTo_mVolts(ADCResult);
        
        // If the milliVolt reading is above the required threshold, perform the requested operation depending on the channel.
        if (milliVoltReading >= 800) {
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
        if (state == FORWARD) {
            if (sensor_state[FL] == OFF) {
                state = TURNING_LEFT;
            } else if (sensor_state[FR] == OFF) {
                state = TURNING_RIGHT;
            } else if (sensor_state[BC] == OFF) {
                state = CORRECTION;
            }
            move_forward();
        } else if (state == TURNING_LEFT) {
            while(sensor_state[CL] == ON) {
                turn_left();
            }
            while(sensor_state[BC] == ON) {
                move_forward();
            }
            while(sensor_state[FL] == OFF) {
                turn_left();
            }
            while(sensor_state[FL] == ON) {
                turn_left();
            }
            state = FORWARD;
        } else if (state == TURNING_RIGHT) {
            while(sensor_state[CR] == ON) {
                turn_right();
            }
            while(sensor_state[BC] == ON) {
                move_forward();
            }
            while(sensor_state[FR] == OFF) {
                turn_right();
            }
            while(sensor_state[FR] == ON) {
                turn_right();
            }
            state = FORWARD;
        } else if (state == CORRECTION) {
            while(sensor_state[FL] == ON && sensor_state[FR] == ON) {
                move_forward();
            }
            if (sensor_state[FL] == OFF) {
                while(sensor_state[BC] == ON) {
                    turn_left();
                }
                state = FORWARD;
            } else if (sensor_state[FR] == OFF) {
                while(sensor_state[BC] == ON) {
                    turn_right();
                }
                state = FORWARD;
            }
            } else {
                move_forward();
            }
        }
    return 0;
    
}

/* [] END OF FILE */
