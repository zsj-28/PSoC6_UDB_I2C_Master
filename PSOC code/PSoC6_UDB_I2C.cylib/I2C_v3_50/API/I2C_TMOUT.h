/*******************************************************************************
* File Name: `$INSTANCE_NAME`_TMOUT.h
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file provides constants and parameter values for the I2C component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_I2C_TMOUT_`$INSTANCE_NAME`_H)
#define CY_I2C_TMOUT_`$INSTANCE_NAME`_H

#include "cyfitter.h"
#include "cy_syslib.h"
#include "`$INSTANCE_NAME`.h"


/***************************************
*        Function Prototypes
***************************************/

void `$INSTANCE_NAME`_TimeoutInit(void)                 `=ReentrantKeil($INSTANCE_NAME . "_TimeoutInit")`;
void `$INSTANCE_NAME`_TimeoutEnable(void)               `=ReentrantKeil($INSTANCE_NAME . "_TimeoutEnable")`;
void `$INSTANCE_NAME`_TimeoutStop(void)                 `=ReentrantKeil($INSTANCE_NAME . "_TimeoutStop")`;

void `$INSTANCE_NAME`_TimeoutReset(void)                `=ReentrantKeil($INSTANCE_NAME . "_TimeoutReset")`;

void `$INSTANCE_NAME`_TimeoutSaveConfig(void)           `=ReentrantKeil($INSTANCE_NAME . "_TimeoutSaveConfig")`;
void `$INSTANCE_NAME`_TimeoutRestoreConfig(void)        `=ReentrantKeil($INSTANCE_NAME . "_TimeoutRestoreConfig")`;

void `$INSTANCE_NAME`_TimeoutSetPeriod(uint16 period)   `=ReentrantKeil($INSTANCE_NAME . "_TimeoutSetPeriod")`;
uint16 `$INSTANCE_NAME`_TimeoutGetPeriod(void)          `=ReentrantKeil($INSTANCE_NAME . "_TimeoutGetPeriod")`;

#if (`$INSTANCE_NAME`_TIMEOUT_PRESCALER_ENABLED)
    void  `$INSTANCE_NAME`_TimeoutSetPrescalerPeriod(uint8 period)
                                                        `=ReentrantKeil($INSTANCE_NAME . "_TimeoutSetPrescalerPeriod")`;
    uint8 `$INSTANCE_NAME`_TimeoutGetPrescalerPeriod(void)
                                                        `=ReentrantKeil($INSTANCE_NAME . "_TimeoutGetPrescalerPeriod")`;
#endif /* (`$INSTANCE_NAME`_TIMEOUT_PRESCALER_ENABLED) */

#if (`$INSTANCE_NAME`_TIMEOUT_FF_ENABLED)
    #define `$INSTANCE_NAME`_TimeoutEnableInt() \
            do{                                 \
                `$INSTANCE_NAME`_TMOUT_CSR_REG |= (uint8)  `$INSTANCE_NAME`_TMOUT_CSR_I2C_TMOUT_IE; \
            }while(0)

    #define `$INSTANCE_NAME`_TimeoutDisableInt() \
            do{                                  \
                `$INSTANCE_NAME`_TMOUT_CSR_REG &= (uint8) ~`$INSTANCE_NAME`_TMOUT_CSR_I2C_TMOUT_IE; \
            }while(0)

    #define `$INSTANCE_NAME`_TimeoutClearPendingInt()       CyIntClearPending(`$INSTANCE_NAME`_ISR_NUMBER)

    #define `$INSTANCE_NAME`_TimeoutSetIntrMode(intrSource) \
            do{                                             \
                `$INSTANCE_NAME`_TMOUT_CSR_REG |= ((intrSource) & `$INSTANCE_NAME`_TMOUT_CSR_INTR_MASK); \
            }while(0)

    #define `$INSTANCE_NAME`_TimeoutGetIntrMode()           (`$INSTANCE_NAME`_TMOUT_CSR_REG & \
                                                             `$INSTANCE_NAME`_TMOUT_CSR_INTR_MASK)

    #define `$INSTANCE_NAME`_TimeoutGetStatus()             (`$INSTANCE_NAME`_TMOUT_SR_REG)
    #define `$INSTANCE_NAME`_TimeoutClearStatus(intrStatus) {`$INSTANCE_NAME`_TMOUT_SR_REG = (intrStatus);}

