/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "project.h"
#include "USBUART.h"


void uart_code(){
    USBUART_1_Start(0,USBUART_1_5V_OPERATION);
    
    //check if device is connected
    while(USBUART_1_GetConfiguration()== 0){}
    
    //check if the device is connnected
}

/* [] END OF FILE */
