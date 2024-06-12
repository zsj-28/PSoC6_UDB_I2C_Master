/*******************************************************************************
* File Name: `$INSTANCE_NAME`.h
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file provides constants and parameter values for the I2C component.

*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_I2C_`$INSTANCE_NAME`_H)
#define CY_I2C_`$INSTANCE_NAME`_H

#include "cyfitter.h"
#include "syslib/cy_syslib.h"
#include "sysint/cy_sysint.h"
//#include "CyLib.h"

extern const cy_stc_sysint_t `$INSTANCE_NAME`_IRQ_cfg;

/***************************************
*   Conditional Compilation Parameters
****************************************/

#define `$INSTANCE_NAME`_IMPLEMENTATION     (`$Implementation`u)
#define `$INSTANCE_NAME`_MODE               (`$I2C_Mode`u)
#define `$INSTANCE_NAME`_ENABLE_WAKEUP      (`$EnableWakeup`u)
#define `$INSTANCE_NAME`_ADDR_DECODE        (`$Address_Decode`u)
#define `$INSTANCE_NAME`_UDB_INTRN_CLOCK    (`$UdbInternalClock`u)


/* I2C implementation enum */
#define `$INSTANCE_NAME`_UDB    (0x00u)
#define `$INSTANCE_NAME`_FF     (0x01u)

#define `$INSTANCE_NAME`_FF_IMPLEMENTED     (`$INSTANCE_NAME`_FF  == `$INSTANCE_NAME`_IMPLEMENTATION)
#define `$INSTANCE_NAME`_UDB_IMPLEMENTED    (`$INSTANCE_NAME`_UDB == `$INSTANCE_NAME`_IMPLEMENTATION)

#define `$INSTANCE_NAME`_UDB_INTRN_CLOCK_ENABLED    (`$INSTANCE_NAME`_UDB_IMPLEMENTED && \
                                                     (0u != `$INSTANCE_NAME`_UDB_INTRN_CLOCK))
/* I2C modes enum */
#define `$INSTANCE_NAME`_MODE_SLAVE                 (0x01u)
#define `$INSTANCE_NAME`_MODE_MASTER                (0x02u)
#define `$INSTANCE_NAME`_MODE_MULTI_MASTER          (0x06u)
#define `$INSTANCE_NAME`_MODE_MULTI_MASTER_SLAVE    (0x07u)
#define `$INSTANCE_NAME`_MODE_MULTI_MASTER_MASK     (0x04u)

#define `$INSTANCE_NAME`_MODE_SLAVE_ENABLED         (0u != (`$INSTANCE_NAME`_MODE_SLAVE  & `$INSTANCE_NAME`_MODE))
#define `$INSTANCE_NAME`_MODE_MASTER_ENABLED        (0u != (`$INSTANCE_NAME`_MODE_MASTER & `$INSTANCE_NAME`_MODE))
#define `$INSTANCE_NAME`_MODE_MULTI_MASTER_ENABLED  (0u != (`$INSTANCE_NAME`_MODE_MULTI_MASTER_MASK & \
                                                            `$INSTANCE_NAME`_MODE))
#define `$INSTANCE_NAME`_MODE_MULTI_MASTER_SLAVE_ENABLED    (`$INSTANCE_NAME`_MODE_MULTI_MASTER_SLAVE == \
                                                             `$INSTANCE_NAME`_MODE)

/* Address detection enum */
#define `$INSTANCE_NAME`_SW_DECODE      (0x00u)
#define `$INSTANCE_NAME`_HW_DECODE      (0x01u)

#define `$INSTANCE_NAME`_SW_ADRR_DECODE             (`$INSTANCE_NAME`_SW_DECODE == `$INSTANCE_NAME`_ADDR_DECODE)
#define `$INSTANCE_NAME`_HW_ADRR_DECODE             (`$INSTANCE_NAME`_HW_DECODE == `$INSTANCE_NAME`_ADDR_DECODE)

/* Wakeup enabled */
#define `$INSTANCE_NAME`_WAKEUP_ENABLED             (0u != `$INSTANCE_NAME`_ENABLE_WAKEUP)

/* Adds bootloader APIs to component */
#define `$INSTANCE_NAME`_BOOTLOADER_INTERFACE_ENABLED   (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED && \
                                                            ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_`$INSTANCE_NAME`) || \
                                                             (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface)))

/* Timeout functionality */
#define `$INSTANCE_NAME`_TIMEOUT_ENABLE             (`$TimeoutEnabled`u)
#define `$INSTANCE_NAME`_TIMEOUT_SCL_TMOUT_ENABLE   (`$SclTimeoutEnabled`u)
#define `$INSTANCE_NAME`_TIMEOUT_SDA_TMOUT_ENABLE   (`$SdaTimeoutEnabled`u)
#define `$INSTANCE_NAME`_TIMEOUT_PRESCALER_ENABLE   (`$PrescalerEnabled`u)
#define `$INSTANCE_NAME`_TIMEOUT_IMPLEMENTATION     (`$TimeoutImplementation`u)

/* Convert to boolean */
#define `$INSTANCE_NAME`_TIMEOUT_ENABLED            (0u != `$INSTANCE_NAME`_TIMEOUT_ENABLE)
#define `$INSTANCE_NAME`_TIMEOUT_SCL_TMOUT_ENABLED  (0u != `$INSTANCE_NAME`_TIMEOUT_SCL_TMOUT_ENABLE)
#define `$INSTANCE_NAME`_TIMEOUT_SDA_TMOUT_ENABLED  (0u != `$INSTANCE_NAME`_TIMEOUT_SDA_TMOUT_ENABLE)
#define `$INSTANCE_NAME`_TIMEOUT_PRESCALER_ENABLED  (0u != `$INSTANCE_NAME`_TIMEOUT_PRESCALER_ENABLE)

/* Timeout implementation enum. */
#define `$INSTANCE_NAME`_TIMEOUT_UDB    (0x00u)
#define `$INSTANCE_NAME`_TIMEOUT_FF     (0x01u)

#define `$INSTANCE_NAME`_TIMEOUT_FF_IMPLEMENTED     (`$INSTANCE_NAME`_TIMEOUT_FF  == \
                                                        `$INSTANCE_NAME`_TIMEOUT_IMPLEMENTATION)
#define `$INSTANCE_NAME`_TIMEOUT_UDB_IMPLEMENTED    (`$INSTANCE_NAME`_TIMEOUT_UDB == \
                                                        `$INSTANCE_NAME`_TIMEOUT_IMPLEMENTATION)

#define `$INSTANCE_NAME`_TIMEOUT_FF_ENABLED         (`$INSTANCE_NAME`_TIMEOUT_ENABLED && \
                                                     `$INSTANCE_NAME`_TIMEOUT_FF_IMPLEMENTED)

#define `$INSTANCE_NAME`_TIMEOUT_UDB_ENABLED        (`$INSTANCE_NAME`_TIMEOUT_ENABLED && \
                                                     `$INSTANCE_NAME`_TIMEOUT_UDB_IMPLEMENTED)

#define `$INSTANCE_NAME`_EXTERN_I2C_INTR_HANDLER    (`$Externi2cIntrHandler`u)
#define `$INSTANCE_NAME`_EXTERN_TMOUT_INTR_HANDLER  (`$ExternTmoutIntrHandler`u)

#define `$INSTANCE_NAME`_INTERN_I2C_INTR_HANDLER    (0u == `$INSTANCE_NAME`_EXTERN_I2C_INTR_HANDLER)
#define `$INSTANCE_NAME`_INTERN_TMOUT_INTR_HANDLER  (0u == `$INSTANCE_NAME`_EXTERN_TMOUT_INTR_HANDLER)


/***************************************
*       Type defines
***************************************/

/* Structure to save registers before go to sleep */
typedef struct
{
    uint8_t enableState;

#if (`$INSTANCE_NAME`_FF_IMPLEMENTED)
    uint8_t xcfg;
    uint8_t cfg;
    uint8_t addr;
    uint8_t clkDiv1;
    uint8_t clkDiv2;
#else
    uint8_t control;
#endif /* (`$INSTANCE_NAME`_FF_IMPLEMENTED) */

#if (`$INSTANCE_NAME`_TIMEOUT_ENABLED)
    uint16 tmoutCfg;
    uint8_t  tmoutIntr;
#endif /* (`$INSTANCE_NAME`_TIMEOUT_ENABLED) */

} `$INSTANCE_NAME`_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void `$INSTANCE_NAME`_Init(void)                            `=ReentrantKeil($INSTANCE_NAME . "_Init")`;
void `$INSTANCE_NAME`_Enable(void)                          `=ReentrantKeil($INSTANCE_NAME . "_Enable")`;

void `$INSTANCE_NAME`_Start(void)                           `=ReentrantKeil($INSTANCE_NAME . "_Start")`;
void `$INSTANCE_NAME`_Stop(void)                            `=ReentrantKeil($INSTANCE_NAME . "_Stop")`;

#define `$INSTANCE_NAME`_EnableInt()        NVIC_EnableIRQ      (`$INSTANCE_NAME`_ISR_NUMBER)
#define `$INSTANCE_NAME`_DisableInt()       NVIC_DisableIRQ     (`$INSTANCE_NAME`_ISR_NUMBER)
#define `$INSTANCE_NAME`_ClearPendingInt()  NVIC_ClearPendingIRQ(`$INSTANCE_NAME`_ISR_NUMBER)
#define `$INSTANCE_NAME`_SetPendingInt()    NVIC_SetPendingIRQ  (`$INSTANCE_NAME`_ISR_NUMBER)

