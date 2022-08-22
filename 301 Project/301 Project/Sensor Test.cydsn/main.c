#include "project.h"

CY_ISR_PROTO(isr_eoc_1);
//CY_ISR_PROTO(isr_eoc_2);

uint8 flag = 0;

CY_ISR(isr_eoc_1){
    flag = 1;
    LED_3_Write(1);
}

int main(void){
    CyGlobalIntEnable;
    isr_eoc_1_StartEx(isr_eoc_1);
    isr_eoc_1_Enable();
    
    
    
    ADC_SAR_1_Start();
    
    ADC_SAR_1_StartConvert();
    
    for(;;){
        if (flag == 1){
            flag = 0;
            
            ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT);
            
            uint16 number = ADC_SAR_1_GetResult16();
            float voltsConvert = ADC_SAR_1_CountsTo_Volts(number);
            
            if (voltsConvert >= 1){
                LED_1_Write(1);
            } else {
                LED_1_Write(0);
            }
            
            if (voltsConvert >= 2) {
                LED_2_Write(1);
            } else {
                LED_2_Write(0);
            }
        }
    }
}

/* [] END OF FILE */