#else
    CY_ISR_PROTO(`$INSTANCE_NAME`_TMOUT_ISR);    /* Timeout interrupt handler */
 
	#define	LO8			CY_LO8
	#define HI8			CY_HI8

    #define `$INSTANCE_NAME`_TimeoutEnableInt()         __NVIC_EnableIRQ      (`$INSTANCE_NAME`_TMOUT_ISR_NUMBER)
    #define `$INSTANCE_NAME`_TimeoutDisableInt()        __NVIC_DisableIRQ     (`$INSTANCE_NAME`_TMOUT_ISR_NUMBER)
    #define `$INSTANCE_NAME`_TimeoutClearPendingInt()   __NVIC_ClearPendingIRQ(`$INSTANCE_NAME`_TMOUT_ISR_NUMBER)

    #define `$INSTANCE_NAME`_TimeoutSetIntrMode(intrSource) \
            do{                                             \
            `$INSTANCE_NAME`_TMOUT_INTR_MASK_REG = ((intrSource) & `$INSTANCE_NAME`_TMOUT_STS_INTR_MASK); \
            }while(0)

    #define `$INSTANCE_NAME`_TimeoutGetIntrMode() \
            (`$INSTANCE_NAME`_TMOUT_INTR_MASK_REG & `$INSTANCE_NAME`_TMOUT_STS_INTR_MASK)

    #define `$INSTANCE_NAME`_TimeoutGetStatus() \
            (`$INSTANCE_NAME`_TMOUT_STS_REG & `$INSTANCE_NAME`_TMOUT_STS_INTR_MASK)

    #define `$INSTANCE_NAME`_TimeoutClearStatus(intrStatus) /* There is no implementation for UDB version */

#endif /* (`$INSTANCE_NAME`_TIMEOUT_FF_ENABLED) */


/***************************************
*        API constants
***************************************/


/***************************************
*              Registers
***************************************/

#if (`$INSTANCE_NAME`_TIMEOUT_FF_ENABLED)
    /* I2C FF timeout hardware */
    #define `$INSTANCE_NAME`_TMOUT_CSR_REG  (*(reg8 *) `$INSTANCE_NAME`_I2C_FF__TMOUT_CSR)
    #define `$INSTANCE_NAME`_TMOUT_CSR_PTR  ( (reg8 *) `$INSTANCE_NAME`_I2C_FF__TMOUT_CSR)

    #define `$INSTANCE_NAME`_TMOUT_SR_REG   (*(reg8 *) `$INSTANCE_NAME`_I2C_FF__TMOUT_SR)
    #define `$INSTANCE_NAME`_TMOUT_SR_PTR   ( (reg8 *) `$INSTANCE_NAME`_I2C_FF__TMOUT_SR)

    #define `$INSTANCE_NAME`_TMOUT_CFG0_REG (*(reg8 *) `$INSTANCE_NAME`_I2C_FF__TMOUT_CFG0)
    #define `$INSTANCE_NAME`_TMOUT_CFG0_PTR ( (reg8 *) `$INSTANCE_NAME`_I2C_FF__TMOUT_CFG0)

    #define `$INSTANCE_NAME`_TMOUT_CFG1_REG (*(reg8 *) `$INSTANCE_NAME`_I2C_FF__TMOUT_CFG1)
    #define `$INSTANCE_NAME`_TMOUT_CFG1_PTR ( (reg8 *) `$INSTANCE_NAME`_I2C_FF__TMOUT_CFG1)