void `$INSTANCE_NAME`_SaveConfig(void)                      `=ReentrantKeil($INSTANCE_NAME . "_SaveConfig")`;
void `$INSTANCE_NAME`_Sleep(void)                           `=ReentrantKeil($INSTANCE_NAME . "_Sleep")`;
void `$INSTANCE_NAME`_RestoreConfig(void)                   `=ReentrantKeil($INSTANCE_NAME . "_RestoreConfig")`;
void `$INSTANCE_NAME`_Wakeup(void)                          `=ReentrantKeil($INSTANCE_NAME . "_Wakeup")`;

/* I2C Master functions prototypes */
#if (`$INSTANCE_NAME`_MODE_MASTER_ENABLED)
    /* Read and Clear status functions */
    uint8_t `$INSTANCE_NAME`_MasterStatus(void)                `=ReentrantKeil($INSTANCE_NAME . "_MasterStatus")`;
    uint8_t `$INSTANCE_NAME`_MasterClearStatus(void)           `=ReentrantKeil($INSTANCE_NAME . "_MasterClearStatus")`;

    /* Interrupt based operation functions */
    uint8_t `$INSTANCE_NAME`_MasterWriteBuf(uint8_t slaveAddress, uint8_t * wrData, uint8_t cnt, uint8_t mode) \
                                                            `=ReentrantKeil($INSTANCE_NAME . "_MasterWriteBuf")`;
    uint8_t `$INSTANCE_NAME`_MasterReadBuf(uint8_t slaveAddress, uint8_t * rdData, uint8_t cnt, uint8_t mode) \
                                                            `=ReentrantKeil($INSTANCE_NAME . "_MasterReadBuf")`;
    uint8_t `$INSTANCE_NAME`_MasterGetReadBufSize(void)       `=ReentrantKeil($INSTANCE_NAME . "_MasterGetReadBufSize")`;
    uint8_t `$INSTANCE_NAME`_MasterGetWriteBufSize(void)      `=ReentrantKeil($INSTANCE_NAME . "_MasterGetWriteBufSize")`;
    void  `$INSTANCE_NAME`_MasterClearReadBuf(void)         `=ReentrantKeil($INSTANCE_NAME . "_MasterClearReadBuf")`;
    void  `$INSTANCE_NAME`_MasterClearWriteBuf(void)        `=ReentrantKeil($INSTANCE_NAME . "_MasterClearWriteBuf")`;

    /* Manual operation functions */
    uint8_t `$INSTANCE_NAME`_MasterSendStart(uint8_t slaveAddress, uint8_t R_nW) \
                                                            `=ReentrantKeil($INSTANCE_NAME . "_MasterSendStart")`;
    uint8_t `$INSTANCE_NAME`_MasterSendRestart(uint8_t slaveAddress, uint8_t R_nW) \
                                                            `=ReentrantKeil($INSTANCE_NAME . "_MasterSendRestart")`;
    uint8_t `$INSTANCE_NAME`_MasterSendStop(void)             `=ReentrantKeil($INSTANCE_NAME . "_MasterSendStop")`;
    uint8_t `$INSTANCE_NAME`_MasterWriteByte(uint8_t theByte)   `=ReentrantKeil($INSTANCE_NAME . "_MasterWriteByte")`;
    uint8_t `$INSTANCE_NAME`_MasterReadByte(uint8_t acknNak)    `=ReentrantKeil($INSTANCE_NAME . "_MasterReadByte")`;

#endif /* (`$INSTANCE_NAME`_MODE_MASTER_ENABLED) */

/* I2C Slave functions prototypes */
#if (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED)
    /* Read and Clear status functions */
    uint8_t `$INSTANCE_NAME`_SlaveStatus(void)                `=ReentrantKeil($INSTANCE_NAME . "_SlaveStatus")`;
    uint8_t `$INSTANCE_NAME`_SlaveClearReadStatus(void)       `=ReentrantKeil($INSTANCE_NAME . "_SlaveClearReadStatus")`;
    uint8_t `$INSTANCE_NAME`_SlaveClearWriteStatus(void)      `=ReentrantKeil($INSTANCE_NAME . "_SlaveClearWriteStatus")`;

    void  `$INSTANCE_NAME`_SlaveSetAddress(uint8_t address)   `=ReentrantKeil($INSTANCE_NAME . "_SlaveSetAddress")`;

    /* Interrupt based operation functions */
    void  `$INSTANCE_NAME`_SlaveInitReadBuf(uint8_t * rdBuf, uint8_t bufSize) \
                                                            `=ReentrantKeil($INSTANCE_NAME . "_SlaveInitReadBuf")`;
    void  `$INSTANCE_NAME`_SlaveInitWriteBuf(uint8_t * wrBuf, uint8_t bufSize) \
                                                            `=ReentrantKeil($INSTANCE_NAME . "_SlaveInitWriteBuf")`;
    uint8_t `$INSTANCE_NAME`_SlaveGetReadBufSize(void)        `=ReentrantKeil($INSTANCE_NAME . "_SlaveGetReadBufSize")`;
    uint8_t `$INSTANCE_NAME`_SlaveGetWriteBufSize(void)       `=ReentrantKeil($INSTANCE_NAME . "_SlaveGetWriteBufSize")`;
    void  `$INSTANCE_NAME`_SlaveClearReadBuf(void)          `=ReentrantKeil($INSTANCE_NAME . "_SlaveClearReadBuf")`;
    void  `$INSTANCE_NAME`_SlaveClearWriteBuf(void)         `=ReentrantKeil($INSTANCE_NAME . "_SlaveClearWriteBuf")`;

    /* Communication bootloader I2C Slave APIs */
    #if defined(CYDEV_BOOTLOADER_IO_COMP) && (`$INSTANCE_NAME`_BOOTLOADER_INTERFACE_ENABLED)
        /* Physical layer functions */
        void     `$INSTANCE_NAME`_CyBtldrCommStart(void) CYSMALL \
                                                            `=ReentrantKeil($INSTANCE_NAME . "_CyBtldrCommStart")`;
        void     `$INSTANCE_NAME`_CyBtldrCommStop(void)  CYSMALL \
                                                            `=ReentrantKeil($INSTANCE_NAME . "_CyBtldrCommStop")`;
        void     `$INSTANCE_NAME`_CyBtldrCommReset(void) CYSMALL \
                                                            `=ReentrantKeil($INSTANCE_NAME . "_CyBtldrCommReset")`;
        cystatus `$INSTANCE_NAME`_CyBtldrCommWrite(const uint8_t pData[], uint16 size, uint16 * count, uint8_t timeOut) \
                                                        CYSMALL `=ReentrantKeil($INSTANCE_NAME . "_CyBtldrCommWrite")`;
        cystatus `$INSTANCE_NAME`_CyBtldrCommRead(uint8_t pData[], uint16 size, uint16 * count, uint8_t timeOut)  CYSMALL \
                                                            `=ReentrantKeil($INSTANCE_NAME . "_CyBtldrCommRead")`;

        #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_`$INSTANCE_NAME`)
            #define CyBtldrCommStart    `$INSTANCE_NAME`_CyBtldrCommStart
            #define CyBtldrCommStop     `$INSTANCE_NAME`_CyBtldrCommStop
            #define CyBtldrCommReset    `$INSTANCE_NAME`_CyBtldrCommReset
            #define CyBtldrCommWrite    `$INSTANCE_NAME`_CyBtldrCommWrite
            #define CyBtldrCommRead     `$INSTANCE_NAME`_CyBtldrCommRead
        #endif /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_`$INSTANCE_NAME`) */

        /* Size of Read/Write buffers for I2C bootloader  */
        #define `$INSTANCE_NAME`_BTLDR_SIZEOF_READ_BUFFER   (0x80u)
        #define `$INSTANCE_NAME`_BTLDR_SIZEOF_WRITE_BUFFER  (0x80u)
        #define `$INSTANCE_NAME`_MIN_UNT16(a, b)            ( ((uint16)(a) < (b)) ? ((uint16) (a)) : ((uint16) (b)) )
        #define `$INSTANCE_NAME`_WAIT_1_MS                  (1u)

    #endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (`$INSTANCE_NAME`_BOOTLOADER_INTERFACE_ENABLED) */

#endif /* (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED) */

/* Component interrupt handlers */
//CY_ISR_PROTO(`$INSTANCE_NAME`_ISR);
void `$INSTANCE_NAME`_ISR (void);
#if ((`$INSTANCE_NAME`_FF_IMPLEMENTED) || (`$INSTANCE_NAME`_WAKEUP_ENABLED))
    //CY_ISR_PROTO(`$INSTANCE_NAME`_WAKEUP_ISR);
    void `$INSTANCE_NAME`_WAKEUP_ISR (void);
#endif /* ((`$INSTANCE_NAME`_FF_IMPLEMENTED) || (`$INSTANCE_NAME`_WAKEUP_ENABLED)) */


/**********************************
*   Variable with external linkage
**********************************/

extern uint8_t `$INSTANCE_NAME`_initVar;


/***************************************
*   Initial Parameter Constants
***************************************/

#define `$INSTANCE_NAME`_DATA_RATE          (`$BusSpeed_kHz`u)
#define `$INSTANCE_NAME`_DEFAULT_ADDR       (`$Slave_Address`u)
#define `$INSTANCE_NAME`_I2C_PAIR_SELECTED  (`$I2cBusPort`u)

/* I2C pair enum */
#define `$INSTANCE_NAME`_I2C_PAIR_ANY   (0x01u) /* Any pins for I2C */
#define `$INSTANCE_NAME`_I2C_PAIR0      (0x01u) /* I2C0: (SCL = P12[4]) && (SCL = P12[5]) */
#define `$INSTANCE_NAME`_I2C_PAIR1      (0x02u) /* I2C1: (SCL = P12[0]) && (SDA = P12[1]) */

