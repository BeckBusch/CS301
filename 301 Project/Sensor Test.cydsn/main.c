#include "project.h"

CY_ISR_PROTO(isr_eoc_1);
CY_ISR_PROTO(isr_eoc_2);
CY_ISR_PROTO(isr_timer);

uint8 flagA = 0;
uint8 flagB = 0;

uint16 count = 0;

CY_ISR(isr_eoc_1){
    // Flag which is set upon completion of an ADC conversion.
    flagA = 1;
}

//CY_ISR(isr_timer){
  //  ADC_SAR_1_StartConvert();
    //ADC_SAR_2_StartConvert();
    //if (count <= 1000) { 
      //  count = count + 1;
    //} else {
      //  count = 0;
      //  LED_1_Write(!LED_1_Read());
    //}

//}

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
    //ADC_SAR_2_Start();
    //Timer_1_Start();
    LED_1_Write(1); 
        
    while(1) {
        
        // If the flag is set, enter the loop else keep checking.
        if (flagA == 1) {
            flagA = 0;
            
            // If the conversion result is ready, put it into a variable and convert it into millivolts.
            ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT);
            uint16 ADCResult1 = ADC_SAR_1_GetResult16();
            uint16 milliVoltReading = ADC_SAR_1_CountsTo_mVolts(ADCResult1);
                        
            // If the millivolt reading is above the required threshold, write the LED high, else write low.
            if (milliVoltReading >= 800) {
                LED_1_Write(1);
            } else {
                LED_1_Write(0);
            }
        }
        
    }
    
    
    //for(;;){ // main loop
        //if (flagA==1){ // if both flags are set
            //flagA = 0; // reset flags
            //flagB = 0;
            
            //ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT); // make sure both adcs are done
            //ADC_SAR_2_IsEndConversion(ADC_SAR_2_WAIT_FOR_RESULT);
            
            //uint16 numberA = ADC_SAR_1_GetResult16(); // get both results
            //uint16 numberB = ADC_SAR_2_GetResult16();
            
            //float voltsConvertA = ADC_SAR_1_CountsTo_mVolts(numberA); // convert to mv
            //float voltsConvertB = ADC_SAR_2_CountsTo_mVolts(numberB);
            
            //     ##### These are the voltage checks #####
            //if (voltsConvertA >= 1000){ // if pin 2.2 is smaller than 37mV, turn led on, else off
              //  LED_1_Write(1);
            //} else {
              //  LED_1_Write(0);
            //}
            
            //if (voltsConvertB <= 1000){ // if pin 2.0 is smaller than 37mV, turn led on, else off
              //  LED_2_Write(1);
            //} else {
                //LED_2_Write(0);
            //}
        //}
    //}
}

/* [] END OF FILE */