#else

    #define `$INSTANCE_NAME`_TMOUT_CTRL_REG \
                        (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_`$CtlModeReplacementString`_CtrlReg__CONTROL_REG)
    #define `$INSTANCE_NAME`_TMOUT_CTRL_PTR \
                        ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_`$CtlModeReplacementString`_CtrlReg__CONTROL_REG)

    #define `$INSTANCE_NAME`_TMOUT_STS_REG          (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_StsReg__STATUS_REG)
    #define `$INSTANCE_NAME`_TMOUT_STS_PTR          ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_StsReg__STATUS_REG)

    #define `$INSTANCE_NAME`_TMOUT_INTR_MASK_REG    (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_StsReg__MASK_REG)
    #define `$INSTANCE_NAME`_TMOUT_INTR_MASK_PTR    ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_StsReg__MASK_REG)

    #define `$INSTANCE_NAME`_TMOUT_INTR_ENABLE_REG  (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_StsReg__STATUS_AUX_CTL_REG)
    #define `$INSTANCE_NAME`_TMOUT_INTR_ENABLE_PTR  ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_StsReg__STATUS_AUX_CTL_REG)

    #if (`$INSTANCE_NAME`_TIMEOUT_SCL_TMOUT_ENABLED)
        #define `$INSTANCE_NAME`_TMOUT_SCL_PRD0_REG     (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Scl_dpScl_u0__F0_REG)
        #define `$INSTANCE_NAME`_TMOUT_SCL_PRD0_PTR     ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Scl_dpScl_u0__F0_REG)

        #define `$INSTANCE_NAME`_TMOUT_SCL_PRD1_REG     (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Scl_dpScl_u0__F1_REG)
        #define `$INSTANCE_NAME`_TMOUT_SCL_PRD1_PTR     ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Scl_dpScl_u0__F1_REG)

        #define `$INSTANCE_NAME`_TMOUT_SCL_ADDER_REG    (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Scl_dpScl_u0__D0_REG)
        #define `$INSTANCE_NAME`_TMOUT_SCL_ADDER_PTR    ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Scl_dpScl_u0__D0_REG)

        #define `$INSTANCE_NAME`_TMOUT_SCL_AUX_CTRL_REG \
                                                (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Scl_dpScl_u0__DP_AUX_CTL_REG)
        #define `$INSTANCE_NAME`_TMOUT_SCL_AUX_CTRL_PTR \
                                                ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Scl_dpScl_u0__DP_AUX_CTL_REG)

        #if (`$INSTANCE_NAME`_TIMEOUT_PRESCALER_ENABLED)
            #define `$INSTANCE_NAME`_SCL_PRESCALER_AUX_CTRL_REG \
                                    (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_SclPrescaler_prScl__CONTROL_AUX_CTL_REG)
            #define `$INSTANCE_NAME`_SCL_PRESCALER_AUX_CTRL_PTR \
                                    ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_SclPrescaler_prScl__CONTROL_AUX_CTL_REG)

            #define `$INSTANCE_NAME`_SCL_PRESCALER_PRD_REG  \
                                    (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_SclPrescaler_prScl__PERIOD_REG)
            #define `$INSTANCE_NAME`_SCL_PRESCALER_PRD_PTR  \
                                    ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_SclPrescaler_prScl__PERIOD_REG)

        #endif /* (`$INSTANCE_NAME`_TIMEOUT_PRESCALER_ENABLED) */
    #endif     /* (`$INSTANCE_NAME`_TIMEOUT_SCL_TMOUT_ENABLED) */

    #if (`$INSTANCE_NAME`_TIMEOUT_SDA_TMOUT_ENABLED)
        #define `$INSTANCE_NAME`_TMOUT_SDA_PRD0_REG     (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Sda_dpSda_u0__F0_REG)
        #define `$INSTANCE_NAME`_TMOUT_SDA_PRD0_PTR     ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Sda_dpSda_u0__F0_REG)

        #define `$INSTANCE_NAME`_TMOUT_SDA_PRD1_REG     (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Sda_dpSda_u0__F1_REG)
        #define `$INSTANCE_NAME`_TMOUT_SDA_PRD1_PTR     ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Sda_dpSda_u0__F1_REG)

        #define `$INSTANCE_NAME`_TMOUT_SDA_ADDER_REG    (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Sda_dpSda_u0__D0_REG)
        #define `$INSTANCE_NAME`_TMOUT_SDA_ADDER_PTR    ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Sda_dpSda_u0__D0_REG)

        #define `$INSTANCE_NAME`_TMOUT_SDA_AUX_CTRL_REG \
                                                (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Sda_dpSda_u0__DP_AUX_CTL_REG)
        #define `$INSTANCE_NAME`_TMOUT_SDA_AUX_CTRL_PTR \
                                                ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_Sda_dpSda_u0__DP_AUX_CTL_REG)

        #if (`$INSTANCE_NAME`_TIMEOUT_PRESCALER_ENABLED)
            #define `$INSTANCE_NAME`_SDA_PRESCALER_AUX_CTRL_REG \
                                    (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_SdaPrescaler_prSda__CONTROL_AUX_CTL_REG)
            #define `$INSTANCE_NAME`_SDA_PRESCALER_AUX_CTRL_PTR \
                                    ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_SdaPrescaler_prSda__CONTROL_AUX_CTL_REG)

            #define `$INSTANCE_NAME`_SDA_PRESCALER_PRD_REG \
                                    (*(reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_SdaPrescaler_prSda__PERIOD_REG)
            #define `$INSTANCE_NAME`_SDA_PRESCALER_PRD_PTR \
                                    ( (reg8 *) `$INSTANCE_NAME`_bTimeoutTimer_SdaPrescaler_prSda__PERIOD_REG)

        #endif /* (`$INSTANCE_NAME`_TIMEOUT_PRESCALER_ENABLED) */
    #endif /* (`$INSTANCE_NAME`_TIMEOUT_SDA_TMOUT_ENABLED) */

#endif /*(`$INSTANCE_NAME`_TIMEOUT_FF_ENABLED) */


/***************************************
*        Registers Constants
***************************************/

#if (`$INSTANCE_NAME`_TIMEOUT_FF_ENABLED)
    /* `$INSTANCE_NAME`_TMOUT_CSR */
    #define `$INSTANCE_NAME`_TMOUT_CSR_SDA_PIN_POS      (4u)
    #define `$INSTANCE_NAME`_TMOUT_CSR_SCL_PIN_POS      (3u)
    #define `$INSTANCE_NAME`_TMOUT_CSR_I2C_TMOUT_IE_POS (2u)
    #define `$INSTANCE_NAME`_TMOUT_CSR_SDA_TMOUT_IE_POS (1u)
    #define `$INSTANCE_NAME`_TMOUT_CSR_SCL_TMOUT_IE_POS (0u)
    #define `$INSTANCE_NAME`_TMOUT_CSR_SDA_PIN_STS  ((uint8) (0x01u << `$INSTANCE_NAME`_TMOUT_CSR_SDA_PIN_POS))
    #define `$INSTANCE_NAME`_TMOUT_CSR_SCL_PIN_STS  ((uint8) (0x01u << `$INSTANCE_NAME`_TMOUT_CSR_SCL_PIN_POS))
    #define `$INSTANCE_NAME`_TMOUT_CSR_I2C_TMOUT_IE ((uint8) (0x01u << `$INSTANCE_NAME`_TMOUT_CSR_I2C_TMOUT_IE_POS))
    #define `$INSTANCE_NAME`_TMOUT_CSR_SDA_TMOUT_IE ((uint8) (0x01u << `$INSTANCE_NAME`_TMOUT_CSR_SDA_TMOUT_IE_POS))
    #define `$INSTANCE_NAME`_TMOUT_CSR_SCL_TMOUT_IE ((uint8) (0x01u << `$INSTANCE_NAME`_TMOUT_CSR_SCL_TMOUT_IE_POS))
    #define `$INSTANCE_NAME`_TMOUT_CSR_INTR_MASK    ((uint8) (0x03u << `$INSTANCE_NAME`_TMOUT_CSR_SCL_TMOUT_IE_POS))

    /* `$INSTANCE_NAME`_TMOUT_SR */
    #define `$INSTANCE_NAME`_TMOUT_SR_SDA_PIN_TMOUT_POS (1u)
    #define `$INSTANCE_NAME`_TMOUT_SR_SCL_PIN_TMOUT_POS (0u)
    #define `$INSTANCE_NAME`_TMOUT_SR_SDA_PIN_TMOUT ((uint8) (0x01u << `$INSTANCE_NAME`_TMOUT_SR_SDA_PIN_TMOUT_STS_POS))
    #define `$INSTANCE_NAME`_TMOUT_SR_SCL_PIN_TMOUT ((uint8) (0x01u << `$INSTANCE_NAME`_TMOUT_SR_SCL_PIN_TMOUT_STS_POS))
    #define `$INSTANCE_NAME`_TMOUT_SR_PINS_TMOUT_MASK   ((uint8) (0x03u << `$INSTANCE_NAME`_TMOUT_SR_SCL_PIN_TMOUT_POS))

    /*`$INSTANCE_NAME`_TMOUT_CFG0 and `$INSTANCE_NAME`_TMOUT_CFG1 */
    #define `$INSTANCE_NAME`_TMOUT_CFG0__MASK   (0xFFu)
    #define `$INSTANCE_NAME`_TMOUT_CFG1__MASK   (0x0Fu)

#else

    /* Interrupt setup */
    #define `$INSTANCE_NAME`_TMOUT_ISR_NUMBER   ((uint8) `$INSTANCE_NAME`_I2C_TMOUT__INTC_NUMBER)
	/* isr can be assigned only to M4 or to M0*/
	#if(`$INSTANCE_NAME`_I2C_IRQ_INTC_CORTEXM4_ASSIGNED)
    #define `$INSTANCE_NAME`_TMOUT_ISR_PRIORITY ((uint8) `$INSTANCE_NAME`_I2C_TMOUT__INTC_CORTEXM4_PRIORITY)
	#else
	#define `$INSTANCE_NAME`_TMOUT_ISR_PRIORITY ((uint8) `$INSTANCE_NAME`_I2C_TMOUT__INTC_CORTEXM0P_PRIORITY)	
	#endif

    /* Timeout STATUS register constants */
    #define `$INSTANCE_NAME`_TMOUT_CTRL_ENABLE_POS  (0u)
    #define `$INSTANCE_NAME`_TMOUT_CTRL_ENABLE      ((uint8) (0x01u << `$INSTANCE_NAME`_TMOUT_CTRL_ENABLE_POS))

    /* Timeout STATUS register constants */
    #define `$INSTANCE_NAME`_TMOUT_STS_SDA_PIN_POS  (3u)
    #define `$INSTANCE_NAME`_TMOUT_STS_SCL_PIN_POS  (4u)
    #define `$INSTANCE_NAME`_TMOUT_STS_SDA_INTR_POS (1u)
    #define `$INSTANCE_NAME`_TMOUT_STS_SCL_INTR_POS (0u)
    #define `$INSTANCE_NAME`_TMOUT_STS_SCL_PINS     ((uint8) (0x01u << `$INSTANCE_NAME`_TMOUT_STS_SDA_PIN_POS))
    #define `$INSTANCE_NAME`_TMOUT_STS_SDA_PIN      ((uint8) (0x01u << `$INSTANCE_NAME`_TMOUT_STS_SCL_PIN_POS))
    #define `$INSTANCE_NAME`_TMOUT_STS_SCL_INTR     ((uint8) (0x01u << `$INSTANCE_NAME`_TMOUT_STS_SDA_INTR_POS))
    #define `$INSTANCE_NAME`_TMOUT_STS_SDA_INTR     ((uint8) (0x01u << `$INSTANCE_NAME`_TMOUT_STS_SCL_INTR_POS))
    #define `$INSTANCE_NAME`_TMOUT_STS_INTR_MASK    ((uint8) (0x03u << `$INSTANCE_NAME`_TMOUT_STS_SCL_INTR_POS))

    /* AUX_CTL bits definition */
    #define `$INSTANCE_NAME`_TMOUT_FIFO_SINGLE_REG      (0x03u) /* Makes f0 and f1 as single reg */
    #define `$INSTANCE_NAME`_TMOUT_PRESCALER_CNT7_EN    (0x20u) /* Enable 7-bit counter */
    #define `$INSTANCE_NAME`_TMOUT_ENABLE_INTR          (0x10u) /* Enable interrupt from status register */

#endif /* (`$INSTANCE_NAME`_TIMEOUT_FF_ENABLED) */


/***************************************
*        Marco
***************************************/

#if (`$INSTANCE_NAME`_TIMEOUT_FF_ENABLED)
    /* Returns true if timeout functionality is enabled */
    #define `$INSTANCE_NAME`_IS_TIMEOUT_ENABLED (0u != (`$INSTANCE_NAME`_TMOUT_CSR_REG & \
                                                        `$INSTANCE_NAME`_TMOUT_CSR_I2C_TMOUT_IE))