#define `$INSTANCE_NAME`_I2C1_SIO_PAIR  (`$INSTANCE_NAME`_I2C_PAIR1 == `$INSTANCE_NAME`_I2C_PAIR_SELECTED)
#define `$INSTANCE_NAME`_I2C0_SIO_PAIR  (`$INSTANCE_NAME`_I2C_PAIR0 == `$INSTANCE_NAME`_I2C_PAIR_SELECTED)


/***************************************
*            API Constants
***************************************/

/* Master/Slave control constants */
#define `$INSTANCE_NAME`_READ_XFER_MODE     (0x01u) /* Read */
#define `$INSTANCE_NAME`_WRITE_XFER_MODE    (0x00u) /* Write */
#define `$INSTANCE_NAME`_ACK_DATA           (0x01u) /* Send ACK */
#define `$INSTANCE_NAME`_NAK_DATA           (0x00u) /* Send NAK */
#define `$INSTANCE_NAME`_OVERFLOW_RETURN    (0xFFu) /* Send on bus in case of overflow */

#if (`$INSTANCE_NAME`_MODE_MASTER_ENABLED)
    /* "Mode" constants for MasterWriteBuf() or MasterReadBuf() function */
    #define `$INSTANCE_NAME`_MODE_COMPLETE_XFER     (0x00u) /* Full transfer with Start and Stop */
    #define `$INSTANCE_NAME`_MODE_REPEAT_START      (0x01u) /* Begin with a ReStart instead of a Start */
    #define `$INSTANCE_NAME`_MODE_NO_STOP           (0x02u) /* Complete the transfer without a Stop */

    /* Master status */
    #define `$INSTANCE_NAME`_MSTAT_CLEAR            (0x00u) /* Clear (initialize) status value */

    #define `$INSTANCE_NAME`_MSTAT_RD_CMPLT         (0x01u) /* Read complete */
    #define `$INSTANCE_NAME`_MSTAT_WR_CMPLT         (0x02u) /* Write complete */
    #define `$INSTANCE_NAME`_MSTAT_XFER_INP         (0x04u) /* Master transfer in progress */
    #define `$INSTANCE_NAME`_MSTAT_XFER_HALT        (0x08u) /* Transfer is halted */

    #define `$INSTANCE_NAME`_MSTAT_ERR_MASK         (0xF0u) /* Mask for all errors */
    #define `$INSTANCE_NAME`_MSTAT_ERR_SHORT_XFER   (0x10u) /* Master NAKed before end of packet */
    #define `$INSTANCE_NAME`_MSTAT_ERR_ADDR_NAK     (0x20u) /* Slave did not ACK */
    #define `$INSTANCE_NAME`_MSTAT_ERR_ARB_LOST     (0x40u) /* Master lost arbitration during communication */
    #define `$INSTANCE_NAME`_MSTAT_ERR_XFER         (0x80u) /* Error during transfer */

    /* Master API returns */
    #define `$INSTANCE_NAME`_MSTR_NO_ERROR          (0x00u) /* Function complete without error */
    #define `$INSTANCE_NAME`_MSTR_BUS_BUSY          (0x01u) /* Bus is busy, process not started */
    #define `$INSTANCE_NAME`_MSTR_NOT_READY         (0x02u) /* Master not Master on the bus or */
                                                            /*  Slave operation in progress */
    #define `$INSTANCE_NAME`_MSTR_ERR_LB_NAK        (0x03u) /* Last Byte Naked */
    #define `$INSTANCE_NAME`_MSTR_ERR_ARB_LOST      (0x04u) /* Master lost arbitration during communication */
    #define `$INSTANCE_NAME`_MSTR_ERR_ABORT_START_GEN  (0x05u) /* Master did not generate Start, the Slave */
                                                               /* was addressed before */

#endif /* (`$INSTANCE_NAME`_MODE_MASTER_ENABLED) */

#if (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED)
    /* Slave Status Constants */
    #define `$INSTANCE_NAME`_SSTAT_RD_CMPLT     (0x01u) /* Read transfer complete */
    #define `$INSTANCE_NAME`_SSTAT_RD_BUSY      (0x02u) /* Read transfer in progress */
    #define `$INSTANCE_NAME`_SSTAT_RD_ERR_OVFL  (0x04u) /* Read overflow Error */
    #define `$INSTANCE_NAME`_SSTAT_RD_MASK      (0x0Fu) /* Read Status Mask */
    #define `$INSTANCE_NAME`_SSTAT_RD_NO_ERR    (0x00u) /* Read no Error */

    #define `$INSTANCE_NAME`_SSTAT_WR_CMPLT     (0x10u) /* Write transfer complete */
    #define `$INSTANCE_NAME`_SSTAT_WR_BUSY      (0x20u) /* Write transfer in progress */
    #define `$INSTANCE_NAME`_SSTAT_WR_ERR_OVFL  (0x40u) /* Write overflow Error */
    #define `$INSTANCE_NAME`_SSTAT_WR_MASK      (0xF0u) /* Write Status Mask  */
    #define `$INSTANCE_NAME`_SSTAT_WR_NO_ERR    (0x00u) /* Write no Error */

    #define `$INSTANCE_NAME`_SSTAT_RD_CLEAR     (0x0Du) /* Read Status clear */
    #define `$INSTANCE_NAME`_SSTAT_WR_CLEAR     (0xD0u) /* Write Status Clear */

#endif /* (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED) */


/***************************************
*       I2C state machine constants
***************************************/

/* Default slave address states */
#define  `$INSTANCE_NAME`_SM_IDLE           (0x10u) /* Default state - IDLE */
#define  `$INSTANCE_NAME`_SM_EXIT_IDLE      (0x00u) /* Pass master and slave processing and go to IDLE */

/* Slave mode states */
#define  `$INSTANCE_NAME`_SM_SLAVE          (`$INSTANCE_NAME`_SM_IDLE) /* Any Slave state */
#define  `$INSTANCE_NAME`_SM_SL_WR_DATA     (0x11u) /* Master writes data to slave  */
#define  `$INSTANCE_NAME`_SM_SL_RD_DATA     (0x12u) /* Master reads data from slave */

/* Master mode states */
#define  `$INSTANCE_NAME`_SM_MASTER         (0x40u) /* Any master state */

#define  `$INSTANCE_NAME`_SM_MSTR_RD        (0x08u) /* Any master read state          */
#define  `$INSTANCE_NAME`_SM_MSTR_RD_ADDR   (0x49u) /* Master sends address with read */
#define  `$INSTANCE_NAME`_SM_MSTR_RD_DATA   (0x4Au) /* Master reads data              */

#define  `$INSTANCE_NAME`_SM_MSTR_WR        (0x04u) /* Any master read state           */
#define  `$INSTANCE_NAME`_SM_MSTR_WR_ADDR   (0x45u) /* Master sends address with write */
#define  `$INSTANCE_NAME`_SM_MSTR_WR_DATA   (0x46u) /* Master writes data              */

#define  `$INSTANCE_NAME`_SM_MSTR_HALT      (0x60u) /* Master waits for ReStart */

#define `$INSTANCE_NAME`_CHECK_SM_MASTER    (0u != (`$INSTANCE_NAME`_SM_MASTER & `$INSTANCE_NAME`_state))
#define `$INSTANCE_NAME`_CHECK_SM_SLAVE     (0u != (`$INSTANCE_NAME`_SM_SLAVE  & `$INSTANCE_NAME`_state))


/***************************************
*              Registers
***************************************/

#if (`$INSTANCE_NAME`_FF_IMPLEMENTED)
    /* Fixed Function registers */
    #define `$INSTANCE_NAME`_XCFG_REG           (*(reg8 *) `$INSTANCE_NAME`_I2C_FF__XCFG)
    #define `$INSTANCE_NAME`_XCFG_PTR           ( (reg8 *) `$INSTANCE_NAME`_I2C_FF__XCFG)

    #define `$INSTANCE_NAME`_ADDR_REG           (*(reg8 *) `$INSTANCE_NAME`_I2C_FF__ADR)
    #define `$INSTANCE_NAME`_ADDR_PTR           ( (reg8 *) `$INSTANCE_NAME`_I2C_FF__ADR)

    #define `$INSTANCE_NAME`_CFG_REG            (*(reg8 *) `$INSTANCE_NAME`_I2C_FF__CFG)
    #define `$INSTANCE_NAME`_CFG_PTR            ( (reg8 *) `$INSTANCE_NAME`_I2C_FF__CFG)

    #define `$INSTANCE_NAME`_CSR_REG            (*(reg8 *) `$INSTANCE_NAME`_I2C_FF__CSR)
    #define `$INSTANCE_NAME`_CSR_PTR            ( (reg8 *) `$INSTANCE_NAME`_I2C_FF__CSR)

    #define `$INSTANCE_NAME`_DATA_REG           (*(reg8 *) `$INSTANCE_NAME`_I2C_FF__D)
    #define `$INSTANCE_NAME`_DATA_PTR           ( (reg8 *) `$INSTANCE_NAME`_I2C_FF__D)

    #define `$INSTANCE_NAME`_MCSR_REG           (*(reg8 *) `$INSTANCE_NAME`_I2C_FF__MCSR)
    #define `$INSTANCE_NAME`_MCSR_PTR           ( (reg8 *) `$INSTANCE_NAME`_I2C_FF__MCSR)

    #define `$INSTANCE_NAME`_ACT_PWRMGR_REG     (*(reg8 *) `$INSTANCE_NAME`_I2C_FF__PM_ACT_CFG)
    #define `$INSTANCE_NAME`_ACT_PWRMGR_PTR     ( (reg8 *) `$INSTANCE_NAME`_I2C_FF__PM_ACT_CFG)
    #define `$INSTANCE_NAME`_ACT_PWR_EN         ( (uint8_t)  `$INSTANCE_NAME`_I2C_FF__PM_ACT_MSK)

    #define `$INSTANCE_NAME`_STBY_PWRMGR_REG    (*(reg8 *) `$INSTANCE_NAME`_I2C_FF__PM_STBY_CFG)
    #define `$INSTANCE_NAME`_STBY_PWRMGR_PTR    ( (reg8 *) `$INSTANCE_NAME`_I2C_FF__PM_STBY_CFG)
    #define `$INSTANCE_NAME`_STBY_PWR_EN        ( (uint8_t)  `$INSTANCE_NAME`_I2C_FF__PM_STBY_MSK)

    #define `$INSTANCE_NAME`_PWRSYS_CR1_REG     (*(reg8 *) CYREG_PWRSYS_CR1)
    #define `$INSTANCE_NAME`_PWRSYS_CR1_PTR     ( (reg8 *) CYREG_PWRSYS_CR1)

    #define `$INSTANCE_NAME`_CLKDIV1_REG    (*(reg8 *) `$INSTANCE_NAME`_I2C_FF__CLK_DIV1)
    #define `$INSTANCE_NAME`_CLKDIV1_PTR    ( (reg8 *) `$INSTANCE_NAME`_I2C_FF__CLK_DIV1)

    #define `$INSTANCE_NAME`_CLKDIV2_REG    (*(reg8 *) `$INSTANCE_NAME`_I2C_FF__CLK_DIV2)
    #define `$INSTANCE_NAME`_CLKDIV2_PTR    ( (reg8 *) `$INSTANCE_NAME`_I2C_FF__CLK_DIV2)

