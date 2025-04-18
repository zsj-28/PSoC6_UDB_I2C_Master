/*******************************************************************************
* File Name: `$INSTANCE_NAME`_SLAVE.c
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file provides the source code of APIs for the I2C component Slave mode.
*
* Note:
*
*******************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_PVT.h"

#if (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED)

/**********************************
*      System variables
**********************************/

volatile uint8 `$INSTANCE_NAME`_slStatus;   /* Slave Status  */

/* Transmit buffer variables */
volatile uint8 * `$INSTANCE_NAME`_slRdBufPtr;   /* Pointer to Transmit buffer  */
volatile uint8   `$INSTANCE_NAME`_slRdBufSize;  /* Slave Transmit buffer size  */
volatile uint8   `$INSTANCE_NAME`_slRdBufIndex; /* Slave Transmit buffer Index */

/* Receive buffer variables */
volatile uint8 * `$INSTANCE_NAME`_slWrBufPtr;   /* Pointer to Receive buffer  */
volatile uint8   `$INSTANCE_NAME`_slWrBufSize;  /* Slave Receive buffer size  */
volatile uint8   `$INSTANCE_NAME`_slWrBufIndex; /* Slave Receive buffer Index */

#if (`$INSTANCE_NAME`_SW_ADRR_DECODE)
    volatile uint8 `$INSTANCE_NAME`_slAddress;  /* Software address variable */
