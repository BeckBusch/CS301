/*******************************************************************************
* File Name: L_ENCD.h  
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

#if !defined(CY_PINS_L_ENCD_H) /* Pins L_ENCD_H */
#define CY_PINS_L_ENCD_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "L_ENCD_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 L_ENCD__PORT == 15 && ((L_ENCD__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    L_ENCD_Write(uint8 value);
void    L_ENCD_SetDriveMode(uint8 mode);
uint8   L_ENCD_ReadDataReg(void);
uint8   L_ENCD_Read(void);
void    L_ENCD_SetInterruptMode(uint16 position, uint16 mode);
uint8   L_ENCD_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the L_ENCD_SetDriveMode() function.
     *  @{
     */
        #define L_ENCD_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define L_ENCD_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define L_ENCD_DM_RES_UP          PIN_DM_RES_UP
        #define L_ENCD_DM_RES_DWN         PIN_DM_RES_DWN
        #define L_ENCD_DM_OD_LO           PIN_DM_OD_LO
        #define L_ENCD_DM_OD_HI           PIN_DM_OD_HI
        #define L_ENCD_DM_STRONG          PIN_DM_STRONG
        #define L_ENCD_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define L_ENCD_MASK               L_ENCD__MASK
#define L_ENCD_SHIFT              L_ENCD__SHIFT
#define L_ENCD_WIDTH              1u

/* Interrupt constants */
#if defined(L_ENCD__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in L_ENCD_SetInterruptMode() function.
     *  @{
     */
        #define L_ENCD_INTR_NONE      (uint16)(0x0000u)
        #define L_ENCD_INTR_RISING    (uint16)(0x0001u)
        #define L_ENCD_INTR_FALLING   (uint16)(0x0002u)
        #define L_ENCD_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define L_ENCD_INTR_MASK      (0x01u) 
#endif /* (L_ENCD__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define L_ENCD_PS                     (* (reg8 *) L_ENCD__PS)
/* Data Register */
#define L_ENCD_DR                     (* (reg8 *) L_ENCD__DR)
/* Port Number */
#define L_ENCD_PRT_NUM                (* (reg8 *) L_ENCD__PRT) 
/* Connect to Analog Globals */                                                  
#define L_ENCD_AG                     (* (reg8 *) L_ENCD__AG)                       
/* Analog MUX bux enable */
#define L_ENCD_AMUX                   (* (reg8 *) L_ENCD__AMUX) 
/* Bidirectional Enable */                                                        
#define L_ENCD_BIE                    (* (reg8 *) L_ENCD__BIE)
/* Bit-mask for Aliased Register Access */
#define L_ENCD_BIT_MASK               (* (reg8 *) L_ENCD__BIT_MASK)
/* Bypass Enable */
#define L_ENCD_BYP                    (* (reg8 *) L_ENCD__BYP)
/* Port wide control signals */                                                   
#define L_ENCD_CTL                    (* (reg8 *) L_ENCD__CTL)
/* Drive Modes */
#define L_ENCD_DM0                    (* (reg8 *) L_ENCD__DM0) 
#define L_ENCD_DM1                    (* (reg8 *) L_ENCD__DM1)
#define L_ENCD_DM2                    (* (reg8 *) L_ENCD__DM2) 
/* Input Buffer Disable Override */
#define L_ENCD_INP_DIS                (* (reg8 *) L_ENCD__INP_DIS)
/* LCD Common or Segment Drive */
#define L_ENCD_LCD_COM_SEG            (* (reg8 *) L_ENCD__LCD_COM_SEG)
/* Enable Segment LCD */
#define L_ENCD_LCD_EN                 (* (reg8 *) L_ENCD__LCD_EN)
/* Slew Rate Control */
#define L_ENCD_SLW                    (* (reg8 *) L_ENCD__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define L_ENCD_PRTDSI__CAPS_SEL       (* (reg8 *) L_ENCD__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define L_ENCD_PRTDSI__DBL_SYNC_IN    (* (reg8 *) L_ENCD__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define L_ENCD_PRTDSI__OE_SEL0        (* (reg8 *) L_ENCD__PRTDSI__OE_SEL0) 
#define L_ENCD_PRTDSI__OE_SEL1        (* (reg8 *) L_ENCD__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define L_ENCD_PRTDSI__OUT_SEL0       (* (reg8 *) L_ENCD__PRTDSI__OUT_SEL0) 
#define L_ENCD_PRTDSI__OUT_SEL1       (* (reg8 *) L_ENCD__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define L_ENCD_PRTDSI__SYNC_OUT       (* (reg8 *) L_ENCD__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(L_ENCD__SIO_CFG)
    #define L_ENCD_SIO_HYST_EN        (* (reg8 *) L_ENCD__SIO_HYST_EN)
    #define L_ENCD_SIO_REG_HIFREQ     (* (reg8 *) L_ENCD__SIO_REG_HIFREQ)
    #define L_ENCD_SIO_CFG            (* (reg8 *) L_ENCD__SIO_CFG)
    #define L_ENCD_SIO_DIFF           (* (reg8 *) L_ENCD__SIO_DIFF)
#endif /* (L_ENCD__SIO_CFG) */

/* Interrupt Registers */
#if defined(L_ENCD__INTSTAT)
    #define L_ENCD_INTSTAT            (* (reg8 *) L_ENCD__INTSTAT)
    #define L_ENCD_SNAP               (* (reg8 *) L_ENCD__SNAP)
    
	#define L_ENCD_0_INTTYPE_REG 		(* (reg8 *) L_ENCD__0__INTTYPE)
#endif /* (L_ENCD__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_L_ENCD_H */


/* [] END OF FILE */