#else
    /* Returns true if timeout functionality is enabled */
    #define `$INSTANCE_NAME`_IS_TIMEOUT_ENABLED (0u != (`$INSTANCE_NAME`_TMOUT_CTRL_ENABLE & \
                                                        `$INSTANCE_NAME`_TMOUT_CTRL_REG))
#endif /* (`$INSTANCE_NAME`_TIMEOUT_FF_ENABLED) */


/***************************************
*     Default register init constants
***************************************/

#define `$INSTANCE_NAME`_DEFAULT_TMOUT_PERIOD   ((`$INSTANCE_NAME`_TIMEOUT_FF_ENABLED) ? \
                                                    ((uint16) `$TimeoutPeriodff`u) : ((uint16) `$TimeoutPeriodUdb`u))
#define `$INSTANCE_NAME`_DEFAULT_TMOUT_PRESCALER_PRD    (`$PrescalerPeriod`u)
#define `$INSTANCE_NAME`_DEFAULT_TMOUT_INTR_MASK        ((uint8) (`$INSTANCE_NAME`_TIMEOUT_SDA_TMOUT_ENABLE << 1u) | \
                                                                  `$INSTANCE_NAME`_TIMEOUT_SCL_TMOUT_ENABLE)
#define `$INSTANCE_NAME`_DEFAULT_TMOUT_ADDER    (0xFFu)

#endif /* (CY_I2C_TMOUT_`$INSTANCE_NAME`_H) */


/* [] END OF FILE */
