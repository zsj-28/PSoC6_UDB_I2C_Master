/*******************************************************************************
* File Name: `$INSTANCE_NAME`.c
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file provides the source code of APIs for the I2C component.
*  The actual protocol and operation code resides in the interrupt service
*  routine file.
*
*******************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_PVT.h"


/**********************************
*      System variables
**********************************/

uint8 `$INSTANCE_NAME`_initVar = 0u; /* Defines if component was initialized */

volatile uint8 `$INSTANCE_NAME`_state;  /* Current state of I2C FSM */


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Init
********************************************************************************
*
* Summary:
*  Initializes I2C registers with initial values provided from customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Init(void) `=ReentrantKeil($INSTANCE_NAME . "_Init")`
{
#if (`$INSTANCE_NAME`_FF_IMPLEMENTED)
    /* Configure fixed function block */
    `$INSTANCE_NAME`_CFG_REG  = `$INSTANCE_NAME`_DEFAULT_CFG;
    `$INSTANCE_NAME`_XCFG_REG = `$INSTANCE_NAME`_DEFAULT_XCFG;
    `$INSTANCE_NAME`_ADDR_REG = `$INSTANCE_NAME`_DEFAULT_ADDR;
    `$INSTANCE_NAME`_CLKDIV1_REG = LO8(`$INSTANCE_NAME`_DEFAULT_DIVIDE_FACTOR);
    `$INSTANCE_NAME`_CLKDIV2_REG = HI8(`$INSTANCE_NAME`_DEFAULT_DIVIDE_FACTOR);

#else
    uint8 intState;

    /* Configure control and interrupt sources */
    `$INSTANCE_NAME`_CFG_REG      = `$INSTANCE_NAME`_DEFAULT_CFG;
    `$INSTANCE_NAME`_INT_MASK_REG = `$INSTANCE_NAME`_DEFAULT_INT_MASK;

    /* Enable interrupt generation in status */
    intState = CyEnterCriticalSection();
    `$INSTANCE_NAME`_INT_ENABLE_REG |= `$INSTANCE_NAME`_INTR_ENABLE;
    CyExitCriticalSection(intState);

    /* Configure bit counter */
    #if (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED)
        `$INSTANCE_NAME`_PERIOD_REG = `$INSTANCE_NAME`_DEFAULT_PERIOD;
    #endif  /* (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED) */

    /* Configure clock generator */
    #if (`$INSTANCE_NAME`_MODE_MASTER_ENABLED)
        `$INSTANCE_NAME`_MCLK_PRD_REG = `$INSTANCE_NAME`_DEFAULT_MCLK_PRD;
        `$INSTANCE_NAME`_MCLK_CMP_REG = `$INSTANCE_NAME`_DEFAULT_MCLK_CMP;
    #endif /* (`$INSTANCE_NAME`_MODE_MASTER_ENABLED) */
#endif /* (`$INSTANCE_NAME`_FF_IMPLEMENTED) */

#if (`$INSTANCE_NAME`_TIMEOUT_ENABLED)
    `$INSTANCE_NAME`_TimeoutInit();
#endif /* (`$INSTANCE_NAME`_TIMEOUT_ENABLED) */

    /* Configure internal interrupt */
    CyIntDisable    (`$INSTANCE_NAME`_ISR_NUMBER);
    CyIntSetPriority(`$INSTANCE_NAME`_ISR_NUMBER, `$INSTANCE_NAME`_ISR_PRIORITY);
    #if (`$INSTANCE_NAME`_INTERN_I2C_INTR_HANDLER)
        (void) CyIntSetVector(`$INSTANCE_NAME`_ISR_NUMBER, &`$INSTANCE_NAME`_ISR);
    #endif /* (`$INSTANCE_NAME`_INTERN_I2C_INTR_HANDLER) */

    /* Set FSM to default state */
    `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_IDLE;

#if (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED)
    /* Clear status and buffers index */
    `$INSTANCE_NAME`_slStatus = 0u;
    `$INSTANCE_NAME`_slRdBufIndex = 0u;
    `$INSTANCE_NAME`_slWrBufIndex = 0u;

    /* Configure matched address */
    `$INSTANCE_NAME`_SlaveSetAddress(`$INSTANCE_NAME`_DEFAULT_ADDR);
#endif /* (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED) */

#if (`$INSTANCE_NAME`_MODE_MASTER_ENABLED)
    /* Clear status and buffers index */
    `$INSTANCE_NAME`_mstrStatus = 0u;
    `$INSTANCE_NAME`_mstrRdBufIndex = 0u;
    `$INSTANCE_NAME`_mstrWrBufIndex = 0u;
#endif /* (`$INSTANCE_NAME`_MODE_MASTER_ENABLED) */
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Enable
********************************************************************************
*
* Summary:
*  Enables I2C operations.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  None.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Enable(void) `=ReentrantKeil($INSTANCE_NAME . "_Enable")`
{
#if (`$INSTANCE_NAME`_FF_IMPLEMENTED)
    uint8 intState;

    /* Enable power to block */
    intState = CyEnterCriticalSection();
    `$INSTANCE_NAME`_ACT_PWRMGR_REG  |= `$INSTANCE_NAME`_ACT_PWR_EN;
    `$INSTANCE_NAME`_STBY_PWRMGR_REG |= `$INSTANCE_NAME`_STBY_PWR_EN;
    CyExitCriticalSection(intState);
#else
    #if (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED)
        /* Enable bit counter */
        uint8 intState = CyEnterCriticalSection();
        `$INSTANCE_NAME`_COUNTER_AUX_CTL_REG |= `$INSTANCE_NAME`_CNT7_ENABLE;
        CyExitCriticalSection(intState);
    #endif /* (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED) */

    /* Enable slave or master bits */
    `$INSTANCE_NAME`_CFG_REG |= `$INSTANCE_NAME`_ENABLE_MS;
#endif /* (`$INSTANCE_NAME`_FF_IMPLEMENTED) */

#if (`$INSTANCE_NAME`_TIMEOUT_ENABLED)
    `$INSTANCE_NAME`_TimeoutEnable();
#endif /* (`$INSTANCE_NAME`_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Start
********************************************************************************
*
* Summary:
*  Starts the I2C hardware. Enables Active mode power template bits or clock
*  gating as appropriate. It is required to be executed before I2C bus
*  operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  This component automatically enables its interrupt.  If I2C is enabled !
*  without the interrupt enabled, it can lock up the I2C bus.
*
* Global variables:
*  `$INSTANCE_NAME`_initVar - This variable is used to check the initial
*                             configuration, modified on the first
*                             function call.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Start(void) `=ReentrantKeil($INSTANCE_NAME . "_Start")`
{
    if (0u == `$INSTANCE_NAME`_initVar)
    {
        `$INSTANCE_NAME`_Init();
        `$INSTANCE_NAME`_initVar = 1u; /* Component initialized */
    }

    `$INSTANCE_NAME`_Enable();
    `$INSTANCE_NAME`_EnableInt();
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Stop
********************************************************************************
*
* Summary:
*  Disables I2C hardware and disables I2C interrupt. Disables Active mode power
*  template bits or clock gating as appropriate.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Stop(void) `=ReentrantKeil($INSTANCE_NAME . "_Stop")`
{
    `$INSTANCE_NAME`_DisableInt();

#if (`$INSTANCE_NAME`_TIMEOUT_ENABLED)
    `$INSTANCE_NAME`_TimeoutStop();
#endif  /* End (`$INSTANCE_NAME`_TIMEOUT_ENABLED) */

#if (`$INSTANCE_NAME`_FF_IMPLEMENTED)
    {
        uint8 intState;
        uint16 blockResetCycles;

        /* Store registers effected by block disable */
        `$INSTANCE_NAME`_backup.addr    = `$INSTANCE_NAME`_ADDR_REG;
        `$INSTANCE_NAME`_backup.clkDiv1 = `$INSTANCE_NAME`_CLKDIV1_REG;
        `$INSTANCE_NAME`_backup.clkDiv2 = `$INSTANCE_NAME`_CLKDIV2_REG;

        /* Calculate number of cycles to reset block */
        blockResetCycles = ((uint16) ((uint16) `$INSTANCE_NAME`_CLKDIV2_REG << 8u) | `$INSTANCE_NAME`_CLKDIV1_REG) + 1u;

        /* Disable block */
        `$INSTANCE_NAME`_CFG_REG &= (uint8) ~`$INSTANCE_NAME`_CFG_EN_SLAVE;
        /* Wait for block reset before disable power */
        CyDelayCycles((uint32) blockResetCycles);

        /* Disable power to block */
        intState = CyEnterCriticalSection();
        `$INSTANCE_NAME`_ACT_PWRMGR_REG  &= (uint8) ~`$INSTANCE_NAME`_ACT_PWR_EN;
        `$INSTANCE_NAME`_STBY_PWRMGR_REG &= (uint8) ~`$INSTANCE_NAME`_STBY_PWR_EN;
        CyExitCriticalSection(intState);

        /* Enable block */
        `$INSTANCE_NAME`_CFG_REG |= (uint8) `$INSTANCE_NAME`_ENABLE_MS;

        /* Restore registers effected by block disable. Ticket ID#198004 */
        `$INSTANCE_NAME`_ADDR_REG    = `$INSTANCE_NAME`_backup.addr;
        `$INSTANCE_NAME`_ADDR_REG    = `$INSTANCE_NAME`_backup.addr;
        `$INSTANCE_NAME`_CLKDIV1_REG = `$INSTANCE_NAME`_backup.clkDiv1;
        `$INSTANCE_NAME`_CLKDIV2_REG = `$INSTANCE_NAME`_backup.clkDiv2;
    }
#else

    /* Disable slave or master bits */
    `$INSTANCE_NAME`_CFG_REG &= (uint8) ~`$INSTANCE_NAME`_ENABLE_MS;

#if (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED)
    {
        /* Disable bit counter */
        uint8 intState = CyEnterCriticalSection();
        `$INSTANCE_NAME`_COUNTER_AUX_CTL_REG &= (uint8) ~`$INSTANCE_NAME`_CNT7_ENABLE;
        CyExitCriticalSection(intState);
    }
#endif /* (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED) */

    /* Clear interrupt source register */
    (void) `$INSTANCE_NAME`_CSR_REG;
#endif /* (`$INSTANCE_NAME`_FF_IMPLEMENTED) */

    /* Disable interrupt on stop (enabled by write transaction) */
    `$INSTANCE_NAME`_DISABLE_INT_ON_STOP;
    `$INSTANCE_NAME`_ClearPendingInt();

    /* Reset FSM to default state */
    `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_IDLE;

    /* Clear busy statuses */
#if (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED)
    `$INSTANCE_NAME`_slStatus &= (uint8) ~(`$INSTANCE_NAME`_SSTAT_RD_BUSY | `$INSTANCE_NAME`_SSTAT_WR_BUSY);
#endif /* (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED) */
}


/* [] END OF FILE */