#endif /* (`$INSTANCE_NAME`_SW_ADRR_DECODE) */


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SlaveStatus
********************************************************************************
*
* Summary:
*  Returns I2C slave's communication status.
*
* Parameters:
*  None.
*
* Return:
*  Current status of I2C slave.
*
* Global variables:
*  `$INSTANCE_NAME`_slStatus - The global variable used to store a current
*                              status of the I2C slave.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_SlaveStatus(void) `=ReentrantKeil($INSTANCE_NAME . "_SlaveStatus")`
{
    return (`$INSTANCE_NAME`_slStatus);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SlaveClearReadStatus
********************************************************************************
*
* Summary:
*  Clears the read status flags and returns they values.
*  The `$INSTANCE_NAME`_SSTAT_RD_BUSY flag is not effected by clear.
*
* Parameters:
*  None.
*
* Return:
*  Current read status of I2C slave.
*
* Global variables:
*  `$INSTANCE_NAME`_slStatus - The global variable used to store a current
*                              status of the I2C slave.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_SlaveClearReadStatus(void) `=ReentrantKeil($INSTANCE_NAME . "_SlaveClearReadStatus")`
{
    uint8 status;

    `$INSTANCE_NAME`_DisableInt(); /* Lock from interrupt */

    /* Mask of transfer complete flag and Error status */
    status = (`$INSTANCE_NAME`_slStatus & `$INSTANCE_NAME`_SSTAT_RD_MASK);
    `$INSTANCE_NAME`_slStatus &= (uint8) ~`$INSTANCE_NAME`_SSTAT_RD_CLEAR;

    `$INSTANCE_NAME`_EnableInt(); /* Release lock */

    return (status);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SlaveClearWriteStatus
********************************************************************************
*
* Summary:
*  Clears the write status flags and returns they values.
*  The `$INSTANCE_NAME`_SSTAT_WR_BUSY flag is not effected by clear.
*
* Parameters:
*  None.
*
* Return:
*  Current write status of I2C slave.
*
* Global variables:
*  `$INSTANCE_NAME`_slStatus - The global variable used to store a current
*                              status of the I2C slave.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_SlaveClearWriteStatus(void) `=ReentrantKeil($INSTANCE_NAME . "_SlaveClearWriteStatus")`
{
    uint8 status;

    `$INSTANCE_NAME`_DisableInt(); /* Lock from interrupt */

    /* Mask of transfer complete flag and Error status */
    status = (`$INSTANCE_NAME`_slStatus & `$INSTANCE_NAME`_SSTAT_WR_MASK);
    `$INSTANCE_NAME`_slStatus &= (uint8) ~`$INSTANCE_NAME`_SSTAT_WR_CLEAR;

    `$INSTANCE_NAME`_EnableInt(); /* Release lock */

    return (status);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SlaveSetAddress
********************************************************************************
*
* Summary:
*  Sets the I2C slave address.
*
* Parameters:
*  address: I2C slave address for the primary device. This value may be any
*  address between 0 and 127.
*
* Return:
*  None.
*
* Global variables:
*  `$INSTANCE_NAME`_Address  - The global variable used to store an I2C slave
*                              address for the primary device when the software
*                              address detect feature is used.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_SlaveSetAddress(uint8 address) `=ReentrantKeil($INSTANCE_NAME . "_SlaveSetAddress")`
{
#if (`$INSTANCE_NAME`_SW_ADRR_DECODE)
    `$INSTANCE_NAME`_slAddress = (address & `$INSTANCE_NAME`_SLAVE_ADDR_MASK);
#else
    `$INSTANCE_NAME`_ADDR_REG  = (address & `$INSTANCE_NAME`_SLAVE_ADDR_MASK);
#endif /* (`$INSTANCE_NAME`_SW_ADRR_DECODE) */
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SlaveInitReadBuf
********************************************************************************
*
* Summary:
*  Sets the buffer pointer and size of the read buffer. This function also
*  resets the transfer count returned with the I2C_SlaveGetReadBufSize function.
*
* Parameters:
*  readBuf:  Pointer to the data buffer to be read by the master.
*  bufSize:  Size of the read buffer exposed to the I2C master.
*
* Return:
*  None.
*
* Global variables:
*  `$INSTANCE_NAME`_slRdBufPtr   - The global variable used to store a pointer
*                                  to the slave read buffer.
*  `$INSTANCE_NAME`_slRdBufSize  - The global variable used to store a slave
*                                  read buffer size.
*  `$INSTANCE_NAME`_slRdBufIndex - The global variable used to store a current
*                                  index within the slave read buffer.
*
* Side Effects:
*  If this function is called during a bus transaction, data from the previous
*  buffer location and the beginning of the current buffer may be transmitted.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_SlaveInitReadBuf(uint8 * rdBuf, uint8 bufSize)
     `=ReentrantKeil($INSTANCE_NAME . "_SlaveInitReadBuf")`
{
    if (NULL != rdBuf)
    {
        `$INSTANCE_NAME`_DisableInt(); /* Lock from interrupt */

        /* Set buffer pointer */
        `$INSTANCE_NAME`_slRdBufPtr   = (volatile uint8 *) rdBuf;
        `$INSTANCE_NAME`_slRdBufSize  = bufSize;    /* Set buffer size     */
        `$INSTANCE_NAME`_slRdBufIndex = 0u;         /* Clears buffer index */

        `$INSTANCE_NAME`_EnableInt(); /* Release lock */
    }
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SlaveInitWriteBuf
********************************************************************************
*
* Summary:
*  Sets the buffer pointer and size of the read buffer. This function also
*  resets the transfer count returned with the I2C_SlaveGetReadBufSize function.
*
* Parameters:
*  writeBuf:  Pointer to the data buffer to be read by the master.
*  bufSize:  Size of the buffer exposed to the I2C master.
*
* Return:
*  None.
*
* Global variables:
*  `$INSTANCE_NAME`_slWrBufPtr   - The global variable used to store a pointer
*                                  to the slave write buffer.
*  `$INSTANCE_NAME`_slWrBufSize  - The global variable used to store a slave
*                                  write buffer size.
*  `$INSTANCE_NAME`_slWrBufIndex - The global variable used to store a current
*                                  index within the slave write buffer.
*
* Side Effects:
*  If this function is called during a bus transaction, data from the previous
*  buffer location and the beginning of the current buffer may be transmitted.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_SlaveInitWriteBuf(uint8 * wrBuf, uint8 bufSize)
     `=ReentrantKeil($INSTANCE_NAME . "_SlaveInitWriteBuf")`
{
    if (NULL != wrBuf)
    {
        `$INSTANCE_NAME`_DisableInt(); /* Lock from interrupt */

         /* Set buffer pointer */
        `$INSTANCE_NAME`_slWrBufPtr   = (volatile uint8 *) wrBuf;
        `$INSTANCE_NAME`_slWrBufSize  = bufSize;    /* Set buffer size     */
        `$INSTANCE_NAME`_slWrBufIndex = 0u;         /* Clears buffer index */

        `$INSTANCE_NAME`_EnableInt(); /* Release lock */
    }
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SlaveGetReadBufSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes read by the I2C master since an
*  I2C_SlaveInitReadBuf or I2C_SlaveClearReadBuf function was executed.
*  The maximum return value will be the size of the read buffer.
*
* Parameters:
*  None.
*
* Return:
*  Bytes read by master.
*
* Global variables:
*  `$INSTANCE_NAME`_slRdBufIndex - The global variable used to store a current
*                                  index within the slave read buffer.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_SlaveGetReadBufSize(void) `=ReentrantKeil($INSTANCE_NAME . "_SlaveGetReadBufSize")`
{
    return (`$INSTANCE_NAME`_slRdBufIndex);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SlaveGetWriteBufSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes written by the I2C master since an
*  I2C_SlaveInitWriteBuf or I2C_SlaveClearWriteBuf function was executed.
*  The maximum return value will be the size of the write buffer.
*
* Parameters:
*  None.
*
* Return:
*  Bytes written by master.
*
* Global variables:
*  `$INSTANCE_NAME`_slWrBufIndex - The global variable used to store a current
*                                  index within the slave write buffer.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_SlaveGetWriteBufSize(void) `=ReentrantKeil($INSTANCE_NAME . "_SlaveGetWriteBufSize")`
{
    return (`$INSTANCE_NAME`_slWrBufIndex);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SlaveClearReadBuf
********************************************************************************
*
* Summary:
*  Resets the read pointer to the first byte in the read buffer. The next byte
*  read by the master will be the first byte in the read buffer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  `$INSTANCE_NAME`_slRdBufIndex - The global variable used to store a current
*                                  index the within slave read buffer.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_SlaveClearReadBuf(void) `=ReentrantKeil($INSTANCE_NAME . "_SlaveClearReadBuf")`
{
    `$INSTANCE_NAME`_slRdBufIndex = 0u;
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SlaveClearRxBuf
********************************************************************************
*
* Summary:
*  Resets the write pointer to the first byte in the write buffer. The next byte
*  written by the master will be the first byte in the write buffer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  `$INSTANCE_NAME`_slWrBufIndex - The global variable used to store a current
*                                  index within the slave write buffer.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_SlaveClearWriteBuf(void) `=ReentrantKeil($INSTANCE_NAME . "_SlaveClearWriteBuf")`
{
    `$INSTANCE_NAME`_slWrBufIndex = 0u;
}

#endif /* (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED) */


/* [] END OF FILE */
