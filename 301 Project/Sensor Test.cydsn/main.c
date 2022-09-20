#include "project.h"
#include "motors.c"

#define ON 1
#define OFF 0

#define FL 0
#define FR 1
#define CL 2
#define CR 3
#define BC 4

CY_ISR_PROTO(isr_eoc_1);
CY_ISR_PROTO(isr_eoc_2);
CY_ISR_PROTO(isr_timer);

uint8 channel = 0;

uint16 ADCResult;
uint16 milliVoltReading;
uint16 count = 0;

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
    ADC_SAR_1_Start();
    ADC_SAR_1_StartConvert();
    //Timer_1_Start();
        
    while(1) {
           
        // If the conversion result is ready, put it into a variable and convert it into millivolts.
        ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT);
        ADCResult = ADC_SAR_1_GetResult16();
        milliVoltReading = ADC_SAR_1_CountsTo_mVolts(ADCResult);
        
        // If the milliVolt reading is above the required threshold, perform the requested operation depending on the channel.
        if (milliVoltReading >= 800) {
            // If the flag is set, enter the loop else keep checking.
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
    
}

/* [] END OF FILE */
