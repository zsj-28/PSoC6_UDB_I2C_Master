/*******************************************************************************
* File Name: `$INSTANCE_NAME`_TMOUT_INT.c
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file provides the source code of Interrupt Service Routine (ISR)
*  for the timeout feature of the I2C component.
*
*  Note:
*
********************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_PVT.h"
`$CY_API_CALLBACK_HEADER_INCLUDE`


/*******************************************************************************
*  Place your includes, defines and code here
*******************************************************************************/
/* `#START `$INSTANCE_NAME`_TMOUT_ISR_intc` */

/* `#END` */


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_TimeoutReset
********************************************************************************
*
* Summary:
*  Resets the component into the known state (default) and enables it.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No
*
*******************************************************************************/
void `$INSTANCE_NAME`_TimeoutReset(void) `=ReentrantKeil($INSTANCE_NAME . "_TimeoutReset")`
{
    /* Reset component into known state (default) */
    `$INSTANCE_NAME`_Stop();

    /* `#START `$INSTANCE_NAME`_TMOUT_ISR_BEFORE_BUF_RESET` */

    /* `#END` */
    
#ifdef `$INSTANCE_NAME`_TIMEOUT_RESET_CALLBACK
    `$INSTANCE_NAME`_TimeoutReset_Callback();
#endif /* `$INSTANCE_NAME`_TIMEOUT_RESET_CALLBACK */

#if (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED)
    /* Clear status and buffers */
    `$INSTANCE_NAME`_SlaveClearReadBuf();
    `$INSTANCE_NAME`_SlaveClearWriteBuf();
    (void) `$INSTANCE_NAME`_SlaveClearReadStatus();
    (void) `$INSTANCE_NAME`_SlaveClearWriteStatus();
#endif /* (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED) */

#if (`$INSTANCE_NAME`_MODE_MASTER_ENABLED)
    /* Clear status and buffers */
    `$INSTANCE_NAME`_MasterClearReadBuf();
    `$INSTANCE_NAME`_MasterClearWriteBuf();
    (void) `$INSTANCE_NAME`_MasterClearStatus();
#endif /* (`$INSTANCE_NAME`_MODE_MASTER_ENABLED) */

    /* Enable component operation */
    `$INSTANCE_NAME`_Enable();
    `$INSTANCE_NAME`_EnableInt();
}


#if (`$INSTANCE_NAME`_TIMEOUT_UDB_ENABLED)
    /*******************************************************************************
    * Function Name: `$INSTANCE_NAME`_TMOUT_ISR
    ********************************************************************************
    *
    * Summary:
    *  The handler of the I2C timeout interrupt.
    *  Triggers when SCL or SDA remain in the low state greater than a defined
    *  timeout. Resets the component into the known state and enables it for
    *  the following operations.
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
    CY_ISR(`$INSTANCE_NAME`_TMOUT_ISR)
    {
    #ifdef `$INSTANCE_NAME`_TMOUT_ISR_ENTRY_CALLBACK
        `$INSTANCE_NAME`_TMOUT_ISR_EntryCallback();
    #endif /* `$INSTANCE_NAME`_TMOUT_ISR_ENTRY_CALLBACK */
        
        `$INSTANCE_NAME`_TimeoutReset();

    #ifdef `$INSTANCE_NAME`_TMOUT_ISR_EXIT_CALLBACK
        `$INSTANCE_NAME`_TMOUT_ISR_ExitCallback();
    #endif /* `$INSTANCE_NAME`_TMOUT_ISR_EXIT_CALLBACK*/
    }
#endif /* (`$INSTANCE_NAME`_TIMEOUT_UDB_ENABLED) */


/* [] END OF FILE */
