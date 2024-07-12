#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "cy_crypto.h"
#include "cy_crypto_server.h"
#include "cy_sysint.h"

/*******************************************************************************
* Macro definitions
********************************************************************************/
/* Define the missing macros */
#define CY_CRYPTO_IPC_CHANNEL (uint32_t)(3u)
#define CY_CRYPTO_NOTIFY_RELEASE (uint32_t)(1u)
#define CY_CRYPTO_NOTIFY_ACQUIRE (uint32_t)(2u)

/* Define the missing opcode */
#define OPCODE_ADC_0 0x01
#define OPCODE_ADC_1 0x02
#define OPCODE_ADC_2 0x03
#define OPCODE_ADC_3 0x04
#define OPCODE_ADC_4 0x05
#define OPCODE_ADC_5 0x06
#define OPCODE_ADC_6 0x07
#define OPCODE_ADC_7 0x08
#define OPCODE_I2C 0x09

/* The input message size (inclusive of the string terminating character '\0').
 * Edit this macro to suit your message size */
#define MAX_MESSAGE_SIZE                    (100u)

/* Size of the message block that can be processed by Crypto hardware for
 * AES encryption */
#define AES128_ENCRYPTION_LENGTH            (uint32_t)(16u)

#define AES128_KEY_LENGTH                   (uint32_t)(16u)

/* Crypto configuration structure */
const cy_stc_crypto_config_t cryptoConfig =
{
    /* .ipcChannel             */ CY_CRYPTO_IPC_CHANNEL,
    /* .acquireNotifierChannel */ CY_CRYPTO_NOTIFY_RELEASE,
    /* .releaseNotifierChannel */ CY_CRYPTO_NOTIFY_ACQUIRE,
    /* .releaseNotifierConfig */ {
        /* .intrSrc            */ cpuss_interrupts_ipc_0_IRQn,
        /* .intrPriority       */ 2u,
    },
    /* .userCompleteCallback   */ NULL,
    /* .userGetDataHandler     */ NULL,
    /* .userErrorHandler       */ NULL,
    /* .acquireNotifierConfig */ {
        /* .intrSrc            */ cpuss_interrupts_ipc_1_IRQn,
        /* .intrPriority       */ 2u,
    },
    /* .cryptoErrorIntrConfig */ {
        /* .intrSrc            */ cpuss_interrupt_crypto_IRQn,
        /* .intrPriority       */ 2u,
    }
};

/* Key used for AES encryption */
CY_ALIGN(4) uint8_t AES_Key[AES128_KEY_LENGTH] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0xAA, 0xBB, 0xCC, 0xDD};

/* Variables to store Crypto internal states */
cy_stc_crypto_context_t cryptoScratch;
uint32_t cryptoAES;

/*******************************************************************************
* Function prototypes
*******************************************************************************/
void send_task(void *pvParameters);
void encrypt_data(uint8_t *input, uint8_t *output, const uint8_t *key);
void decrypt_data(uint8_t *input, uint8_t *output, const uint8_t *key);

int main(void)
{
    // Initialize UART for serial communication
    UART_Start();
    
    __enable_irq();  // Enable global interrupts
    
    // Initialize the Crypto driver
    cy_en_crypto_status_t cryptoStatus;
    cryptoStatus = Cy_Crypto_Init(&cryptoConfig, &cryptoScratch);
    if (cryptoStatus != CY_CRYPTO_SUCCESS) {
        // Handle error
    }
    Cy_Crypto_Enable();

    // Create the send task
    xTaskCreate(send_task, "send data", 1024, NULL, 2, NULL);
    
    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    for (;;) {
        // This point should never be reached
    }
}

/* calculate crc8 for specific data */
uint8_t calculate_crc8(uint8_t *data, uint8_t length) {
    uint8_t crc = 0x00;
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc;
}

void wrap_data(uint8_t opcode, uint8_t* data, uint8_t length) {
    uint8_t packet[2 + length + 1];
    packet[0] = opcode;
    packet[1] = length;
    memcpy(&packet[2], data, length);
    packet[2 + length] = calculate_crc8(packet, 2 + length);
    
    // Encrypt the packet
    uint8_t encrypted_packet[16]; // AES block size is 16 bytes
    encrypt_data(packet, encrypted_packet, AES_Key);
    
    Cy_SCB_UART_PutArray(UART_HW, encrypted_packet, 16);
}

/*  send data 
    adc_channel goes from 0 - 8 
    0x0-0x7 are 8 ADC channels, 0x8 is the I2C ADPD readings */
void send_data(uint16_t adc_value, uint16_t adc_channel) {
    uint8_t data[2];
    data[0] = (adc_value >> 8) & 0xFF;
    data[1] = adc_value & 0xFF;
    wrap_data(OPCODE_ADC_0 + adc_channel, data, 2);
}

void send_task(void *pvParameters) {
    while(1) {
        uint16_t data = 0x1234;
        send_data(data, 0x0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void encrypt_data(uint8_t *input, uint8_t *output, const uint8_t *key) {
    Cy_Crypto_Aes_Init(&cryptoAES, CY_CRYPTO_KEY_AES_128, (uint32_t *)key);
    Cy_Crypto_Aes_Ecb_Run(CY_CRYPTO_ENCRYPT, (uint32_t *)output, (uint32_t *)input, &cryptoAES);
    Cy_Crypto_Sync(CY_CRYPTO_SYNC_BLOCKING);
    //Cy_Crypto_Aes_Free(&cryptoAES);
}

void decrypt_data(uint8_t *input, uint8_t *output, const uint8_t *key) {
    Cy_Crypto_Aes_Init(&cryptoAES, CY_CRYPTO_KEY_AES_128, (uint32_t *)key);
    Cy_Crypto_Aes_Ecb_Run(CY_CRYPTO_DECRYPT, (uint32_t *)output, (uint32_t *)input, &cryptoAES);
    Cy_Crypto_Sync(CY_CRYPTO_SYNC_BLOCKING);
    //Cy_Crypto_Aes_Free(&cryptoAES);
}