#else
    /* UDB implementation registers */
    #define `$INSTANCE_NAME`_CFG_REG \
            (*(reg8 *) `$INSTANCE_NAME`_bI2C_UDB_`$CtlModeReplacementString`_CtrlReg__CONTROL_REG)
    #define `$INSTANCE_NAME`_CFG_PTR \
            ( (reg8 *) `$INSTANCE_NAME`_bI2C_UDB_`$CtlModeReplacementString`_CtrlReg__CONTROL_REG)

    #define `$INSTANCE_NAME`_CSR_REG        (*(reg8 *) `$INSTANCE_NAME`_bI2C_UDB_StsReg__STATUS_REG)
    #define `$INSTANCE_NAME`_CSR_PTR        ( (reg8 *) `$INSTANCE_NAME`_bI2C_UDB_StsReg__STATUS_REG)

    #define `$INSTANCE_NAME`_INT_MASK_REG   (*(reg8 *) `$INSTANCE_NAME`_bI2C_UDB_StsReg__MASK_REG)
    #define `$INSTANCE_NAME`_INT_MASK_PTR   ( (reg8 *) `$INSTANCE_NAME`_bI2C_UDB_StsReg__MASK_REG)

    #define `$INSTANCE_NAME`_INT_ENABLE_REG (*(reg8 *) `$INSTANCE_NAME`_bI2C_UDB_StsReg__STATUS_AUX_CTL_REG)
    #define `$INSTANCE_NAME`_INT_ENABLE_PTR ( (reg8 *) `$INSTANCE_NAME`_bI2C_UDB_StsReg__STATUS_AUX_CTL_REG)

    #define `$INSTANCE_NAME`_DATA_REG       (*(reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Shifter_u0__A0_REG)
    #define `$INSTANCE_NAME`_DATA_PTR       ( (reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Shifter_u0__A0_REG)

    #define `$INSTANCE_NAME`_GO_REG         (*(reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Shifter_u0__F1_REG)
    #define `$INSTANCE_NAME`_GO_PTR         ( (reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Shifter_u0__F1_REG)

    #define `$INSTANCE_NAME`_GO_DONE_REG    (*(reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Shifter_u0__A1_REG)
    #define `$INSTANCE_NAME`_GO_DONE_PTR    ( (reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Shifter_u0__A1_REG)

    #define `$INSTANCE_NAME`_MCLK_PRD_REG   (*(reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Master_ClkGen_u0__D0_REG)
    #define `$INSTANCE_NAME`_MCLK_PRD_PTR   ( (reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Master_ClkGen_u0__D0_REG)

    #define `$INSTANCE_NAME`_MCLK_CMP_REG   (*(reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Master_ClkGen_u0__D1_REG)
    #define `$INSTANCE_NAME`_MCLK_CMP_PTR   ( (reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Master_ClkGen_u0__D1_REG)

    #if (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED)
        #define `$INSTANCE_NAME`_ADDR_REG       (*(reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Shifter_u0__D0_REG)
        #define `$INSTANCE_NAME`_ADDR_PTR       ( (reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Shifter_u0__D0_REG)

        #define `$INSTANCE_NAME`_PERIOD_REG     (*(reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Slave_BitCounter__PERIOD_REG)
        #define `$INSTANCE_NAME`_PERIOD_PTR     ( (reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Slave_BitCounter__PERIOD_REG)

        #define `$INSTANCE_NAME`_COUNTER_REG    (*(reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Slave_BitCounter__COUNT_REG)
        #define `$INSTANCE_NAME`_COUNTER_PTR    ( (reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Slave_BitCounter__COUNT_REG)

        #define `$INSTANCE_NAME`_COUNTER_AUX_CTL_REG \
                                    (*(reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Slave_BitCounter__CONTROL_AUX_CTL_REG)
        #define `$INSTANCE_NAME`_COUNTER_AUX_CTL_PTR \
                                    ( (reg8 *) `$INSTANCE_NAME`_bI2C_UDB_Slave_BitCounter__CONTROL_AUX_CTL_REG)

    #endif /* (`$INSTANCE_NAME`_MODE_SLAVE_ENABLED) */

#endif /* (`$INSTANCE_NAME`_FF_IMPLEMENTED) */


/***************************************
*        Registers Constants
***************************************/

/* `$INSTANCE_NAME`_I2C_IRQ */
#define `$INSTANCE_NAME`_ISR_NUMBER              ((uint8_t) `$INSTANCE_NAME`_I2C_IRQ__INTC_NUMBER)
#define `$INSTANCE_NAME`_ISR_CORTEXM0P_PRIORITY  ((uint8_t) `$INSTANCE_NAME`_I2C_IRQ__INTC_CORTEXM0P_PRIORITY)
#define `$INSTANCE_NAME`_ISR_CORTEXM4_PRIORITY   ((uint8_t) `$INSTANCE_NAME`_I2C_IRQ__INTC_CORTEXM4_PRIORITY)

/* I2C Slave Data Register */
#define `$INSTANCE_NAME`_SLAVE_ADDR_MASK    (0x7Fu)
#define `$INSTANCE_NAME`_SLAVE_ADDR_SHIFT   (0x01u)
#define `$INSTANCE_NAME`_DATA_MASK          (0xFFu)
#define `$INSTANCE_NAME`_READ_FLAG          (0x01u)

/* Block reset constants */
#define `$INSTANCE_NAME`_CLEAR_REG          (0x00u)
#define `$INSTANCE_NAME`_BLOCK_RESET_DELAY  (2u)
#define `$INSTANCE_NAME`_FF_RESET_DELAY     (`$INSTANCE_NAME`_BLOCK_RESET_DELAY)
#define `$INSTANCE_NAME`_RESTORE_TIMEOUT    (255u)

#if (`$INSTANCE_NAME`_FF_IMPLEMENTED)
    /* XCFG I2C Extended Configuration Register */
    #define `$INSTANCE_NAME`_XCFG_CLK_EN        (0x80u) /* Enable gated clock to block */
    #define `$INSTANCE_NAME`_XCFG_I2C_ON        (0x40u) /* Enable I2C as wake up source*/
    #define `$INSTANCE_NAME`_XCFG_RDY_TO_SLEEP  (0x20u) /* I2C ready go to sleep */
    #define `$INSTANCE_NAME`_XCFG_FORCE_NACK    (0x10u) /* Force NACK all incoming transactions */
    #define `$INSTANCE_NAME`_XCFG_NO_BC_INT     (0x08u) /* No interrupt on byte complete */
    #define `$INSTANCE_NAME`_XCFG_BUF_MODE      (0x02u) /* Enable buffer mode */
    #define `$INSTANCE_NAME`_XCFG_HDWR_ADDR_EN  (0x01u) /* Enable Hardware address match */

    /* CFG I2C Configuration Register */
    #define `$INSTANCE_NAME`_CFG_SIO_SELECT     (0x80u) /* Pin Select for SCL/SDA lines */
    #define `$INSTANCE_NAME`_CFG_PSELECT        (0x40u) /* Pin Select */
    #define `$INSTANCE_NAME`_CFG_BUS_ERR_IE     (0x20u) /* Bus Error Interrupt Enable */
    #define `$INSTANCE_NAME`_CFG_STOP_IE        (0x10u) /* Enable Interrupt on STOP condition */
    #define `$INSTANCE_NAME`_CFG_CLK_RATE_MSK   (0x0Cu) /* Clock rate select */
    #define `$INSTANCE_NAME`_CFG_CLK_RATE_100   (0x00u) /* Clock rate select 100K */
    #define `$INSTANCE_NAME`_CFG_CLK_RATE_400   (0x04u) /* Clock rate select 400K */
    #define `$INSTANCE_NAME`_CFG_CLK_RATE_050   (0x08u) /* Clock rate select 50K  */
    #define `$INSTANCE_NAME`_CFG_CLK_RATE_RSVD  (0x0Cu) /* Clock rate select Invalid */
    #define `$INSTANCE_NAME`_CFG_EN_MSTR        (0x02u) /* Enable Master operation */
    #define `$INSTANCE_NAME`_CFG_EN_SLAVE       (0x01u) /* Enable Slave operation */

    #define `$INSTANCE_NAME`_CFG_CLK_RATE_LESS_EQUAL_50 (0x04u) /* Clock rate select <= 50kHz */
    #define `$INSTANCE_NAME`_CFG_CLK_RATE_GRATER_50     (0x00u) /* Clock rate select > 50kHz */

    /* CSR I2C Control and Status Register */
    #define `$INSTANCE_NAME`_CSR_BUS_ERROR      (0x80u) /* Active high when bus error has occurred */
    #define `$INSTANCE_NAME`_CSR_LOST_ARB       (0x40u) /* Set to 1 if lost arbitration in host mode */
    #define `$INSTANCE_NAME`_CSR_STOP_STATUS    (0x20u) /* Set if Stop has been detected */
    #define `$INSTANCE_NAME`_CSR_ACK            (0x10u) /* ACK response */
    #define `$INSTANCE_NAME`_CSR_NAK            (0x00u) /* NAK response */
    #define `$INSTANCE_NAME`_CSR_ADDRESS        (0x08u) /* Set in firmware 0 = status bit, 1 Address is slave */
    #define `$INSTANCE_NAME`_CSR_TRANSMIT       (0x04u) /* Set in firmware 1 = transmit, 0 = receive */
    #define `$INSTANCE_NAME`_CSR_LRB            (0x02u) /* Last received bit */
    #define `$INSTANCE_NAME`_CSR_LRB_ACK        (0x00u) /* Last received bit was an ACK */
    #define `$INSTANCE_NAME`_CSR_LRB_NAK        (0x02u) /* Last received bit was an NAK */
    #define `$INSTANCE_NAME`_CSR_BYTE_COMPLETE  (0x01u) /* Informs that last byte has been sent */
    #define `$INSTANCE_NAME`_CSR_STOP_GEN       (0x00u) /* Generate a stop condition */
    #define `$INSTANCE_NAME`_CSR_RDY_TO_RD      (0x00u) /* Set to receive mode */

    /* MCSR I2C Master Control and Status Register */
    #define `$INSTANCE_NAME`_MCSR_STOP_GEN      (0x10u) /* Firmware sets this bit to initiate a Stop condition */
    #define `$INSTANCE_NAME`_MCSR_BUS_BUSY      (0x08u) /* Status bit, Set at Start and cleared at Stop condition */
    #define `$INSTANCE_NAME`_MCSR_MSTR_MODE     (0x04u) /* Status bit, Set at Start and cleared at Stop condition */
    #define `$INSTANCE_NAME`_MCSR_RESTART_GEN   (0x02u) /* Firmware sets this bit to initiate a ReStart condition */
    #define `$INSTANCE_NAME`_MCSR_START_GEN     (0x01u) /* Firmware sets this bit to initiate a Start condition */

    /* PWRSYS_CR1 to handle Sleep */
    #define `$INSTANCE_NAME`_PWRSYS_CR1_I2C_REG_BACKUP  (0x04u) /* Enables, power to I2C regs while sleep */

#else
    /* CONTROL REG bits location */
    #define `$INSTANCE_NAME`_CTRL_START_SHIFT           (7u)
    #define `$INSTANCE_NAME`_CTRL_STOP_SHIFT            (6u)
    #define `$INSTANCE_NAME`_CTRL_RESTART_SHIFT         (5u)
    #define `$INSTANCE_NAME`_CTRL_NACK_SHIFT            (4u)
    #define `$INSTANCE_NAME`_CTRL_ANY_ADDRESS_SHIFT     (3u)
    #define `$INSTANCE_NAME`_CTRL_TRANSMIT_SHIFT        (2u)
    #define `$INSTANCE_NAME`_CTRL_ENABLE_MASTER_SHIFT   (1u)
    #define `$INSTANCE_NAME`_CTRL_ENABLE_SLAVE_SHIFT    (0u)
    #define `$INSTANCE_NAME`_CTRL_START_MASK            ((uint8_t) (0x01u << `$INSTANCE_NAME`_CTRL_START_SHIFT))
    #define `$INSTANCE_NAME`_CTRL_STOP_MASK             ((uint8_t) (0x01u << `$INSTANCE_NAME`_CTRL_STOP_SHIFT))
    #define `$INSTANCE_NAME`_CTRL_RESTART_MASK          ((uint8_t) (0x01u << `$INSTANCE_NAME`_CTRL_RESTART_SHIFT))
    #define `$INSTANCE_NAME`_CTRL_NACK_MASK             ((uint8_t) (0x01u << `$INSTANCE_NAME`_CTRL_NACK_SHIFT))
    #define `$INSTANCE_NAME`_CTRL_ANY_ADDRESS_MASK      ((uint8_t) (0x01u << `$INSTANCE_NAME`_CTRL_ANY_ADDRESS_SHIFT))
    #define `$INSTANCE_NAME`_CTRL_TRANSMIT_MASK         ((uint8_t) (0x01u << `$INSTANCE_NAME`_CTRL_TRANSMIT_SHIFT))
    #define `$INSTANCE_NAME`_CTRL_ENABLE_MASTER_MASK    ((uint8_t) (0x01u << `$INSTANCE_NAME`_CTRL_ENABLE_MASTER_SHIFT))
    #define `$INSTANCE_NAME`_CTRL_ENABLE_SLAVE_MASK     ((uint8_t) (0x01u << `$INSTANCE_NAME`_CTRL_ENABLE_SLAVE_SHIFT))

    /* STATUS REG bits location */
    #define `$INSTANCE_NAME`_STS_LOST_ARB_SHIFT         (6u)
    #define `$INSTANCE_NAME`_STS_STOP_SHIFT             (5u)
    #define `$INSTANCE_NAME`_STS_BUSY_SHIFT             (4u)
    #define `$INSTANCE_NAME`_STS_ADDR_SHIFT             (3u)
    #define `$INSTANCE_NAME`_STS_MASTER_MODE_SHIFT      (2u)
    #define `$INSTANCE_NAME`_STS_LRB_SHIFT              (1u)
    #define `$INSTANCE_NAME`_STS_BYTE_COMPLETE_SHIFT    (0u)
    #define `$INSTANCE_NAME`_STS_LOST_ARB_MASK          ((uint8_t) (0x01u << `$INSTANCE_NAME`_STS_LOST_ARB_SHIFT))
    #define `$INSTANCE_NAME`_STS_STOP_MASK              ((uint8_t) (0x01u << `$INSTANCE_NAME`_STS_STOP_SHIFT))
    #define `$INSTANCE_NAME`_STS_BUSY_MASK              ((uint8_t) (0x01u << `$INSTANCE_NAME`_STS_BUSY_SHIFT))
    #define `$INSTANCE_NAME`_STS_ADDR_MASK              ((uint8_t) (0x01u << `$INSTANCE_NAME`_STS_ADDR_SHIFT))
    #define `$INSTANCE_NAME`_STS_MASTER_MODE_MASK       ((uint8_t) (0x01u << `$INSTANCE_NAME`_STS_MASTER_MODE_SHIFT))
    #define `$INSTANCE_NAME`_STS_LRB_MASK               ((uint8_t) (0x01u << `$INSTANCE_NAME`_STS_LRB_SHIFT))
    #define `$INSTANCE_NAME`_STS_BYTE_COMPLETE_MASK     ((uint8_t) (0x01u << `$INSTANCE_NAME`_STS_BYTE_COMPLETE_SHIFT))

    /* AUX_CTL bits definition */
    #define `$INSTANCE_NAME`_COUNTER_ENABLE_MASK        (0x20u) /* Enable 7-bit counter */
    #define `$INSTANCE_NAME`_INT_ENABLE_MASK            (0x10u) /* Enable interrupt from status register */
    #define `$INSTANCE_NAME`_CNT7_ENABLE                (`$INSTANCE_NAME`_COUNTER_ENABLE_MASK)
    #define `$INSTANCE_NAME`_INTR_ENABLE                (`$INSTANCE_NAME`_INT_ENABLE_MASK)

#endif /* (`$INSTANCE_NAME`_FF_IMPLEMENTED) */


/***************************************
*        Marco
***************************************/

/* ACK and NACK for data and address checks */
#define `$INSTANCE_NAME`_CHECK_ADDR_ACK(csr)    ((`$INSTANCE_NAME`_CSR_LRB_ACK | `$INSTANCE_NAME`_CSR_ADDRESS) == \
                                                 ((`$INSTANCE_NAME`_CSR_LRB    | `$INSTANCE_NAME`_CSR_ADDRESS) &  \
                                                  (csr)))


