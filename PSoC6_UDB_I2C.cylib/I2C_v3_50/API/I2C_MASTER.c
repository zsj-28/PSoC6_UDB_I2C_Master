/*******************************************************************************
* File Name: `$INSTANCE_NAME`_MASTER.c
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file provides the source code of APIs for the I2C component master mode.
*
*******************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_PVT.h"

#if(`$INSTANCE_NAME`_MODE_MASTER_ENABLED)

/**********************************
*      System variables
**********************************/

volatile uint8 `$INSTANCE_NAME`_mstrStatus;     /* Master Status byte  */
volatile uint8 `$INSTANCE_NAME`_mstrControl;    /* Master Control byte */

/* Transmit buffer variables */
volatile uint8 * `$INSTANCE_NAME`_mstrRdBufPtr;     /* Pointer to Master Read buffer */
volatile uint8   `$INSTANCE_NAME`_mstrRdBufSize;    /* Master Read buffer size       */
volatile uint8   `$INSTANCE_NAME`_mstrRdBufIndex;   /* Master Read buffer Index      */

/* Receive buffer variables */
volatile uint8 * `$INSTANCE_NAME`_mstrWrBufPtr;     /* Pointer to Master Write buffer */
volatile uint8   `$INSTANCE_NAME`_mstrWrBufSize;    /* Master Write buffer size       */
volatile uint8   `$INSTANCE_NAME`_mstrWrBufIndex;   /* Master Write buffer Index      */


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_MasterWriteBuf
********************************************************************************
*
* Summary:
*  Automatically writes an entire buffer of data to a slave device. Once the
*  data transfer is initiated by this function, further data transfer is handled
*  by the included ISR in byte by byte mode.
*
* Parameters:
*  slaveAddr: 7-bit slave address.
*  xferData:  Pointer to buffer of data to be sent.
*  cnt:       Size of buffer to send.
*  mode:      Transfer mode defines: start or restart condition generation at
*             begin of the transfer and complete the transfer or halt before
*             generating a stop.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  The included ISR will start a transfer after a start or restart condition is
*  generated.
*
* Global variables:
*  `$INSTANCE_NAME`_mstrStatus  - The global variable used to store a current
*                                 status of the I2C Master.
*  `$INSTANCE_NAME`_state       - The global variable used to store a current
*                                 state of the software FSM.
*  `$INSTANCE_NAME`_mstrControl - The global variable used to control the master
*                                 end of a transaction with or without Stop
*                                 generation.
*  `$INSTANCE_NAME`_mstrWrBufPtr - The global variable used to store a pointer
*                                  to the master write buffer.
*  `$INSTANCE_NAME`_mstrWrBufIndex - The global variable used to store current
*                                    index within the master write buffer.
*  `$INSTANCE_NAME`_mstrWrBufSize - The global variable used to store a master
*                                   write buffer size.
*
* Reentrant:
*  No
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_MasterWriteBuf(uint8 slaveAddress, uint8 * wrData, uint8 cnt, uint8 mode)
      `=ReentrantKeil($INSTANCE_NAME . "_MasterWriteBuf")`
{
    uint8 errStatus;

    errStatus = `$INSTANCE_NAME`_MSTR_NOT_READY;

    if(NULL != wrData)
    {
        /* Check I2C state to allow transfer: valid states are IDLE or HALT */
        if(`$INSTANCE_NAME`_SM_IDLE == `$INSTANCE_NAME`_state)
        {
            /* Master is ready for transaction: check if bus is free */
            if(`$INSTANCE_NAME`_CHECK_BUS_FREE(`$INSTANCE_NAME`_MCSR_REG))
            {
                errStatus = `$INSTANCE_NAME`_MSTR_NO_ERROR;
            }
            else
            {
                errStatus = `$INSTANCE_NAME`_MSTR_BUS_BUSY;
            }
        }
        else if(`$INSTANCE_NAME`_SM_MSTR_HALT == `$INSTANCE_NAME`_state)
        {
            /* Master is ready and waiting for ReStart */
            errStatus = `$INSTANCE_NAME`_MSTR_NO_ERROR;

            `$INSTANCE_NAME`_ClearPendingInt();
            `$INSTANCE_NAME`_mstrStatus &= (uint8) ~`$INSTANCE_NAME`_MSTAT_XFER_HALT;
        }
        else
        {
            /* errStatus = `$INSTANCE_NAME`_MSTR_NOT_READY was send before */
        }

        if(`$INSTANCE_NAME`_MSTR_NO_ERROR == errStatus)
        {
            /* Set state to start write transaction */
            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_WR_ADDR;

            /* Prepare write buffer */
            `$INSTANCE_NAME`_mstrWrBufIndex = 0u;
            `$INSTANCE_NAME`_mstrWrBufSize  = cnt;
            `$INSTANCE_NAME`_mstrWrBufPtr   = (volatile uint8 *) wrData;

            /* Set end of transaction flag: Stop or Halt (following ReStart) */
            `$INSTANCE_NAME`_mstrControl = mode;

            /* Clear write status history */
            `$INSTANCE_NAME`_mstrStatus &= (uint8) ~`$INSTANCE_NAME`_MSTAT_WR_CMPLT;

            /* Hardware actions: write address and generate Start or ReStart */
            `$INSTANCE_NAME`_DATA_REG = (uint8) (slaveAddress << `$INSTANCE_NAME`_SLAVE_ADDR_SHIFT);

            if(`$INSTANCE_NAME`_CHECK_RESTART(mode))
            {
                `$INSTANCE_NAME`_GENERATE_RESTART;
            }
            else
            {
                `$INSTANCE_NAME`_GENERATE_START;
            }

            /* Enable interrupt to complete transfer */
            `$INSTANCE_NAME`_EnableInt();
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_MasterReadBuf
********************************************************************************
*
* Summary:
*  Automatically writes an entire buffer of data to a slave device. Once the
*  data transfer is initiated by this function, further data transfer is handled
*  by the included ISR in byte by byte mode.
*
* Parameters:
*  slaveAddr: 7-bit slave address.
*  xferData:  Pointer to buffer where to put data from slave.
*  cnt:       Size of buffer to read.
*  mode:      Transfer mode defines: start or restart condition generation at
*             begin of the transfer and complete the transfer or halt before
*             generating a stop.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  The included ISR will start a transfer after start or restart condition is
*  generated.
*
* Global variables:
*  `$INSTANCE_NAME`_mstrStatus  - The global variable used to store a current
*                                 status of the I2C Master.
*  `$INSTANCE_NAME`_state       - The global variable used to store a current
*                                 state of the software FSM.
*  `$INSTANCE_NAME`_mstrControl - The global variable used to control the master
*                                 end of a transaction with or without
*                                 Stop generation.
*  `$INSTANCE_NAME`_mstrRdBufPtr - The global variable used to store a pointer
*                                  to the master write buffer.
*  `$INSTANCE_NAME`_mstrRdBufIndex - The global variable  used to store a
*                                    current index within the master
*                                    write buffer.
*  `$INSTANCE_NAME`_mstrRdBufSize - The global variable used to store a master
*                                   write buffer size.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_MasterReadBuf(uint8 slaveAddress, uint8 * rdData, uint8 cnt, uint8 mode)
      `=ReentrantKeil($INSTANCE_NAME . "_MasterReadBuf")`
{
    uint8 errStatus;

    errStatus = `$INSTANCE_NAME`_MSTR_NOT_READY;

    if(NULL != rdData)
    {
        /* Check I2C state to allow transfer: valid states are IDLE or HALT */
        if(`$INSTANCE_NAME`_SM_IDLE == `$INSTANCE_NAME`_state)
        {
            /* Master is ready to transaction: check if bus is free */
            if(`$INSTANCE_NAME`_CHECK_BUS_FREE(`$INSTANCE_NAME`_MCSR_REG))
            {
                errStatus = `$INSTANCE_NAME`_MSTR_NO_ERROR;
            }
            else
            {
                errStatus = `$INSTANCE_NAME`_MSTR_BUS_BUSY;
            }
        }
        else if(`$INSTANCE_NAME`_SM_MSTR_HALT == `$INSTANCE_NAME`_state)
        {
            /* Master is ready and waiting for ReStart */
            errStatus = `$INSTANCE_NAME`_MSTR_NO_ERROR;

            `$INSTANCE_NAME`_ClearPendingInt();
            `$INSTANCE_NAME`_mstrStatus &= (uint8) ~`$INSTANCE_NAME`_MSTAT_XFER_HALT;
        }
        else
        {
            /* errStatus = `$INSTANCE_NAME`_MSTR_NOT_READY was set before */
        }

        if(`$INSTANCE_NAME`_MSTR_NO_ERROR == errStatus)
        {
            /* Set state to start write transaction */
            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_RD_ADDR;

            /* Prepare read buffer */
            `$INSTANCE_NAME`_mstrRdBufIndex  = 0u;
            `$INSTANCE_NAME`_mstrRdBufSize   = cnt;
            `$INSTANCE_NAME`_mstrRdBufPtr    = (volatile uint8 *) rdData;

            /* Set end of transaction flag: Stop or Halt (following ReStart) */
            `$INSTANCE_NAME`_mstrControl = mode;

            /* Clear read status history */
            `$INSTANCE_NAME`_mstrStatus &= (uint8) ~`$INSTANCE_NAME`_MSTAT_RD_CMPLT;

            /* Hardware actions: write address and generate Start or ReStart */
            `$INSTANCE_NAME`_DATA_REG = ((uint8) (slaveAddress << `$INSTANCE_NAME`_SLAVE_ADDR_SHIFT) |
                                                  `$INSTANCE_NAME`_READ_FLAG);

            if(`$INSTANCE_NAME`_CHECK_RESTART(mode))
            {
                `$INSTANCE_NAME`_GENERATE_RESTART;
            }
            else
            {
                `$INSTANCE_NAME`_GENERATE_START;
            }

            /* Enable interrupt to complete transfer */
            `$INSTANCE_NAME`_EnableInt();
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_MasterSendStart
********************************************************************************
*
* Summary:
*  Generates Start condition and sends slave address with read/write bit.
*
* Parameters:
*  slaveAddress:  7-bit slave address.
*  R_nW:          Zero, send write command, non-zero send read command.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  `$INSTANCE_NAME`_state - The global variable used to store a current state of
*                           the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_MasterSendStart(uint8 slaveAddress, uint8 R_nW)
      `=ReentrantKeil($INSTANCE_NAME . "_MasterSendStart")`
{
    uint8 errStatus;

    errStatus = `$INSTANCE_NAME`_MSTR_NOT_READY;

    /* If IDLE, check if bus is free */
    if(`$INSTANCE_NAME`_SM_IDLE == `$INSTANCE_NAME`_state)
    {
        /* If bus is free, generate Start condition */
        if(`$INSTANCE_NAME`_CHECK_BUS_FREE(`$INSTANCE_NAME`_MCSR_REG))
        {
            /* Disable interrupt for manual master operation */
            `$INSTANCE_NAME`_DisableInt();

            /* Set address and read/write flag */
            slaveAddress = (uint8) (slaveAddress << `$INSTANCE_NAME`_SLAVE_ADDR_SHIFT);
            if(0u != R_nW)
            {
                slaveAddress |= `$INSTANCE_NAME`_READ_FLAG;
                `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_RD_ADDR;
            }
            else
            {
                `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_WR_ADDR;
            }

            /* Hardware actions: write address and generate Start */
            `$INSTANCE_NAME`_DATA_REG = slaveAddress;
            `$INSTANCE_NAME`_GENERATE_START_MANUAL;

            /* Wait until address is transferred */
            while(`$INSTANCE_NAME`_WAIT_BYTE_COMPLETE(`$INSTANCE_NAME`_CSR_REG))
            {
            }

        #if(`$INSTANCE_NAME`_MODE_MULTI_MASTER_SLAVE_ENABLED)
            if(`$INSTANCE_NAME`_CHECK_START_GEN(`$INSTANCE_NAME`_MCSR_REG))
            {
                `$INSTANCE_NAME`_CLEAR_START_GEN;

                /* Start condition was not generated: reset FSM to IDLE */
                `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_IDLE;
                errStatus = `$INSTANCE_NAME`_MSTR_ERR_ABORT_START_GEN;
            }
            else
        #endif /* (`$INSTANCE_NAME`_MODE_MULTI_MASTER_SLAVE_ENABLED) */

        #if(`$INSTANCE_NAME`_MODE_MULTI_MASTER_ENABLED)
            if(`$INSTANCE_NAME`_CHECK_LOST_ARB(`$INSTANCE_NAME`_CSR_REG))
            {
                `$INSTANCE_NAME`_BUS_RELEASE_MANUAL;

                /* Master lost arbitrage: reset FSM to IDLE */
                `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_IDLE;
                errStatus = `$INSTANCE_NAME`_MSTR_ERR_ARB_LOST;
            }
            else
        #endif /* (`$INSTANCE_NAME`_MODE_MULTI_MASTER_ENABLED) */

            if(`$INSTANCE_NAME`_CHECK_ADDR_NAK(`$INSTANCE_NAME`_CSR_REG))
            {
                /* Address has been NACKed: reset FSM to IDLE */
                `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_IDLE;
                errStatus = `$INSTANCE_NAME`_MSTR_ERR_LB_NAK;
            }
            else
            {
                /* Start was sent without errors */
                errStatus = `$INSTANCE_NAME`_MSTR_NO_ERROR;
            }
        }
        else
        {
            errStatus = `$INSTANCE_NAME`_MSTR_BUS_BUSY;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_MasterSendRestart
********************************************************************************
*
* Summary:
*  Generates ReStart condition and sends slave address with read/write bit.
*
* Parameters:
*  slaveAddress:  7-bit slave address.
*  R_nW:          Zero, send write command, non-zero send read command.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  `$INSTANCE_NAME`_state - The global variable used to store a current state of
*                           the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_MasterSendRestart(uint8 slaveAddress, uint8 R_nW)
      `=ReentrantKeil($INSTANCE_NAME . "_MasterSendRestart")`
{
    uint8 errStatus;

    errStatus = `$INSTANCE_NAME`_MSTR_NOT_READY;

    /* Check if START condition was generated */
    if(`$INSTANCE_NAME`_CHECK_MASTER_MODE(`$INSTANCE_NAME`_MCSR_REG))
    {
        /* Set address and read/write flag */
        slaveAddress = (uint8) (slaveAddress << `$INSTANCE_NAME`_SLAVE_ADDR_SHIFT);
        if(0u != R_nW)
        {
            slaveAddress |= `$INSTANCE_NAME`_READ_FLAG;
            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_RD_ADDR;
        }
        else
        {
            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_WR_ADDR;
        }

        /* Hardware actions: write address and generate ReStart */
        `$INSTANCE_NAME`_DATA_REG = slaveAddress;
        `$INSTANCE_NAME`_GENERATE_RESTART_MANUAL;

        /* Wait until address has been transferred */
        while(`$INSTANCE_NAME`_WAIT_BYTE_COMPLETE(`$INSTANCE_NAME`_CSR_REG))
        {
        }

    #if(`$INSTANCE_NAME`_MODE_MULTI_MASTER_ENABLED)
        if(`$INSTANCE_NAME`_CHECK_LOST_ARB(`$INSTANCE_NAME`_CSR_REG))
        {
            `$INSTANCE_NAME`_BUS_RELEASE_MANUAL;

            /* Master lost arbitrage: reset FSM to IDLE */
            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_IDLE;
            errStatus = `$INSTANCE_NAME`_MSTR_ERR_ARB_LOST;
        }
        else
    #endif /* (`$INSTANCE_NAME`_MODE_MULTI_MASTER_ENABLED) */

        if(`$INSTANCE_NAME`_CHECK_ADDR_NAK(`$INSTANCE_NAME`_CSR_REG))
        {
            /* Address has been NACKed: reset FSM to IDLE */
            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_IDLE;
            errStatus = `$INSTANCE_NAME`_MSTR_ERR_LB_NAK;
        }
        else
        {
            /* ReStart was sent without errors */
            errStatus = `$INSTANCE_NAME`_MSTR_NO_ERROR;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_MasterSendStop
********************************************************************************
*
* Summary:
*  Generates I2C Stop condition on bus. Function do nothing if Start or Restart
*  condition was failed before call this function.
*
* Parameters:
*  None.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  Stop generation is required to complete the transaction.
*  This function does not wait until a Stop condition is generated.
*
* Global variables:
*  `$INSTANCE_NAME`_state - The global variable used to store a current state of
*                           the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_MasterSendStop(void) `=ReentrantKeil($INSTANCE_NAME . "_MasterSendStop")`
{
    uint8 errStatus;

    errStatus = `$INSTANCE_NAME`_MSTR_NOT_READY;

    /* Check if master is active on bus */
    if(`$INSTANCE_NAME`_CHECK_MASTER_MODE(`$INSTANCE_NAME`_MCSR_REG))
    {
        `$INSTANCE_NAME`_GENERATE_STOP_MANUAL;
        `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_IDLE;

        /* Wait until stop has been generated */
        while(`$INSTANCE_NAME`_WAIT_STOP_COMPLETE(`$INSTANCE_NAME`_CSR_REG))
        {
        }

        errStatus = `$INSTANCE_NAME`_MSTR_NO_ERROR;

    #if(`$INSTANCE_NAME`_MODE_MULTI_MASTER_ENABLED)
        if(`$INSTANCE_NAME`_CHECK_LOST_ARB(`$INSTANCE_NAME`_CSR_REG))
        {
            `$INSTANCE_NAME`_BUS_RELEASE_MANUAL;

            /* NACK was generated by instead Stop */
            errStatus = `$INSTANCE_NAME`_MSTR_ERR_ARB_LOST;
        }
    #endif /* (`$INSTANCE_NAME`_MODE_MULTI_MASTER_ENABLED) */
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_MasterWriteByte
********************************************************************************
*
* Summary:
*  Sends one byte to a slave. A valid Start or ReStart condition must be
*  generated before this call this function. Function do nothing if Start or
*  Restart condition was failed before call this function.
*
* Parameters:
*  data:  The data byte to send to the slave.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  `$INSTANCE_NAME`_state - The global variable used to store a current state of
*                           the software FSM.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_MasterWriteByte(uint8 theByte) `=ReentrantKeil($INSTANCE_NAME . "_MasterWriteByte")`
{
    uint8 errStatus;

    errStatus = `$INSTANCE_NAME`_MSTR_NOT_READY;

    /* Check if START condition was generated */
    if(`$INSTANCE_NAME`_CHECK_MASTER_MODE(`$INSTANCE_NAME`_MCSR_REG))
    {
        `$INSTANCE_NAME`_DATA_REG = theByte;   /* Write DATA register */
        `$INSTANCE_NAME`_TRANSMIT_DATA_MANUAL; /* Set transmit mode   */
        `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_WR_DATA;

        /* Wait until data byte has been transmitted */
        while(`$INSTANCE_NAME`_WAIT_BYTE_COMPLETE(`$INSTANCE_NAME`_CSR_REG))
        {
        }

    #if(`$INSTANCE_NAME`_MODE_MULTI_MASTER_ENABLED)
        if(`$INSTANCE_NAME`_CHECK_LOST_ARB(`$INSTANCE_NAME`_CSR_REG))
        {
            `$INSTANCE_NAME`_BUS_RELEASE_MANUAL;

            /* Master lost arbitrage: reset FSM to IDLE */
            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_IDLE;
            errStatus = `$INSTANCE_NAME`_MSTR_ERR_ARB_LOST;
        }
        /* Check LRB bit */
        else
    #endif /* (`$INSTANCE_NAME`_MODE_MULTI_MASTER_ENABLED) */

        if(`$INSTANCE_NAME`_CHECK_DATA_ACK(`$INSTANCE_NAME`_CSR_REG))
        {
            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_HALT;
            errStatus = `$INSTANCE_NAME`_MSTR_NO_ERROR;
        }
        else
        {
            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_HALT;
            errStatus = `$INSTANCE_NAME`_MSTR_ERR_LB_NAK;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_MasterReadByte
********************************************************************************
*
* Summary:
*  Reads one byte from a slave and ACK or NACK the transfer. A valid Start or
*  ReStart condition must be generated before this call this function. Function
*  do nothing if Start or Restart condition was failed before call this
*  function.
*
* Parameters:
*  acknNack:  Zero, response with NACK, if non-zero response with ACK.
*
* Return:
*  Byte read from slave.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  `$INSTANCE_NAME`_state - The global variable used to store a current
*                           state of the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_MasterReadByte(uint8 acknNak) `=ReentrantKeil($INSTANCE_NAME . "_MasterReadByte")`
{
    uint8 theByte;

    theByte = 0u;

    /* Check if START condition was generated */
    if(`$INSTANCE_NAME`_CHECK_MASTER_MODE(`$INSTANCE_NAME`_MCSR_REG))
    {
        /* When address phase needs to release bus and receive byte,
        * then decide ACK or NACK
        */
        if(`$INSTANCE_NAME`_SM_MSTR_RD_ADDR == `$INSTANCE_NAME`_state)
        {
            `$INSTANCE_NAME`_READY_TO_READ_MANUAL;
            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_RD_DATA;
        }

        /* Wait until data byte has been received */
        while(`$INSTANCE_NAME`_WAIT_BYTE_COMPLETE(`$INSTANCE_NAME`_CSR_REG))
        {
        }

        theByte = `$INSTANCE_NAME`_DATA_REG;

        /* Command ACK to receive next byte and continue transfer.
        *  Do nothing for NACK. The NACK will be generated by
        *  Stop or ReStart routine.
        */
        if(acknNak != 0u) /* Generate ACK */
        {
            `$INSTANCE_NAME`_ACK_AND_RECEIVE_MANUAL;
        }
        else              /* Do nothing for the follwong NACK */
        {
            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_HALT;
        }
    }

    return(theByte);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_MasterStatus
********************************************************************************
*
* Summary:
*  Returns the master's communication status.
*
* Parameters:
*  None.
*
* Return:
*  Current status of I2C master.
*
* Global variables:
*  `$INSTANCE_NAME`_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_MasterStatus(void) `=ReentrantKeil($INSTANCE_NAME . "_MasterStatus")`
{
    uint8 status;

    `$INSTANCE_NAME`_DisableInt(); /* Lock from interrupt */

    /* Read master status */
    status = `$INSTANCE_NAME`_mstrStatus;

    if (`$INSTANCE_NAME`_CHECK_SM_MASTER)
    {
        /* Set transfer in progress flag in status */
        status |= `$INSTANCE_NAME`_MSTAT_XFER_INP;
    }

    `$INSTANCE_NAME`_EnableInt(); /* Release lock */

    return (status);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_MasterClearStatus
********************************************************************************
*
* Summary:
*  Clears all status flags and returns the master status.
*
* Parameters:
*  None.
*
* Return:
*  Current status of I2C master.
*
* Global variables:
*  `$INSTANCE_NAME`_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_MasterClearStatus(void) `=ReentrantKeil($INSTANCE_NAME . "_MasterClearStatus")`
{
    uint8 status;

    `$INSTANCE_NAME`_DisableInt(); /* Lock from interrupt */

    /* Read and clear master status */
    status = `$INSTANCE_NAME`_mstrStatus;
    `$INSTANCE_NAME`_mstrStatus = `$INSTANCE_NAME`_MSTAT_CLEAR;

    `$INSTANCE_NAME`_EnableInt(); /* Release lock */

    return (status);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_MasterGetReadBufSize
********************************************************************************
*
* Summary:
*  Returns the amount of bytes that has been transferred with an
*  I2C_MasterReadBuf command.
*
* Parameters:
*  None.
*
* Return:
*  Byte count of transfer. If the transfer is not yet complete, it will return
*  the byte count transferred so far.
*
* Global variables:
*  `$INSTANCE_NAME`_mstrRdBufIndex - The global variable stores current index
*                                    within the master read buffer.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_MasterGetReadBufSize(void) `=ReentrantKeil($INSTANCE_NAME . "_MasterGetReadBufSize")`
{
    return (`$INSTANCE_NAME`_mstrRdBufIndex);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_MasterGetWriteBufSize
********************************************************************************
*
* Summary:
*  Returns the amount of bytes that has been transferred with an
*  I2C_MasterWriteBuf command.
*
* Parameters:
*  None.
*
* Return:
*  Byte count of transfer. If the transfer is not yet complete, it will return
*  the byte count transferred so far.
*
* Global variables:
*  `$INSTANCE_NAME`_mstrWrBufIndex -  The global variable used to stores current
*                                     index within master write buffer.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_MasterGetWriteBufSize(void) `=ReentrantKeil($INSTANCE_NAME . "_MasterGetWriteBufSize")`
{
    return (`$INSTANCE_NAME`_mstrWrBufIndex);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_MasterClearReadBuf
********************************************************************************
*
* Summary:
*  Resets the read buffer pointer back to the first byte in the buffer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  `$INSTANCE_NAME`_mstrRdBufIndex - The global variable used to stores current
*                                    index within master read buffer.
*  `$INSTANCE_NAME`_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_MasterClearReadBuf(void) `=ReentrantKeil($INSTANCE_NAME . "_MasterClearReadBuf")`
{
    `$INSTANCE_NAME`_DisableInt(); /* Lock from interrupt */

    `$INSTANCE_NAME`_mstrRdBufIndex = 0u;
    `$INSTANCE_NAME`_mstrStatus    &= (uint8) ~`$INSTANCE_NAME`_MSTAT_RD_CMPLT;

    `$INSTANCE_NAME`_EnableInt(); /* Release lock */
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_MasterClearWriteBuf
********************************************************************************
*
* Summary:
*  Resets the write buffer pointer back to the first byte in the buffer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  `$INSTANCE_NAME`_mstrRdBufIndex - The global variable used to stote current
*                                    index within master read buffer.
*  `$INSTANCE_NAME`_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_MasterClearWriteBuf(void) `=ReentrantKeil($INSTANCE_NAME . "_MasterClearWriteBuf")`
{
    `$INSTANCE_NAME`_DisableInt(); /* Lock from interrupt */

    `$INSTANCE_NAME`_mstrWrBufIndex = 0u;
    `$INSTANCE_NAME`_mstrStatus    &= (uint8) ~`$INSTANCE_NAME`_MSTAT_WR_CMPLT;

    `$INSTANCE_NAME`_EnableInt(); /* Release lock */
}

#endif /* (`$INSTANCE_NAME`_MODE_MASTER_ENABLED) */


/* [] END OF FILE */
