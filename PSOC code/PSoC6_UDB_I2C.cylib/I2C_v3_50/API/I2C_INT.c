/*******************************************************************************
* File Name: `$INSTANCE_NAME`_INT.c
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file provides the source code of Interrupt Service Routine (ISR)
*  for the I2C component.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_PVT.h"
`$CY_API_CALLBACK_HEADER_INCLUDE`


/*******************************************************************************
*  Place your includes, defines and code here.
********************************************************************************/
/* `#START `$INSTANCE_NAME`_ISR_intc` */

/* `#END` */


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_ISR
********************************************************************************
*
* Summary:
*  The handler for the I2C interrupt. The slave and master operations are
*  handled here.
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
CY_ISR(`$INSTANCE_NAME`_ISR)
{
#if (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED)
   uint8  tmp8;
#endif  /* (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED) */

    uint8  tmpCsr;
    
#ifdef `$INSTANCE_NAME`_ISR_ENTRY_CALLBACK
    `$INSTANCE_NAME`_ISR_EntryCallback();
#endif /* `$INSTANCE_NAME`_ISR_ENTRY_CALLBACK */
    

#if(`$INSTANCE_NAME`_TIMEOUT_FF_ENABLED)
    if(0u != `$INSTANCE_NAME`_TimeoutGetStatus())
    {
        `$INSTANCE_NAME`_TimeoutReset();
        `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_EXIT_IDLE;
        /* `$INSTANCE_NAME`_CSR_REG should be cleared after reset */
    }
#endif /* (`$INSTANCE_NAME`_TIMEOUT_FF_ENABLED) */


    tmpCsr = `$INSTANCE_NAME`_CSR_REG;      /* Make copy as interrupts clear */

#if(`$INSTANCE_NAME`_MODE_MULTI_MASTER_SLAVE_ENABLED)
    if(`$INSTANCE_NAME`_CHECK_START_GEN(`$INSTANCE_NAME`_MCSR_REG))
    {
        `$INSTANCE_NAME`_CLEAR_START_GEN;

        /* Set transfer complete and error flags */
        `$INSTANCE_NAME`_mstrStatus |= (`$INSTANCE_NAME`_MSTAT_ERR_XFER |
                                        `$INSTANCE_NAME`_GET_MSTAT_CMPLT);

        /* Slave was addressed */
        `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_SLAVE;
    }
#endif /* (`$INSTANCE_NAME`_MODE_MULTI_MASTER_SLAVE_ENABLED) */


#if(`$INSTANCE_NAME`_MODE_MULTI_MASTER_ENABLED)
    if(`$INSTANCE_NAME`_CHECK_LOST_ARB(tmpCsr))
    {
        /* Set errors */
        `$INSTANCE_NAME`_mstrStatus |= (`$INSTANCE_NAME`_MSTAT_ERR_XFER     |
                                        `$INSTANCE_NAME`_MSTAT_ERR_ARB_LOST |
                                        `$INSTANCE_NAME`_GET_MSTAT_CMPLT);

        `$INSTANCE_NAME`_DISABLE_INT_ON_STOP; /* Interrupt on Stop is enabled by write */

        #if(`$INSTANCE_NAME`_MODE_MULTI_MASTER_SLAVE_ENABLED)
            if(`$INSTANCE_NAME`_CHECK_ADDRESS_STS(tmpCsr))
            {
                /* Slave was addressed */
                `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_SLAVE;
            }
            else
            {
                `$INSTANCE_NAME`_BUS_RELEASE;

                `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_EXIT_IDLE;
            }
        #else
            `$INSTANCE_NAME`_BUS_RELEASE;

            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_EXIT_IDLE;

        #endif /* (`$INSTANCE_NAME`_MODE_MULTI_MASTER_SLAVE_ENABLED) */
    }
#endif /* (`$INSTANCE_NAME`_MODE_MULTI_MASTER_ENABLED) */

    /* Check for master operation mode */
    if(`$INSTANCE_NAME`_CHECK_SM_MASTER)
    {
    #if(`$INSTANCE_NAME`_MODE_MASTER_ENABLED)
        if(`$INSTANCE_NAME`_CHECK_BYTE_COMPLETE(tmpCsr))
        {
            switch (`$INSTANCE_NAME`_state)
            {
            case `$INSTANCE_NAME`_SM_MSTR_WR_ADDR:  /* After address is sent, write data */
            case `$INSTANCE_NAME`_SM_MSTR_RD_ADDR:  /* After address is sent, read data */

                tmpCsr &= ((uint8) ~`$INSTANCE_NAME`_CSR_STOP_STATUS); /* Clear Stop bit history on address phase */

                if(`$INSTANCE_NAME`_CHECK_ADDR_ACK(tmpCsr))
                {
                    /* Setup for transmit or receive of data */
                    if(`$INSTANCE_NAME`_state == `$INSTANCE_NAME`_SM_MSTR_WR_ADDR)   /* TRANSMIT data */
                    {
                        /* Check if at least one byte to transfer */
                        if(`$INSTANCE_NAME`_mstrWrBufSize > 0u)
                        {
                            /* Load the 1st data byte */
                            `$INSTANCE_NAME`_DATA_REG = `$INSTANCE_NAME`_mstrWrBufPtr[0u];
                            `$INSTANCE_NAME`_TRANSMIT_DATA;
                            `$INSTANCE_NAME`_mstrWrBufIndex = 1u;   /* Set index to 2nd element */

                            /* Set transmit state until done */
                            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_WR_DATA;
                        }
                        /* End of buffer: complete writing */
                        else if(`$INSTANCE_NAME`_CHECK_NO_STOP(`$INSTANCE_NAME`_mstrControl))
                        {
                            /* Set write complete and master halted */
                            `$INSTANCE_NAME`_mstrStatus |= (`$INSTANCE_NAME`_MSTAT_XFER_HALT |
                                                            `$INSTANCE_NAME`_MSTAT_WR_CMPLT);

                            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_HALT; /* Expect ReStart */
                            `$INSTANCE_NAME`_DisableInt();
                        }
                        else
                        {
                            `$INSTANCE_NAME`_ENABLE_INT_ON_STOP; /* Enable interrupt on Stop, to catch it */
                            `$INSTANCE_NAME`_GENERATE_STOP;
                        }
                    }
                    else  /* Master receive data */
                    {
                        `$INSTANCE_NAME`_READY_TO_READ; /* Release bus to read data */

                        `$INSTANCE_NAME`_state  = `$INSTANCE_NAME`_SM_MSTR_RD_DATA;
                    }
                }
                /* Address is NACKed */
                else if(`$INSTANCE_NAME`_CHECK_ADDR_NAK(tmpCsr))
                {
                    /* Set Address NAK error */
                    `$INSTANCE_NAME`_mstrStatus |= (`$INSTANCE_NAME`_MSTAT_ERR_XFER |
                                                    `$INSTANCE_NAME`_MSTAT_ERR_ADDR_NAK);

                    if(`$INSTANCE_NAME`_CHECK_NO_STOP(`$INSTANCE_NAME`_mstrControl))
                    {
                        `$INSTANCE_NAME`_mstrStatus |= (`$INSTANCE_NAME`_MSTAT_XFER_HALT |
                                                        `$INSTANCE_NAME`_GET_MSTAT_CMPLT);

                        `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_HALT; /* Expect RESTART */
                        `$INSTANCE_NAME`_DisableInt();
                    }
                    else  /* Do normal Stop */
                    {
                        `$INSTANCE_NAME`_ENABLE_INT_ON_STOP; /* Enable interrupt on Stop, to catch it */
                        `$INSTANCE_NAME`_GENERATE_STOP;
                    }
                }
                else
                {
                    /* Address phase is not set for some reason: error */
                    #if(`$INSTANCE_NAME`_TIMEOUT_ENABLED)
                        /* Exit interrupt to take chance for timeout timer to handle this case */
                        `$INSTANCE_NAME`_DisableInt();
                        `$INSTANCE_NAME`_ClearPendingInt();
                    #else
                        /* Block execution flow: unexpected condition */
                        CY_ASSERT(0u != 0u);
                    #endif /* (`$INSTANCE_NAME`_TIMEOUT_ENABLED) */
                }
                break;

            case `$INSTANCE_NAME`_SM_MSTR_WR_DATA:

                if(`$INSTANCE_NAME`_CHECK_DATA_ACK(tmpCsr))
                {
                    /* Check if end of buffer */
                    if(`$INSTANCE_NAME`_mstrWrBufIndex  < `$INSTANCE_NAME`_mstrWrBufSize)
                    {
                        `$INSTANCE_NAME`_DATA_REG =
                                                 `$INSTANCE_NAME`_mstrWrBufPtr[`$INSTANCE_NAME`_mstrWrBufIndex];
                        `$INSTANCE_NAME`_TRANSMIT_DATA;
                        `$INSTANCE_NAME`_mstrWrBufIndex++;
                    }
                    /* End of buffer: complete writing */
                    else if(`$INSTANCE_NAME`_CHECK_NO_STOP(`$INSTANCE_NAME`_mstrControl))
                    {
                        /* Set write complete and master halted */
                        `$INSTANCE_NAME`_mstrStatus |= (`$INSTANCE_NAME`_MSTAT_XFER_HALT |
                                                        `$INSTANCE_NAME`_MSTAT_WR_CMPLT);

                        `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_HALT;    /* Expect restart */
                        `$INSTANCE_NAME`_DisableInt();
                    }
                    else  /* Do normal Stop */
                    {
                        `$INSTANCE_NAME`_ENABLE_INT_ON_STOP;    /* Enable interrupt on Stop, to catch it */
                        `$INSTANCE_NAME`_GENERATE_STOP;
                    }
                }
                /* Last byte NAKed: end writing */
                else if(`$INSTANCE_NAME`_CHECK_NO_STOP(`$INSTANCE_NAME`_mstrControl))
                {
                    /* Set write complete, short transfer and master halted */
                    `$INSTANCE_NAME`_mstrStatus |= (`$INSTANCE_NAME`_MSTAT_ERR_XFER       |
                                                    `$INSTANCE_NAME`_MSTAT_ERR_SHORT_XFER |
                                                    `$INSTANCE_NAME`_MSTAT_XFER_HALT      |
                                                    `$INSTANCE_NAME`_MSTAT_WR_CMPLT);

                    `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_HALT;    /* Expect ReStart */
                    `$INSTANCE_NAME`_DisableInt();
                }
                else  /* Do normal Stop */
                {
                    `$INSTANCE_NAME`_ENABLE_INT_ON_STOP;    /* Enable interrupt on Stop, to catch it */
                    `$INSTANCE_NAME`_GENERATE_STOP;

                    /* Set short transfer and error flag */
                    `$INSTANCE_NAME`_mstrStatus |= (`$INSTANCE_NAME`_MSTAT_ERR_SHORT_XFER |
                                                    `$INSTANCE_NAME`_MSTAT_ERR_XFER);
                }

                break;

            case `$INSTANCE_NAME`_SM_MSTR_RD_DATA:

                `$INSTANCE_NAME`_mstrRdBufPtr[`$INSTANCE_NAME`_mstrRdBufIndex] = `$INSTANCE_NAME`_DATA_REG;
                `$INSTANCE_NAME`_mstrRdBufIndex++;

                /* Check if end of buffer */
                if(`$INSTANCE_NAME`_mstrRdBufIndex < `$INSTANCE_NAME`_mstrRdBufSize)
                {
                    `$INSTANCE_NAME`_ACK_AND_RECEIVE;       /* ACK and receive byte */
                }
                /* End of buffer: complete reading */
                else if(`$INSTANCE_NAME`_CHECK_NO_STOP(`$INSTANCE_NAME`_mstrControl))
                {
                    /* Set read complete and master halted */
                    `$INSTANCE_NAME`_mstrStatus |= (`$INSTANCE_NAME`_MSTAT_XFER_HALT |
                                                    `$INSTANCE_NAME`_MSTAT_RD_CMPLT);

                    `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_MSTR_HALT;    /* Expect ReStart */
                    `$INSTANCE_NAME`_DisableInt();
                }
                else
                {
                    `$INSTANCE_NAME`_ENABLE_INT_ON_STOP;
                    `$INSTANCE_NAME`_NAK_AND_RECEIVE;       /* NACK and TRY to generate Stop */
                }
                break;

            default: /* This is an invalid state and should not occur */

            #if(`$INSTANCE_NAME`_TIMEOUT_ENABLED)
                /* Exit interrupt to take chance for timeout timer to handles this case */
                `$INSTANCE_NAME`_DisableInt();
                `$INSTANCE_NAME`_ClearPendingInt();
            #else
                /* Block execution flow: unexpected condition */
                CY_ASSERT(0u != 0u);
            #endif /* (`$INSTANCE_NAME`_TIMEOUT_ENABLED) */

                break;
            }
        }

        /* Catches Stop: end of transaction */
        if(`$INSTANCE_NAME`_CHECK_STOP_STS(tmpCsr))
        {
            `$INSTANCE_NAME`_mstrStatus |= `$INSTANCE_NAME`_GET_MSTAT_CMPLT;

            `$INSTANCE_NAME`_DISABLE_INT_ON_STOP;
            `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_IDLE;
        }
    #endif /* (`$INSTANCE_NAME`_MODE_MASTER_ENABLED) */
    }
    else if(`$INSTANCE_NAME`_CHECK_SM_SLAVE)
    {
    #if(`$INSTANCE_NAME`_MODE_SLAVE_ENABLED)

        if((`$INSTANCE_NAME`_CHECK_STOP_STS(tmpCsr)) || /* Stop || Restart */
           (`$INSTANCE_NAME`_CHECK_BYTE_COMPLETE(tmpCsr) && `$INSTANCE_NAME`_CHECK_ADDRESS_STS(tmpCsr)))
        {
            /* Catch end of master write transaction: use interrupt on Stop */
            /* The Stop bit history on address phase does not have correct state */
            if(`$INSTANCE_NAME`_SM_SL_WR_DATA == `$INSTANCE_NAME`_state)
            {
                `$INSTANCE_NAME`_DISABLE_INT_ON_STOP;

                `$INSTANCE_NAME`_slStatus &= ((uint8) ~`$INSTANCE_NAME`_SSTAT_WR_BUSY);
                `$INSTANCE_NAME`_slStatus |= ((uint8)  `$INSTANCE_NAME`_SSTAT_WR_CMPLT);

                `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_IDLE;
            }
        }

        if(`$INSTANCE_NAME`_CHECK_BYTE_COMPLETE(tmpCsr))
        {
            /* The address only issued after Start or ReStart: so check the address
               to catch these events:
                FF : sets an address phase with a byte_complete interrupt trigger.
                UDB: sets an address phase immediately after Start or ReStart. */
            if(`$INSTANCE_NAME`_CHECK_ADDRESS_STS(tmpCsr))
            {
            /* Check for software address detection */
            #if(`$INSTANCE_NAME`_SW_ADRR_DECODE)
                tmp8 = `$INSTANCE_NAME`_GET_SLAVE_ADDR(`$INSTANCE_NAME`_DATA_REG);

                if(tmp8 == `$INSTANCE_NAME`_slAddress)   /* Check for address match */
                {
                    if(0u != (`$INSTANCE_NAME`_DATA_REG & `$INSTANCE_NAME`_READ_FLAG))
                    {
                        /* Place code to prepare read buffer here                  */
                        /* `#START `$INSTANCE_NAME`_SW_PREPARE_READ_BUF_interrupt` */

                        /* `#END` */

                    #ifdef `$INSTANCE_NAME`_SW_PREPARE_READ_BUF_CALLBACK
                        `$INSTANCE_NAME`_SwPrepareReadBuf_Callback();
                    #endif /* `$INSTANCE_NAME`_SW_PREPARE_READ_BUF_CALLBACK */
                        
                        /* Prepare next operation to read, get data and place in data register */
                        if(`$INSTANCE_NAME`_slRdBufIndex < `$INSTANCE_NAME`_slRdBufSize)
                        {
                            /* Load first data byte from array */
                            `$INSTANCE_NAME`_DATA_REG = `$INSTANCE_NAME`_slRdBufPtr[`$INSTANCE_NAME`_slRdBufIndex];
                            `$INSTANCE_NAME`_ACK_AND_TRANSMIT;
                            `$INSTANCE_NAME`_slRdBufIndex++;

                            `$INSTANCE_NAME`_slStatus |= `$INSTANCE_NAME`_SSTAT_RD_BUSY;
                        }
                        else    /* Overflow: provide 0xFF on bus */
                        {
                            `$INSTANCE_NAME`_DATA_REG = `$INSTANCE_NAME`_OVERFLOW_RETURN;
                            `$INSTANCE_NAME`_ACK_AND_TRANSMIT;

                            `$INSTANCE_NAME`_slStatus  |= (`$INSTANCE_NAME`_SSTAT_RD_BUSY |
                                                           `$INSTANCE_NAME`_SSTAT_RD_ERR_OVFL);
                        }

                        `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_SL_RD_DATA;
                    }
                    else  /* Write transaction: receive 1st byte */
                    {
                        `$INSTANCE_NAME`_ACK_AND_RECEIVE;
                        `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_SL_WR_DATA;

                        `$INSTANCE_NAME`_slStatus |= `$INSTANCE_NAME`_SSTAT_WR_BUSY;
                        `$INSTANCE_NAME`_ENABLE_INT_ON_STOP;
                    }
                }
                else
                {
                    /*     Place code to compare for additional address here    */
                    /* `#START `$INSTANCE_NAME`_SW_ADDR_COMPARE_interruptStart` */

                    /* `#END` */

                #ifdef `$INSTANCE_NAME`_SW_ADDR_COMPARE_ENTRY_CALLBACK
                    `$INSTANCE_NAME`_SwAddrCompare_EntryCallback();
                #endif /* `$INSTANCE_NAME`_SW_ADDR_COMPARE_ENTRY_CALLBACK */
                    
                    `$INSTANCE_NAME`_NAK_AND_RECEIVE;   /* NACK address */

                    /* Place code to end of condition for NACK generation here */
                    /* `#START `$INSTANCE_NAME`_SW_ADDR_COMPARE_interruptEnd`  */

                    /* `#END` */

                #ifdef `$INSTANCE_NAME`_SW_ADDR_COMPARE_EXIT_CALLBACK
                    `$INSTANCE_NAME`_SwAddrCompare_ExitCallback();
                #endif /* `$INSTANCE_NAME`_SW_ADDR_COMPARE_EXIT_CALLBACK */
                }

            #else /* (`$INSTANCE_NAME`_HW_ADRR_DECODE) */

                if(0u != (`$INSTANCE_NAME`_DATA_REG & `$INSTANCE_NAME`_READ_FLAG))
                {
                    /* Place code to prepare read buffer here                  */
                    /* `#START `$INSTANCE_NAME`_HW_PREPARE_READ_BUF_interrupt` */

                    /* `#END` */
                    
                #ifdef `$INSTANCE_NAME`_HW_PREPARE_READ_BUF_CALLBACK
                    `$INSTANCE_NAME`_HwPrepareReadBuf_Callback();
                #endif /* `$INSTANCE_NAME`_HW_PREPARE_READ_BUF_CALLBACK */

                    /* Prepare next operation to read, get data and place in data register */
                    if(`$INSTANCE_NAME`_slRdBufIndex < `$INSTANCE_NAME`_slRdBufSize)
                    {
                        /* Load first data byte from array */
                        `$INSTANCE_NAME`_DATA_REG = `$INSTANCE_NAME`_slRdBufPtr[`$INSTANCE_NAME`_slRdBufIndex];
                        `$INSTANCE_NAME`_ACK_AND_TRANSMIT;
                        `$INSTANCE_NAME`_slRdBufIndex++;

                        `$INSTANCE_NAME`_slStatus |= `$INSTANCE_NAME`_SSTAT_RD_BUSY;
                    }
                    else    /* Overflow: provide 0xFF on bus */
                    {
                        `$INSTANCE_NAME`_DATA_REG = `$INSTANCE_NAME`_OVERFLOW_RETURN;
                        `$INSTANCE_NAME`_ACK_AND_TRANSMIT;

                        `$INSTANCE_NAME`_slStatus  |= (`$INSTANCE_NAME`_SSTAT_RD_BUSY |
                                                       `$INSTANCE_NAME`_SSTAT_RD_ERR_OVFL);
                    }

                    `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_SL_RD_DATA;
                }
                else  /* Write transaction: receive 1st byte */
                {
                    `$INSTANCE_NAME`_ACK_AND_RECEIVE;
                    `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_SL_WR_DATA;

                    `$INSTANCE_NAME`_slStatus |= `$INSTANCE_NAME`_SSTAT_WR_BUSY;
                    `$INSTANCE_NAME`_ENABLE_INT_ON_STOP;
                }

            #endif /* (`$INSTANCE_NAME`_SW_ADRR_DECODE) */
            }
            /* Data states */
            /* Data master writes into slave */
            else if(`$INSTANCE_NAME`_state == `$INSTANCE_NAME`_SM_SL_WR_DATA)
            {
                if(`$INSTANCE_NAME`_slWrBufIndex < `$INSTANCE_NAME`_slWrBufSize)
                {
                    tmp8 = `$INSTANCE_NAME`_DATA_REG;
                    `$INSTANCE_NAME`_ACK_AND_RECEIVE;
                    `$INSTANCE_NAME`_slWrBufPtr[`$INSTANCE_NAME`_slWrBufIndex] = tmp8;
                    `$INSTANCE_NAME`_slWrBufIndex++;
                }
                else  /* of array: complete write, send NACK */
                {
                    `$INSTANCE_NAME`_NAK_AND_RECEIVE;

                    `$INSTANCE_NAME`_slStatus |= `$INSTANCE_NAME`_SSTAT_WR_ERR_OVFL;
                }
            }
            /* Data master reads from slave */
            else if(`$INSTANCE_NAME`_state == `$INSTANCE_NAME`_SM_SL_RD_DATA)
            {
                if(`$INSTANCE_NAME`_CHECK_DATA_ACK(tmpCsr))
                {
                    if(`$INSTANCE_NAME`_slRdBufIndex < `$INSTANCE_NAME`_slRdBufSize)
                    {
                         /* Get data from array */
                        `$INSTANCE_NAME`_DATA_REG = `$INSTANCE_NAME`_slRdBufPtr[`$INSTANCE_NAME`_slRdBufIndex];
                        `$INSTANCE_NAME`_TRANSMIT_DATA;
                        `$INSTANCE_NAME`_slRdBufIndex++;
                    }
                    else   /* Overflow: provide 0xFF on bus */
                    {
                        `$INSTANCE_NAME`_DATA_REG = `$INSTANCE_NAME`_OVERFLOW_RETURN;
                        `$INSTANCE_NAME`_TRANSMIT_DATA;

                        `$INSTANCE_NAME`_slStatus |= `$INSTANCE_NAME`_SSTAT_RD_ERR_OVFL;
                    }
                }
                else  /* Last byte was NACKed: read complete */
                {
                    /* Only NACK appears on bus */
                    `$INSTANCE_NAME`_DATA_REG = `$INSTANCE_NAME`_OVERFLOW_RETURN;
                    `$INSTANCE_NAME`_NAK_AND_TRANSMIT;

                    `$INSTANCE_NAME`_slStatus &= ((uint8) ~`$INSTANCE_NAME`_SSTAT_RD_BUSY);
                    `$INSTANCE_NAME`_slStatus |= ((uint8)  `$INSTANCE_NAME`_SSTAT_RD_CMPLT);

                    `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_IDLE;
                }
            }
            else
            {
            #if(`$INSTANCE_NAME`_TIMEOUT_ENABLED)
                /* Exit interrupt to take chance for timeout timer to handle this case */
                `$INSTANCE_NAME`_DisableInt();
                `$INSTANCE_NAME`_ClearPendingInt();
            #else
                /* Block execution flow: unexpected condition */
                CY_ASSERT(0u != 0u);
            #endif /* (`$INSTANCE_NAME`_TIMEOUT_ENABLED) */
            }
        }
    #endif /* (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED) */
    }
    else
    {
        /* The FSM skips master and slave processing: return to IDLE */
        `$INSTANCE_NAME`_state = `$INSTANCE_NAME`_SM_IDLE;
    }

#ifdef `$INSTANCE_NAME`_ISR_EXIT_CALLBACK
    `$INSTANCE_NAME`_ISR_ExitCallback();
#endif /* `$INSTANCE_NAME`_ISR_EXIT_CALLBACK */    
}


#if ((`$INSTANCE_NAME`_FF_IMPLEMENTED) && (`$INSTANCE_NAME`_WAKEUP_ENABLED))
    /*******************************************************************************
    * Function Name: `$INSTANCE_NAME`_WAKEUP_ISR
    ********************************************************************************
    *
    * Summary:
    *  The interrupt handler to trigger after a wakeup.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    CY_ISR(`$INSTANCE_NAME`_WAKEUP_ISR)
    {
    #ifdef `$INSTANCE_NAME`_WAKEUP_ISR_ENTRY_CALLBACK
        `$INSTANCE_NAME`_WAKEUP_ISR_EntryCallback();
    #endif /* `$INSTANCE_NAME`_WAKEUP_ISR_ENTRY_CALLBACK */
         
        /* Set flag to notify that matched address is received */
        `$INSTANCE_NAME`_wakeupSource = 1u;

        /* SCL is stretched until the I2C_Wake() is called */

    #ifdef `$INSTANCE_NAME`_WAKEUP_ISR_EXIT_CALLBACK
        `$INSTANCE_NAME`_WAKEUP_ISR_ExitCallback();
    #endif /* `$INSTANCE_NAME`_WAKEUP_ISR_EXIT_CALLBACK */
    }
#endif /* ((`$INSTANCE_NAME`_FF_IMPLEMENTED) && (`$INSTANCE_NAME`_WAKEUP_ENABLED)) */


/* [] END OF FILE */