#define `$INSTANCE_NAME`_CHECK_ADDR_NAK(csr)    ((`$INSTANCE_NAME`_CSR_LRB_NAK | `$INSTANCE_NAME`_CSR_ADDRESS) == \
                                                 ((`$INSTANCE_NAME`_CSR_LRB    | `$INSTANCE_NAME`_CSR_ADDRESS) &  \
                                                  (csr)))

#define `$INSTANCE_NAME`_CHECK_DATA_ACK(csr)    (0u == ((csr) & `$INSTANCE_NAME`_CSR_LRB_NAK))

/* MCSR conditions check */
#define `$INSTANCE_NAME`_CHECK_BUS_FREE(mcsr)       (0u == ((mcsr) & `$INSTANCE_NAME`_MCSR_BUS_BUSY))
#define `$INSTANCE_NAME`_CHECK_MASTER_MODE(mcsr)    (0u != ((mcsr) & `$INSTANCE_NAME`_MCSR_MSTR_MODE))

/* CSR conditions check */
#define `$INSTANCE_NAME`_WAIT_BYTE_COMPLETE(csr)    (0u == ((csr) & `$INSTANCE_NAME`_CSR_BYTE_COMPLETE))
#define `$INSTANCE_NAME`_WAIT_STOP_COMPLETE(csr)    (0u == ((csr) & (`$INSTANCE_NAME`_CSR_BYTE_COMPLETE | \
                                                                     `$INSTANCE_NAME`_CSR_STOP_STATUS)))
