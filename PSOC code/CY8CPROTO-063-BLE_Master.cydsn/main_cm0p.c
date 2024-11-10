/* ========================================
 *
 * Copyright Carnegie Mellon University, 2024
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Carnegie Mellon University.
 *
 * ========================================
*/

#include "project.h"
#include "cy_crypto_server.h"

/* Macros to configure the Crypto block */
/* IPC data channel for the Crypto */
#define MY_CHAN_CRYPTO         				(uint32_t)(3u)
/* IPC interrupt structure for the Crypto server */
#define MY_INTR_CRYPTO_SRV     				(uint32_t)(1u)
/* IPC interrupt structure for the Crypto client */
#define MY_INTR_CRYPTO_CLI     				(uint32_t)(2u)
/* CM0+ IPC interrupt mux number the Crypto server */
#define MY_INTR_CRYPTO_SRV_MUX 				(IRQn_Type)(2u)
 /* CM0+ IPC interrupt mux number the Crypto client */
#define MY_INTR_CRYPTO_CLI_MUX 				(IRQn_Type)(3u)
/* CM0+ ERROR interrupt mux number the Crypto server */
#define MY_INTR_CRYPTO_ERR_MUX 				(IRQn_Type)(4u)

/* Variables to store Crypto internal states */
cy_stc_crypto_server_context_t cryptoServerContext;
cy_en_crypto_status_t           cryptoStatus;

/* Crypto configuration structure */
const cy_stc_crypto_config_t cryptoConfig =
{
    /* .ipcChannel             */ MY_CHAN_CRYPTO,
    /* .acquireNotifierChannel */ MY_INTR_CRYPTO_SRV,
    /* .releaseNotifierChannel */ MY_INTR_CRYPTO_CLI,
    /* .releaseNotifierConfig */ {
    #if (CY_CPU_CORTEX_M0P)
        /* .intrSrc            */ MY_INTR_CRYPTO_CLI_MUX,
        /* .cm0pSrc            */ cpuss_interrupts_ipc_2_IRQn, /* depends on selected releaseNotifierChannel value */
    #else
        /* .intrSrc            */ cpuss_interrupts_ipc_2_IRQn, /* depends on selected releaseNotifierChannel value */
    #endif
        /* .intrPriority       */ 2u,
    },
    /* .userCompleteCallback   */ NULL,
    /* .userGetDataHandler     */ NULL,
    /* .userErrorHandler       */ NULL,
    /* .acquireNotifierConfig */ {
    #if (CY_CPU_CORTEX_M0P)
        /* .intrSrc            */ MY_INTR_CRYPTO_SRV_MUX,      /* to use with DeepSleep mode should be in DeepSleep capable muxer's range */
        /* .cm0pSrc            */ cpuss_interrupts_ipc_1_IRQn, /* depends on selected acquireNotifierChannel value */
    #else
        /* .intrSrc            */ cpuss_interrupts_ipc_1_IRQn, /* depends on selected acquireNotifierChannel value */
    #endif
        /* .intrPriority       */ 2u,
    },
    /* .cryptoErrorIntrConfig */ {
    #if (CY_CPU_CORTEX_M0P)
        /* .intrSrc            */ MY_INTR_CRYPTO_ERR_MUX,
        /* .cm0pSrc            */ cpuss_interrupt_crypto_IRQn,
    #else
        /* .intrSrc            */ cpuss_interrupt_crypto_IRQn,
    #endif
        /* .intrPriority       */ 2u,
    }
};

/**
 * @brief main function
*/
int main(void) {
    
    __enable_irq(); /* Enable global interrupts. */
    
    /* Start Crypto server before enabling CM4. */
    while (Cy_Crypto_Server_Start(&cryptoConfig, &cryptoServerContext) != CY_CRYPTO_SUCCESS) {}
    
    /* Enable CM4.  CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. */
    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR); 

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
