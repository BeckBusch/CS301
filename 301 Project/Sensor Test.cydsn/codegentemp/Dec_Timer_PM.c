/*******************************************************************************
* File Name: Dec_Timer_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Dec_Timer.h"

static Dec_Timer_backupStruct Dec_Timer_backup;


/*******************************************************************************
* Function Name: Dec_Timer_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Dec_Timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Dec_Timer_SaveConfig(void) 
{
    #if (!Dec_Timer_UsingFixedFunction)
        Dec_Timer_backup.TimerUdb = Dec_Timer_ReadCounter();
        Dec_Timer_backup.InterruptMaskValue = Dec_Timer_STATUS_MASK;
        #if (Dec_Timer_UsingHWCaptureCounter)
            Dec_Timer_backup.TimerCaptureCounter = Dec_Timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Dec_Timer_UDB_CONTROL_REG_REMOVED)
            Dec_Timer_backup.TimerControlRegister = Dec_Timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Dec_Timer_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Dec_Timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Dec_Timer_RestoreConfig(void) 
{   
    #if (!Dec_Timer_UsingFixedFunction)

        Dec_Timer_WriteCounter(Dec_Timer_backup.TimerUdb);
        Dec_Timer_STATUS_MASK =Dec_Timer_backup.InterruptMaskValue;
        #if (Dec_Timer_UsingHWCaptureCounter)
            Dec_Timer_SetCaptureCount(Dec_Timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Dec_Timer_UDB_CONTROL_REG_REMOVED)
            Dec_Timer_WriteControlRegister(Dec_Timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Dec_Timer_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Dec_Timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Dec_Timer_Sleep(void) 
{
    #if(!Dec_Timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Dec_Timer_CTRL_ENABLE == (Dec_Timer_CONTROL & Dec_Timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Dec_Timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Dec_Timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Dec_Timer_Stop();
    Dec_Timer_SaveConfig();
}


/*******************************************************************************
* Function Name: Dec_Timer_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Dec_Timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Dec_Timer_Wakeup(void) 
{
    Dec_Timer_RestoreConfig();
    #if(!Dec_Timer_UDB_CONTROL_REG_REMOVED)
        if(Dec_Timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Dec_Timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