#define `$INSTANCE_NAME`_CHECK_BYTE_COMPLETE(csr)   (0u != ((csr) & `$INSTANCE_NAME`_CSR_BYTE_COMPLETE))
#define `$INSTANCE_NAME`_CHECK_STOP_STS(csr)        (0u != ((csr) & `$INSTANCE_NAME`_CSR_STOP_STATUS))
#define `$INSTANCE_NAME`_CHECK_LOST_ARB(csr)        (0u != ((csr) & `$INSTANCE_NAME`_CSR_LOST_ARB))
#define `$INSTANCE_NAME`_CHECK_ADDRESS_STS(csr)     (0u != ((csr) & `$INSTANCE_NAME`_CSR_ADDRESS))

/* Software start and end of transaction check */
#define `$INSTANCE_NAME`_CHECK_RESTART(mstrCtrl)    (0u != ((mstrCtrl) & `$INSTANCE_NAME`_MODE_REPEAT_START))
#define `$INSTANCE_NAME`_CHECK_NO_STOP(mstrCtrl)    (0u != ((mstrCtrl) & `$INSTANCE_NAME`_MODE_NO_STOP))

/* Send read or write completion depends on state */
#define `$INSTANCE_NAME`_GET_MSTAT_CMPLT ((0u != (`$INSTANCE_NAME`_state & `$INSTANCE_NAME`_SM_MSTR_RD)) ? \
                                                 (`$INSTANCE_NAME`_MSTAT_RD_CMPLT) : (`$INSTANCE_NAME`_MSTAT_WR_CMPLT))

/* Returns 7-bit slave address */
#define `$INSTANCE_NAME`_GET_SLAVE_ADDR(dataReg)   (((dataReg) >> `$INSTANCE_NAME`_SLAVE_ADDR_SHIFT) & \
                                                                  `$INSTANCE_NAME`_SLAVE_ADDR_MASK)

#if (`$INSTANCE_NAME`_FF_IMPLEMENTED)
    /* Check enable of module */
    #define `$INSTANCE_NAME`_I2C_ENABLE_REG     (`$INSTANCE_NAME`_ACT_PWRMGR_REG)
    #define `$INSTANCE_NAME`_IS_I2C_ENABLE(reg) (0u != ((reg) & `$INSTANCE_NAME`_ACT_PWR_EN))
    #define `$INSTANCE_NAME`_IS_ENABLED         (0u != (`$INSTANCE_NAME`_ACT_PWRMGR_REG & `$INSTANCE_NAME`_ACT_PWR_EN))

    #define `$INSTANCE_NAME`_CHECK_PWRSYS_I2C_BACKUP    (0u != (`$INSTANCE_NAME`_PWRSYS_CR1_I2C_REG_BACKUP & \
                                                                `$INSTANCE_NAME`_PWRSYS_CR1_REG))

    /* Check start condition generation */
    #define `$INSTANCE_NAME`_CHECK_START_GEN(mcsr)  ((0u != ((mcsr) & `$INSTANCE_NAME`_MCSR_START_GEN)) && \
                                                     (0u == ((mcsr) & `$INSTANCE_NAME`_MCSR_MSTR_MODE)))

    #define `$INSTANCE_NAME`_CLEAR_START_GEN        do{ \
                                                        `$INSTANCE_NAME`_MCSR_REG &=                                   \
                                                                           ((uint8_t) ~`$INSTANCE_NAME`_MCSR_START_GEN); \
                                                    }while(0)

    /* Stop interrupt */
    #define `$INSTANCE_NAME`_ENABLE_INT_ON_STOP     do{ \
                                                        `$INSTANCE_NAME`_CFG_REG |= `$INSTANCE_NAME`_CFG_STOP_IE; \
                                                    }while(0)

    #define `$INSTANCE_NAME`_DISABLE_INT_ON_STOP    do{ \
                                                        `$INSTANCE_NAME`_CFG_REG &=                                 \
                                                                           ((uint8_t) ~`$INSTANCE_NAME`_CFG_STOP_IE); \
                                                    }while(0)

    /* Transmit data */
    #define `$INSTANCE_NAME`_TRANSMIT_DATA          do{ \
                                                        `$INSTANCE_NAME`_CSR_REG = `$INSTANCE_NAME`_CSR_TRANSMIT; \
                                                    }while(0)

    #define `$INSTANCE_NAME`_ACK_AND_TRANSMIT       do{ \
                                                        `$INSTANCE_NAME`_CSR_REG = (`$INSTANCE_NAME`_CSR_ACK |      \
                                                                                    `$INSTANCE_NAME`_CSR_TRANSMIT); \
                                                    }while(0)

    #define `$INSTANCE_NAME`_NAK_AND_TRANSMIT       do{ \
                                                        `$INSTANCE_NAME`_CSR_REG = `$INSTANCE_NAME`_CSR_NAK; \
                                                    }while(0)

    /* Special case: udb needs to ack, ff needs to nak */
    #define `$INSTANCE_NAME`_ACKNAK_AND_TRANSMIT    do{ \
                                                        `$INSTANCE_NAME`_CSR_REG  = (`$INSTANCE_NAME`_CSR_NAK |      \
                                                                                     `$INSTANCE_NAME`_CSR_TRANSMIT); \
                                                    }while(0)
    /* Receive data */
    #define `$INSTANCE_NAME`_ACK_AND_RECEIVE        do{ \
                                                        `$INSTANCE_NAME`_CSR_REG = `$INSTANCE_NAME`_CSR_ACK; \
                                                    }while(0)

    #define `$INSTANCE_NAME`_NAK_AND_RECEIVE        do{ \
                                                        `$INSTANCE_NAME`_CSR_REG = `$INSTANCE_NAME`_CSR_NAK; \
                                                    }while(0)

    #define `$INSTANCE_NAME`_READY_TO_READ          do{ \
                                                        `$INSTANCE_NAME`_CSR_REG = `$INSTANCE_NAME`_CSR_RDY_TO_RD; \
                                                    }while(0)

    /* Release bus after lost arbitration */
    #define `$INSTANCE_NAME`_BUS_RELEASE    `$INSTANCE_NAME`_READY_TO_READ

    /* Master Start/ReStart/Stop conditions generation */
    #define `$INSTANCE_NAME`_GENERATE_START         do{ \
                                                        `$INSTANCE_NAME`_MCSR_REG = `$INSTANCE_NAME`_MCSR_START_GEN; \
                                                    }while(0)

    #define `$INSTANCE_NAME`_GENERATE_RESTART \
                    do{                       \
                        `$INSTANCE_NAME`_MCSR_REG = (`$INSTANCE_NAME`_MCSR_RESTART_GEN | \
                                                     `$INSTANCE_NAME`_MCSR_STOP_GEN);    \
                        `$INSTANCE_NAME`_CSR_REG  = `$INSTANCE_NAME`_CSR_TRANSMIT;       \
                    }while(0)

    #define `$INSTANCE_NAME`_GENERATE_STOP \
                    do{                    \
                        `$INSTANCE_NAME`_MCSR_REG = `$INSTANCE_NAME`_MCSR_STOP_GEN; \
                        `$INSTANCE_NAME`_CSR_REG  = `$INSTANCE_NAME`_CSR_TRANSMIT;  \
                    }while(0)

    /* Master manual APIs compatible defines */
    #define `$INSTANCE_NAME`_GENERATE_START_MANUAL      `$INSTANCE_NAME`_GENERATE_START
    #define `$INSTANCE_NAME`_GENERATE_RESTART_MANUAL    `$INSTANCE_NAME`_GENERATE_RESTART
    #define `$INSTANCE_NAME`_GENERATE_STOP_MANUAL       `$INSTANCE_NAME`_GENERATE_STOP
    #define `$INSTANCE_NAME`_TRANSMIT_DATA_MANUAL       `$INSTANCE_NAME`_TRANSMIT_DATA
    #define `$INSTANCE_NAME`_READY_TO_READ_MANUAL       `$INSTANCE_NAME`_READY_TO_READ
    #define `$INSTANCE_NAME`_ACK_AND_RECEIVE_MANUAL     `$INSTANCE_NAME`_ACK_AND_RECEIVE
    #define `$INSTANCE_NAME`_BUS_RELEASE_MANUAL         `$INSTANCE_NAME`_BUS_RELEASE

