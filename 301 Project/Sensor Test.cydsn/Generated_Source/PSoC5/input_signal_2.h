/*******************************************************************************
* File Name: input_signal_2.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_input_signal_2_H) /* Pins input_signal_2_H */
#define CY_PINS_input_signal_2_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "input_signal_2_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 input_signal_2__PORT == 15 && ((input_signal_2__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    input_signal_2_Write(uint8 value);
void    input_signal_2_SetDriveMode(uint8 mode);
uint8   input_signal_2_ReadDataReg(void);
uint8   input_signal_2_Read(void);
void    input_signal_2_SetInterruptMode(uint16 position, uint16 mode);
uint8   input_signal_2_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the input_signal_2_SetDriveMode() function.
     *  @{
     */
        #define input_signal_2_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define input_signal_2_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define input_signal_2_DM_RES_UP          PIN_DM_RES_UP
        #define input_signal_2_DM_RES_DWN         PIN_DM_RES_DWN
        #define input_signal_2_DM_OD_LO           PIN_DM_OD_LO
        #define input_signal_2_DM_OD_HI           PIN_DM_OD_HI
        #define input_signal_2_DM_STRONG          PIN_DM_STRONG
        #define input_signal_2_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define input_signal_2_MASK               input_signal_2__MASK
#define input_signal_2_SHIFT              input_signal_2__SHIFT
#define input_signal_2_WIDTH              1u

/* Interrupt constants */
#if defined(input_signal_2__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in input_signal_2_SetInterruptMode() function.
     *  @{
     */
        #define input_signal_2_INTR_NONE      (uint16)(0x0000u)
        #define input_signal_2_INTR_RISING    (uint16)(0x0001u)
        #define input_signal_2_INTR_FALLING   (uint16)(0x0002u)
        #define input_signal_2_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define input_signal_2_INTR_MASK      (0x01u) 
#endif /* (input_signal_2__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define input_signal_2_PS                     (* (reg8 *) input_signal_2__PS)
/* Data Register */
#define input_signal_2_DR                     (* (reg8 *) input_signal_2__DR)
/* Port Number */
#define input_signal_2_PRT_NUM                (* (reg8 *) input_signal_2__PRT) 
/* Connect to Analog Globals */                                                  
#define input_signal_2_AG                     (* (reg8 *) input_signal_2__AG)                       
/* Analog MUX bux enable */
#define input_signal_2_AMUX                   (* (reg8 *) input_signal_2__AMUX) 
/* Bidirectional Enable */                                                        
#define input_signal_2_BIE                    (* (reg8 *) input_signal_2__BIE)
/* Bit-mask for Aliased Register Access */
#define input_signal_2_BIT_MASK               (* (reg8 *) input_signal_2__BIT_MASK)
/* Bypass Enable */
#define input_signal_2_BYP                    (* (reg8 *) input_signal_2__BYP)
/* Port wide control signals */                                                   
#define input_signal_2_CTL                    (* (reg8 *) input_signal_2__CTL)
/* Drive Modes */
#define input_signal_2_DM0                    (* (reg8 *) input_signal_2__DM0) 
#define input_signal_2_DM1                    (* (reg8 *) input_signal_2__DM1)
#define input_signal_2_DM2                    (* (reg8 *) input_signal_2__DM2) 
/* Input Buffer Disable Override */
#define input_signal_2_INP_DIS                (* (reg8 *) input_signal_2__INP_DIS)
/* LCD Common or Segment Drive */
#define input_signal_2_LCD_COM_SEG            (* (reg8 *) input_signal_2__LCD_COM_SEG)
/* Enable Segment LCD */
#define input_signal_2_LCD_EN                 (* (reg8 *) input_signal_2__LCD_EN)
/* Slew Rate Control */
#define input_signal_2_SLW                    (* (reg8 *) input_signal_2__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define input_signal_2_PRTDSI__CAPS_SEL       (* (reg8 *) input_signal_2__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define input_signal_2_PRTDSI__DBL_SYNC_IN    (* (reg8 *) input_signal_2__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define input_signal_2_PRTDSI__OE_SEL0        (* (reg8 *) input_signal_2__PRTDSI__OE_SEL0) 
#define input_signal_2_PRTDSI__OE_SEL1        (* (reg8 *) input_signal_2__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define input_signal_2_PRTDSI__OUT_SEL0       (* (reg8 *) input_signal_2__PRTDSI__OUT_SEL0) 
#define input_signal_2_PRTDSI__OUT_SEL1       (* (reg8 *) input_signal_2__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define input_signal_2_PRTDSI__SYNC_OUT       (* (reg8 *) input_signal_2__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(input_signal_2__SIO_CFG)
    #define input_signal_2_SIO_HYST_EN        (* (reg8 *) input_signal_2__SIO_HYST_EN)
    #define input_signal_2_SIO_REG_HIFREQ     (* (reg8 *) input_signal_2__SIO_REG_HIFREQ)
    #define input_signal_2_SIO_CFG            (* (reg8 *) input_signal_2__SIO_CFG)
    #define input_signal_2_SIO_DIFF           (* (reg8 *) input_signal_2__SIO_DIFF)
#endif /* (input_signal_2__SIO_CFG) */

/* Interrupt Registers */
#if defined(input_signal_2__INTSTAT)
    #define input_signal_2_INTSTAT            (* (reg8 *) input_signal_2__INTSTAT)
    #define input_signal_2_SNAP               (* (reg8 *) input_signal_2__SNAP)
    
	#define input_signal_2_0_INTTYPE_REG 		(* (reg8 *) input_signal_2__0__INTTYPE)
#endif /* (input_signal_2__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_input_signal_2_H */


/* [] END OF FILE */
