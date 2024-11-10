/*******************************************************************************
* File Name: `$INSTANCE_NAME`_PM.c
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file provides low power mode APIs for the I2C component.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_PVT.h"

`$INSTANCE_NAME`_BACKUP_STRUCT `$INSTANCE_NAME`_backup =
{
    `$INSTANCE_NAME`_DISABLE,

#if (`$INSTANCE_NAME`_FF_IMPLEMENTED)
    `$INSTANCE_NAME`_DEFAULT_XCFG,
    `$INSTANCE_NAME`_DEFAULT_CFG,
    `$INSTANCE_NAME`_DEFAULT_ADDR,
    LO8(`$INSTANCE_NAME`_DEFAULT_DIVIDE_FACTOR),
    HI8(`$INSTANCE_NAME`_DEFAULT_DIVIDE_FACTOR),
#else  /* (`$INSTANCE_NAME`_UDB_IMPLEMENTED) */
    `$INSTANCE_NAME`_DEFAULT_CFG,
#endif /* (`$INSTANCE_NAME`_FF_IMPLEMENTED) */

#if (`$INSTANCE_NAME`_TIMEOUT_ENABLED)
    `$INSTANCE_NAME`_DEFAULT_TMOUT_PERIOD,
    `$INSTANCE_NAME`_DEFAULT_TMOUT_INTR_MASK,
#endif /* (`$INSTANCE_NAME`_TIMEOUT_ENABLED) */
};

#if ((`$INSTANCE_NAME`_FF_IMPLEMENTED) && (`$INSTANCE_NAME`_WAKEUP_ENABLED))
    volatile uint8 `$INSTANCE_NAME`_wakeupSource;