#else

    /* Masks to enable interrupts from Status register */
    #define `$INSTANCE_NAME`_STOP_IE_MASK           (`$INSTANCE_NAME`_STS_STOP_MASK)
    #define `$INSTANCE_NAME`_BYTE_COMPLETE_IE_MASK  (`$INSTANCE_NAME`_STS_BYTE_COMPLETE_MASK)

    /* FF compatibility: CSR register bit-fields */
    #define `$INSTANCE_NAME`_CSR_LOST_ARB       (`$INSTANCE_NAME`_STS_LOST_ARB_MASK)
    #define `$INSTANCE_NAME`_CSR_STOP_STATUS    (`$INSTANCE_NAME`_STS_STOP_MASK)
    #define `$INSTANCE_NAME`_CSR_BUS_ERROR      (0x00u)
    #define `$INSTANCE_NAME`_CSR_ADDRESS        (`$INSTANCE_NAME`_STS_ADDR_MASK)
    #define `$INSTANCE_NAME`_CSR_TRANSMIT       (`$INSTANCE_NAME`_CTRL_TRANSMIT_MASK)
    #define `$INSTANCE_NAME`_CSR_LRB            (`$INSTANCE_NAME`_STS_LRB_MASK)
    #define `$INSTANCE_NAME`_CSR_LRB_NAK        (`$INSTANCE_NAME`_STS_LRB_MASK)
    #define `$INSTANCE_NAME`_CSR_LRB_ACK        (0x00u)
    #define `$INSTANCE_NAME`_CSR_BYTE_COMPLETE  (`$INSTANCE_NAME`_STS_BYTE_COMPLETE_MASK)

    /* FF compatibility: MCSR registers bit-fields */
    #define `$INSTANCE_NAME`_MCSR_REG           (`$INSTANCE_NAME`_CSR_REG)  /* UDB incorporates master and slave regs */
    #define `$INSTANCE_NAME`_MCSR_BUS_BUSY      (`$INSTANCE_NAME`_STS_BUSY_MASK)      /* Is bus is busy               */
    #define `$INSTANCE_NAME`_MCSR_START_GEN     (`$INSTANCE_NAME`_CTRL_START_MASK)    /* Generate Start condition     */
    #define `$INSTANCE_NAME`_MCSR_RESTART_GEN   (`$INSTANCE_NAME`_CTRL_RESTART_MASK)  /* Generates RESTART condition  */
    #define `$INSTANCE_NAME`_MCSR_MSTR_MODE     (`$INSTANCE_NAME`_STS_MASTER_MODE_MASK)/* Define if active Master     */

    /* Data to write into TX FIFO to release FSM */
    #define `$INSTANCE_NAME`_PREPARE_TO_RELEASE (0xFFu)
    #define `$INSTANCE_NAME`_RELEASE_FSM        (0x00u)

    /* Define release command done: history of byte complete status is cleared */
    #define `$INSTANCE_NAME`_WAIT_RELEASE_CMD_DONE  (`$INSTANCE_NAME`_RELEASE_FSM != `$INSTANCE_NAME`_GO_DONE_REG)

    /* Check enable of module */
    #define `$INSTANCE_NAME`_I2C_ENABLE_REG     (`$INSTANCE_NAME`_CFG_REG)
    #define `$INSTANCE_NAME`_IS_I2C_ENABLE(reg) ((0u != ((reg) & `$INSTANCE_NAME`_ENABLE_MASTER)) || \
                                                 (0u != ((reg) & `$INSTANCE_NAME`_ENABLE_SLAVE)))

    #define `$INSTANCE_NAME`_IS_ENABLED         (0u != (`$INSTANCE_NAME`_CFG_REG & `$INSTANCE_NAME`_ENABLE_MS))

    /* Check start condition generation */
    #define `$INSTANCE_NAME`_CHECK_START_GEN(mcsr)  ((0u != (`$INSTANCE_NAME`_CFG_REG &        \
                                                             `$INSTANCE_NAME`_MCSR_START_GEN)) \
                                                    &&                                         \
                                                    (0u == ((mcsr) & `$INSTANCE_NAME`_MCSR_MSTR_MODE)))

    #define `$INSTANCE_NAME`_CLEAR_START_GEN        do{ \
                                                        `$INSTANCE_NAME`_CFG_REG &=                 \
                                                        ((uint8_t) ~`$INSTANCE_NAME`_MCSR_START_GEN); \
                                                    }while(0)

    /* Stop interrupt */
    #define `$INSTANCE_NAME`_ENABLE_INT_ON_STOP     do{ \
                                                       `$INSTANCE_NAME`_INT_MASK_REG |= `$INSTANCE_NAME`_STOP_IE_MASK; \
                                                    }while(0)

    #define `$INSTANCE_NAME`_DISABLE_INT_ON_STOP    do{ \
                                                        `$INSTANCE_NAME`_INT_MASK_REG &=                               \
                                                                             ((uint8_t) ~`$INSTANCE_NAME`_STOP_IE_MASK); \
                                                    }while(0)

    /* Transmit data */
    #define `$INSTANCE_NAME`_TRANSMIT_DATA \
                                    do{    \
                                        `$INSTANCE_NAME`_CFG_REG     = (`$INSTANCE_NAME`_CTRL_TRANSMIT_MASK | \
                                                                       `$INSTANCE_NAME`_CTRL_DEFAULT);        \
                                        `$INSTANCE_NAME`_GO_DONE_REG = `$INSTANCE_NAME`_PREPARE_TO_RELEASE;   \
                                        `$INSTANCE_NAME`_GO_REG      = `$INSTANCE_NAME`_RELEASE_FSM;          \
                                    }while(0)

    #define `$INSTANCE_NAME`_ACK_AND_TRANSMIT   `$INSTANCE_NAME`_TRANSMIT_DATA

    #define `$INSTANCE_NAME`_NAK_AND_TRANSMIT \
                                        do{   \
                                            `$INSTANCE_NAME`_CFG_REG     = (`$INSTANCE_NAME`_CTRL_NACK_MASK     | \
                                                                            `$INSTANCE_NAME`_CTRL_TRANSMIT_MASK | \
                                                                            `$INSTANCE_NAME`_CTRL_DEFAULT);       \
                                            `$INSTANCE_NAME`_GO_DONE_REG = `$INSTANCE_NAME`_PREPARE_TO_RELEASE;   \
                                            `$INSTANCE_NAME`_GO_REG      = `$INSTANCE_NAME`_RELEASE_FSM;          \
                                        }while(0)

    /* Receive data */
    #define `$INSTANCE_NAME`_READY_TO_READ  \
                                        do{ \
                                            `$INSTANCE_NAME`_CFG_REG     = `$INSTANCE_NAME`_CTRL_DEFAULT;       \
                                            `$INSTANCE_NAME`_GO_DONE_REG = `$INSTANCE_NAME`_PREPARE_TO_RELEASE; \
                                            `$INSTANCE_NAME`_GO_REG      = `$INSTANCE_NAME`_RELEASE_FSM;       \
                                        }while(0)

    #define `$INSTANCE_NAME`_ACK_AND_RECEIVE    `$INSTANCE_NAME`_READY_TO_READ

    /* Release bus after arbitration is lost */
    #define `$INSTANCE_NAME`_BUS_RELEASE    `$INSTANCE_NAME`_READY_TO_READ

    #define `$INSTANCE_NAME`_NAK_AND_RECEIVE \
                                        do{  \
                                            `$INSTANCE_NAME`_CFG_REG     = (`$INSTANCE_NAME`_CTRL_NACK_MASK |   \
                                                                            `$INSTANCE_NAME`_CTRL_DEFAULT);     \
                                            `$INSTANCE_NAME`_GO_DONE_REG = `$INSTANCE_NAME`_PREPARE_TO_RELEASE; \
                                            `$INSTANCE_NAME`_GO_REG      = `$INSTANCE_NAME`_RELEASE_FSM;       \
                                        }while(0)

    /* Master condition generation */
    #define `$INSTANCE_NAME`_GENERATE_START \
                                        do{ \
                                            `$INSTANCE_NAME`_CFG_REG     = (`$INSTANCE_NAME`_CTRL_START_MASK |  \
                                                                            `$INSTANCE_NAME`_CTRL_DEFAULT);     \
                                            `$INSTANCE_NAME`_GO_DONE_REG = `$INSTANCE_NAME`_PREPARE_TO_RELEASE; \
                                            `$INSTANCE_NAME`_GO_REG      = `$INSTANCE_NAME`_RELEASE_FSM;       \
                                        }while(0)

    #define `$INSTANCE_NAME`_GENERATE_RESTART \
                                        do{   \
                                            `$INSTANCE_NAME`_CFG_REG     = (`$INSTANCE_NAME`_CTRL_RESTART_MASK | \
                                                                            `$INSTANCE_NAME`_CTRL_NACK_MASK    | \
                                                                            `$INSTANCE_NAME`_CTRL_DEFAULT);      \
                                            `$INSTANCE_NAME`_GO_DONE_REG = `$INSTANCE_NAME`_PREPARE_TO_RELEASE;  \
                                            `$INSTANCE_NAME`_GO_REG  =     `$INSTANCE_NAME`_RELEASE_FSM;         \
                                        }while(0)

    #define `$INSTANCE_NAME`_GENERATE_STOP  \
                                        do{ \
                                            `$INSTANCE_NAME`_CFG_REG    = (`$INSTANCE_NAME`_CTRL_NACK_MASK |    \
                                                                           `$INSTANCE_NAME`_CTRL_STOP_MASK |    \
                                                                           `$INSTANCE_NAME`_CTRL_DEFAULT);      \
                                            `$INSTANCE_NAME`_GO_DONE_REG = `$INSTANCE_NAME`_PREPARE_TO_RELEASE; \
                                            `$INSTANCE_NAME`_GO_REG      = `$INSTANCE_NAME`_RELEASE_FSM;        \
                                        }while(0)

    /* Master manual APIs compatible macros */
    /* These macros wait until byte complete history is cleared after command issued */
    #define `$INSTANCE_NAME`_GENERATE_START_MANUAL \
                                        do{ \
                                            `$INSTANCE_NAME`_GENERATE_START;                  \
                                            /* Wait until byte complete history is cleared */ \
                                            while(`$INSTANCE_NAME`_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)
                                        
    #define `$INSTANCE_NAME`_GENERATE_RESTART_MANUAL \
                                        do{          \
                                            `$INSTANCE_NAME`_GENERATE_RESTART;                \
                                            /* Wait until byte complete history is cleared */ \
                                            while(`$INSTANCE_NAME`_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define `$INSTANCE_NAME`_GENERATE_STOP_MANUAL \
                                        do{       \
                                            `$INSTANCE_NAME`_GENERATE_STOP;                   \
                                            /* Wait until byte complete history is cleared */ \
                                            while(`$INSTANCE_NAME`_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define `$INSTANCE_NAME`_TRANSMIT_DATA_MANUAL \
                                        do{       \
                                            `$INSTANCE_NAME`_TRANSMIT_DATA;                   \
                                            /* Wait until byte complete history is cleared */ \
                                            while(`$INSTANCE_NAME`_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define `$INSTANCE_NAME`_READY_TO_READ_MANUAL \
                                        do{       \
                                            `$INSTANCE_NAME`_READY_TO_READ;                   \
                                            /* Wait until byte complete history is cleared */ \
                                            while(`$INSTANCE_NAME`_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define `$INSTANCE_NAME`_ACK_AND_RECEIVE_MANUAL \
                                        do{         \
                                            `$INSTANCE_NAME`_ACK_AND_RECEIVE;                 \
                                            /* Wait until byte complete history is cleared */ \
                                            while(`$INSTANCE_NAME`_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define `$INSTANCE_NAME`_BUS_RELEASE_MANUAL `$INSTANCE_NAME`_READY_TO_READ_MANUAL

#endif /* (`$INSTANCE_NAME`_FF_IMPLEMENTED) */


/***************************************
*     Default register init constants
***************************************/

#define `$INSTANCE_NAME`_DISABLE    (0u)
#define `$INSTANCE_NAME`_ENABLE     (1u)

#if (`$INSTANCE_NAME`_FF_IMPLEMENTED)
    /* `$INSTANCE_NAME`_XCFG_REG: bits definition */
    #define `$INSTANCE_NAME`_DEFAULT_XCFG_HW_ADDR_EN ((`$INSTANCE_NAME`_HW_ADRR_DECODE) ? \
                                                        (`$INSTANCE_NAME`_XCFG_HDWR_ADDR_EN) : (0u))

    #define `$INSTANCE_NAME`_DEFAULT_XCFG_I2C_ON    ((`$INSTANCE_NAME`_WAKEUP_ENABLED) ? \
                                                        (`$INSTANCE_NAME`_XCFG_I2C_ON) : (0u))


    #define `$INSTANCE_NAME`_DEFAULT_CFG_SIO_SELECT ((`$INSTANCE_NAME`_I2C1_SIO_PAIR) ? \
                                                        (`$INSTANCE_NAME`_CFG_SIO_SELECT) : (0u))


    /* `$INSTANCE_NAME`_CFG_REG: bits definition */
    #define `$INSTANCE_NAME`_DEFAULT_CFG_PSELECT    ((`$INSTANCE_NAME`_WAKEUP_ENABLED) ? \
                                                        (`$INSTANCE_NAME`_CFG_PSELECT) : (0u))

    #define `$INSTANCE_NAME`_DEFAULT_CLK_RATE0  ((`$INSTANCE_NAME`_DATA_RATE <= 50u) ?        \
                                                    (`$INSTANCE_NAME`_CFG_CLK_RATE_050) :     \
                                                    ((`$INSTANCE_NAME`_DATA_RATE <= 100u) ?   \
                                                        (`$INSTANCE_NAME`_CFG_CLK_RATE_100) : \
                                                        (`$INSTANCE_NAME`_CFG_CLK_RATE_400)))

    #define `$INSTANCE_NAME`_DEFAULT_CLK_RATE1  ((`$INSTANCE_NAME`_DATA_RATE <= 50u) ?           \
                                                 (`$INSTANCE_NAME`_CFG_CLK_RATE_LESS_EQUAL_50) : \
                                                 (`$INSTANCE_NAME`_CFG_CLK_RATE_GRATER_50))

    #define `$INSTANCE_NAME`_DEFAULT_CLK_RATE   (`$INSTANCE_NAME`_DEFAULT_CLK_RATE1)


    #define `$INSTANCE_NAME`_ENABLE_MASTER      ((`$INSTANCE_NAME`_MODE_MASTER_ENABLED) ? \
                                                 (`$INSTANCE_NAME`_CFG_EN_MSTR) : (0u))

    #define `$INSTANCE_NAME`_ENABLE_SLAVE       ((`$INSTANCE_NAME`_MODE_SLAVE_ENABLED) ? \
                                                 (`$INSTANCE_NAME`_CFG_EN_SLAVE) : (0u))

    #define `$INSTANCE_NAME`_ENABLE_MS      (`$INSTANCE_NAME`_ENABLE_MASTER | `$INSTANCE_NAME`_ENABLE_SLAVE)


    /* `$INSTANCE_NAME`_DEFAULT_XCFG_REG */
    #define `$INSTANCE_NAME`_DEFAULT_XCFG   (`$INSTANCE_NAME`_XCFG_CLK_EN         | \
                                             `$INSTANCE_NAME`_DEFAULT_XCFG_I2C_ON | \
                                             `$INSTANCE_NAME`_DEFAULT_XCFG_HW_ADDR_EN)

    /* `$INSTANCE_NAME`_DEFAULT_CFG_REG */
    #define `$INSTANCE_NAME`_DEFAULT_CFG    (`$INSTANCE_NAME`_DEFAULT_CFG_SIO_SELECT | \
                                             `$INSTANCE_NAME`_DEFAULT_CFG_PSELECT    | \
                                             `$INSTANCE_NAME`_DEFAULT_CLK_RATE       | \
                                             `$INSTANCE_NAME`_ENABLE_MASTER          | \
                                             `$INSTANCE_NAME`_ENABLE_SLAVE)

    /*`$INSTANCE_NAME`_DEFAULT_DIVIDE_FACTOR_REG */
    #define `$INSTANCE_NAME`_DEFAULT_DIVIDE_FACTOR  ((uint16) `$ClkDiv1`u)

#else
    /* `$INSTANCE_NAME`_CFG_REG: bits definition  */
    #define `$INSTANCE_NAME`_ENABLE_MASTER  ((`$INSTANCE_NAME`_MODE_MASTER_ENABLED) ? \
                                             (`$INSTANCE_NAME`_CTRL_ENABLE_MASTER_MASK) : (0u))

    #define `$INSTANCE_NAME`_ENABLE_SLAVE   ((`$INSTANCE_NAME`_MODE_SLAVE_ENABLED) ? \
                                             (`$INSTANCE_NAME`_CTRL_ENABLE_SLAVE_MASK) : (0u))

    #define `$INSTANCE_NAME`_ENABLE_MS      (`$INSTANCE_NAME`_ENABLE_MASTER | `$INSTANCE_NAME`_ENABLE_SLAVE)


    #define `$INSTANCE_NAME`_DEFAULT_CTRL_ANY_ADDR   ((`$INSTANCE_NAME`_HW_ADRR_DECODE) ? \
                                                      (0u) : (`$INSTANCE_NAME`_CTRL_ANY_ADDRESS_MASK))

    /* `$INSTANCE_NAME`_DEFAULT_CFG_REG */
    #define `$INSTANCE_NAME`_DEFAULT_CFG    (`$INSTANCE_NAME`_DEFAULT_CTRL_ANY_ADDR)

    /* All CTRL default bits to be used in macro */
    #define `$INSTANCE_NAME`_CTRL_DEFAULT   (`$INSTANCE_NAME`_DEFAULT_CTRL_ANY_ADDR | `$INSTANCE_NAME`_ENABLE_MS)

    /* Master clock generator: d0 and d1 */
    #define `$INSTANCE_NAME`_MCLK_PERIOD_VALUE  (0x0Fu)
    #define `$INSTANCE_NAME`_MCLK_COMPARE_VALUE (0x08u)

    /* Slave bit-counter: control period */
    #define `$INSTANCE_NAME`_PERIOD_VALUE       (0x07u)

    /* `$INSTANCE_NAME`_DEFAULT_INT_MASK */
    #define `$INSTANCE_NAME`_DEFAULT_INT_MASK   (`$INSTANCE_NAME`_BYTE_COMPLETE_IE_MASK)

    /* `$INSTANCE_NAME`_DEFAULT_MCLK_PRD_REG */
    #define `$INSTANCE_NAME`_DEFAULT_MCLK_PRD   (`$INSTANCE_NAME`_MCLK_PERIOD_VALUE)

    /* `$INSTANCE_NAME`_DEFAULT_MCLK_CMP_REG */
    #define `$INSTANCE_NAME`_DEFAULT_MCLK_CMP   (`$INSTANCE_NAME`_MCLK_COMPARE_VALUE)

    /* `$INSTANCE_NAME`_DEFAULT_PERIOD_REG */
    #define `$INSTANCE_NAME`_DEFAULT_PERIOD     (`$INSTANCE_NAME`_PERIOD_VALUE)

#endif /* (`$INSTANCE_NAME`_FF_IMPLEMENTED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

#define `$INSTANCE_NAME`_SSTAT_RD_ERR       (0x08u)
#define `$INSTANCE_NAME`_SSTAT_WR_ERR       (0x80u)
#define `$INSTANCE_NAME`_MSTR_SLAVE_BUSY    (`$INSTANCE_NAME`_MSTR_NOT_READY)
#define `$INSTANCE_NAME`_MSTAT_ERR_BUF_OVFL (0x80u)
#define `$INSTANCE_NAME`_SSTAT_RD_CMPT      (`$INSTANCE_NAME`_SSTAT_RD_CMPLT)
#define `$INSTANCE_NAME`_SSTAT_WR_CMPT      (`$INSTANCE_NAME`_SSTAT_WR_CMPLT)
#define `$INSTANCE_NAME`_MODE_MULTI_MASTER_ENABLE    (`$INSTANCE_NAME`_MODE_MULTI_MASTER_MASK)
#define `$INSTANCE_NAME`_DATA_RATE_50       (50u)
#define `$INSTANCE_NAME`_DATA_RATE_100      (100u)
#define `$INSTANCE_NAME`_DEV_MASK           (0xF0u)
#define `$INSTANCE_NAME`_SM_SL_STOP         (0x14u)
#define `$INSTANCE_NAME`_SM_MASTER_IDLE     (0x40u)
#define `$INSTANCE_NAME`_HDWR_DECODE        (0x01u)

#endif /* CY_I2C_`$INSTANCE_NAME`_H */


/* [] END OF FILE */