#endif /* ((`$INSTANCE_NAME`_FF_IMPLEMENTED) && (`$INSTANCE_NAME`_WAKEUP_ENABLED)) */


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SaveConfig
********************************************************************************
*
* Summary:
*  The Enable wakeup from Sleep Mode selection influences this function
*  implementation:
*   Unchecked: Stores the component non-retention configuration registers.
*   Checked:   Disables the master, if it was enabled before, and enables
*              backup regulator of the I2C hardware. If a transaction intended
*              for component executes during this function call, it waits until
*              the current transaction is completed and I2C hardware is ready
*              to enter sleep mode. All subsequent I2C traffic is NAKed until
*              the device is put into sleep mode.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  `$INSTANCE_NAME`_backup - The global variable used to save the component
*                            configuration and non-retention registers before
*                            entering the sleep mode.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_SaveConfig(void) `=ReentrantKeil($INSTANCE_NAME . "_SaveConfig")`
{
#if (`$INSTANCE_NAME`_FF_IMPLEMENTED)
    #if (`$INSTANCE_NAME`_WAKEUP_ENABLED)
        uint8 intState;
    #endif /* (`$INSTANCE_NAME`_WAKEUP_ENABLED) */

    /* Store registers before enter low power mode */
    `$INSTANCE_NAME`_backup.cfg     = `$INSTANCE_NAME`_CFG_REG;
    `$INSTANCE_NAME`_backup.xcfg    = `$INSTANCE_NAME`_XCFG_REG;
    `$INSTANCE_NAME`_backup.addr    = `$INSTANCE_NAME`_ADDR_REG;
    `$INSTANCE_NAME`_backup.clkDiv1 = `$INSTANCE_NAME`_CLKDIV1_REG;
    `$INSTANCE_NAME`_backup.clkDiv2 = `$INSTANCE_NAME`_CLKDIV2_REG;

#if (`$INSTANCE_NAME`_WAKEUP_ENABLED)
    /* Disable master */
    `$INSTANCE_NAME`_CFG_REG &= (uint8) ~`$INSTANCE_NAME`_ENABLE_MASTER;

    /* Enable backup regulator to keep block powered in low power mode */
    intState = CyEnterCriticalSection();
    `$INSTANCE_NAME`_PWRSYS_CR1_REG |= `$INSTANCE_NAME`_PWRSYS_CR1_I2C_REG_BACKUP;
    CyExitCriticalSection(intState);

    /* 1) Set force NACK to ignore I2C transactions;
    *  2) Wait unti I2C is ready go to Sleep; !!
    *  3) These bits are cleared on wake up.
    */
    /* Wait when block is ready for sleep */
    `$INSTANCE_NAME`_XCFG_REG |= `$INSTANCE_NAME`_XCFG_FORCE_NACK;
    while (0u == (`$INSTANCE_NAME`_XCFG_REG & `$INSTANCE_NAME`_XCFG_RDY_TO_SLEEP))
    {
    }

    /* Setup wakeup interrupt */
    `$INSTANCE_NAME`_DisableInt();
    (void) CyIntSetVector(`$INSTANCE_NAME`_ISR_NUMBER, &`$INSTANCE_NAME`_WAKEUP_ISR);
    `$INSTANCE_NAME`_wakeupSource = 0u;
    `$INSTANCE_NAME`_EnableInt();
#endif /* (`$INSTANCE_NAME`_WAKEUP_ENABLED) */

#else
    /* Store only address match bit */
    `$INSTANCE_NAME`_backup.control = (`$INSTANCE_NAME`_CFG_REG & `$INSTANCE_NAME`_CTRL_ANY_ADDRESS_MASK);
#endif /* (`$INSTANCE_NAME`_FF_IMPLEMENTED) */

#if (`$INSTANCE_NAME`_TIMEOUT_ENABLED)
    `$INSTANCE_NAME`_TimeoutSaveConfig();
#endif /* (`$INSTANCE_NAME`_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred method to prepare the component before device enters
*  sleep mode. The Enable wakeup from Sleep Mode selection influences this
*  function implementation:
*   Unchecked: Checks current I2C component state, saves it, and disables the
*              component by calling I2C_Stop() if it is currently enabled.
*              I2C_SaveConfig() is then called to save the component
*              non-retention configuration registers.
*   Checked:   If a transaction intended for component executes during this
*              function call, it waits until the current transaction is
*              completed. All subsequent I2C traffic intended for component
*              is NAKed until the device is put to sleep mode. The address
*              match event wakes up the device.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Sleep(void) `=ReentrantKeil($INSTANCE_NAME . "_Sleep")`
{
#if (`$INSTANCE_NAME`_WAKEUP_ENABLED)
    /* Do not enable block after exit low power mode if it is wakeup source */
    `$INSTANCE_NAME`_backup.enableState = `$INSTANCE_NAME`_DISABLE;

    #if (`$INSTANCE_NAME`_TIMEOUT_ENABLED)
        `$INSTANCE_NAME`_TimeoutStop();
    #endif /* (`$INSTANCE_NAME`_TIMEOUT_ENABLED) */

#else
    /* Store enable state */
    `$INSTANCE_NAME`_backup.enableState = (uint8) `$INSTANCE_NAME`_IS_ENABLED;

    if (0u != `$INSTANCE_NAME`_backup.enableState)
    {
        `$INSTANCE_NAME`_Stop();
    }
#endif /* (`$INSTANCE_NAME`_WAKEUP_ENABLED) */

    `$INSTANCE_NAME`_SaveConfig();
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_RestoreConfig
********************************************************************************
*
* Summary:
*  The Enable wakeup from Sleep Mode selection influences this function
*  implementation:
*   Unchecked: Restores the component non-retention configuration registers
*              to the state they were in before I2C_Sleep() or I2C_SaveConfig()
*              was called.
*   Checked:   Disables the backup regulator of the I2C hardware. Sets up the
*              regular component interrupt handler and generates the component
*              interrupt if it was wake up source to release the bus and
*              continue in-coming I2C transaction.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  `$INSTANCE_NAME`_backup - The global variable used to save the component
*                            configuration and non-retention registers before
*                            exiting the sleep mode.
*
* Reentrant:
*  No.
*
* Side Effects:
*  Calling this function before `$INSTANCE_NAME`_SaveConfig() or
*  `$INSTANCE_NAME`_Sleep() will lead to unpredictable results.
*
*******************************************************************************/
void `$INSTANCE_NAME`_RestoreConfig(void) `=ReentrantKeil($INSTANCE_NAME . "_RestoreConfig")`
{
#if (`$INSTANCE_NAME`_FF_IMPLEMENTED)
    uint8 intState;

    if (`$INSTANCE_NAME`_CHECK_PWRSYS_I2C_BACKUP)
    /* Low power mode was Sleep - backup regulator is enabled */
    {
        /* Enable backup regulator in active mode */
        intState = CyEnterCriticalSection();
        `$INSTANCE_NAME`_PWRSYS_CR1_REG &= (uint8) ~`$INSTANCE_NAME`_PWRSYS_CR1_I2C_REG_BACKUP;
        CyExitCriticalSection(intState);

        /* Restore master */
        `$INSTANCE_NAME`_CFG_REG = `$INSTANCE_NAME`_backup.cfg;
    }
    else
    /* Low power mode was Hibernate - backup regulator is disabled. All registers are cleared */
    {
    #if (`$INSTANCE_NAME`_WAKEUP_ENABLED)
        /* Disable power to block before register restore */
        intState = CyEnterCriticalSection();
        `$INSTANCE_NAME`_ACT_PWRMGR_REG  &= (uint8) ~`$INSTANCE_NAME`_ACT_PWR_EN;
        `$INSTANCE_NAME`_STBY_PWRMGR_REG &= (uint8) ~`$INSTANCE_NAME`_STBY_PWR_EN;
        CyExitCriticalSection(intState);

        /* Enable component in I2C_Wakeup() after register restore */
        `$INSTANCE_NAME`_backup.enableState = `$INSTANCE_NAME`_ENABLE;
    #endif /* (`$INSTANCE_NAME`_WAKEUP_ENABLED) */

        /* Restore component registers after Hibernate */
        `$INSTANCE_NAME`_XCFG_REG    = `$INSTANCE_NAME`_backup.xcfg;
        `$INSTANCE_NAME`_CFG_REG     = `$INSTANCE_NAME`_backup.cfg;
        `$INSTANCE_NAME`_ADDR_REG    = `$INSTANCE_NAME`_backup.addr;
        `$INSTANCE_NAME`_CLKDIV1_REG = `$INSTANCE_NAME`_backup.clkDiv1;
        `$INSTANCE_NAME`_CLKDIV2_REG = `$INSTANCE_NAME`_backup.clkDiv2;
    }

#if (`$INSTANCE_NAME`_WAKEUP_ENABLED)
    `$INSTANCE_NAME`_DisableInt();
    (void) CyIntSetVector(`$INSTANCE_NAME`_ISR_NUMBER, &`$INSTANCE_NAME`_ISR);
    if (0u != `$INSTANCE_NAME`_wakeupSource)
    {
        /* Generate interrupt to process incoming transaction */
        `$INSTANCE_NAME`_SetPendingInt();
    }
    `$INSTANCE_NAME`_EnableInt();
#endif /* (`$INSTANCE_NAME`_WAKEUP_ENABLED) */

#else
    `$INSTANCE_NAME`_CFG_REG = `$INSTANCE_NAME`_backup.control;
#endif /* (`$INSTANCE_NAME`_FF_IMPLEMENTED) */

#if (`$INSTANCE_NAME`_TIMEOUT_ENABLED)
    `$INSTANCE_NAME`_TimeoutRestoreConfig();
#endif /* (`$INSTANCE_NAME`_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred method to prepare the component for active mode
*  operation (when device exits sleep mode). The Enable wakeup from Sleep Mode
*  selection influences this function implementation:
*   Unchecked: Restores the component non-retention configuration registers
*              by calling I2C_RestoreConfig(). If the component was enabled
*              before the I2C_Sleep() function was called, I2C_Wakeup()
*              re-enables it.
*   Checked:   Enables  master functionality if it was enabled before sleep,
*              and disables the backup regulator of the I2C hardware.
*              The incoming transaction continues as soon as the regular
*              I2C interrupt handler is set up (global interrupts has to be
*              enabled to service I2C component interrupt).
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
* Side Effects:
*  Calling this function before `$INSTANCE_NAME`_SaveConfig() or
*  `$INSTANCE_NAME`_Sleep() will lead to unpredictable results.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Wakeup(void) `=ReentrantKeil($INSTANCE_NAME . "_Wakeup")`
{
    `$INSTANCE_NAME`_RestoreConfig();

    /* Restore component enable state */
    if (0u != `$INSTANCE_NAME`_backup.enableState)
    {
        `$INSTANCE_NAME`_Enable();
        `$INSTANCE_NAME`_EnableInt();
    }
    else
    {
    #if (`$INSTANCE_NAME`_TIMEOUT_ENABLED)
        `$INSTANCE_NAME`_TimeoutEnable();
    #endif /* (`$INSTANCE_NAME`_TIMEOUT_ENABLED) */
    }
}


/* [] END OF FILE */
