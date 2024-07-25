/**
 * @file main_cm4.c 
*/

#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cy_crypto.h"
#include "ADPD1080.h"
#include "bq34z100.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

/* UNUSED - Task configuration */
#define ADPD1080_TASK_STACK_SIZE          1024
#define ADC_TASK_STACK_SIZE               1024
#define BQ34Z100_TASK_STACK_SIZE          1024
#define ADPD1080_TASK_PRIORITY            2
#define ADC_TASK_PRIORITY                 3
#define BQ34Z100_TASK_PRIORITY            3
#define ADPD1080_READ_INTERVAL_MS         10
#define ADC_READ_INTERVAL_MS              100
#define BQ34Z100_READ_INTERVAL_MS         100

/* Constants */
#define OPCODE_ADC_0 0x01
#define OPCODE_ADC_1 0x02
#define OPCODE_ADC_2 0x03
#define OPCODE_ADC_3 0x04
#define OPCODE_ADC_4 0x05
#define OPCODE_ADC_5 0x06
#define OPCODE_ADC_6 0x07
#define OPCODE_ADC_7 0x08
#define OPCODE_ADPD  0x09
#define OPCODE_ALL   0x0A

#define SMOOTHED_SAMPLE_SIZE     200U

#define ADC_NUM_CHANNELS         4U
#define ADPD_NUM_CHANNELS        1U

#define ADC_SAMPLE_RATE_DIV      10U
#define ADPD_SAMPLE_RATE_DIV     1U // TODO: nonfunctional, need to implement (make nicer with math?)

#define ADC_SAMPLES_PER_PACKET   1U
#define ADPD_SAMPLES_PER_PACKET  10U

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

#define MAX_PACKET_SIZE                     (1000u) // Maximum number of bytes in a packet

/* Size of the message block that can be processed by Crypto hardware for
 * AES encryption */
#define AES128_ENCRYPTION_LENGTH 			(uint32_t)(16u)

#define AES128_KEY_LENGTH  					(uint32_t)(16u)

// CRC-8 calculation table
const uint8_t crcTable[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

/* Global variables */
volatile uint16_t timerCount = 0;
volatile bool dataReady = false;
volatile int16_t ADCData[ADC_SAMPLES_PER_PACKET*ADC_NUM_CHANNELS];
volatile uint16_t adpdDataA[ADPD_SAMPLES_PER_PACKET*ADPD_NUM_CHANNELS];
volatile uint16_t adpdDataB[ADPD_SAMPLES_PER_PACKET*ADPD_NUM_CHANNELS];

/* Variables to store Crypto internal states */
cy_stc_crypto_context_t cryptoScratch;
cy_stc_crypto_context_aes_t cryptoAES;
cy_stc_crypto_server_context_t cryptoServerContext;
cy_en_crypto_status_t cryptoStatus;

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

/* Key used for AES encryption*/
CY_ALIGN(4) uint8_t AES_Key[AES128_KEY_LENGTH]={0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0xFF,0xEE,\
                                     0xDD,0xCC,0xBB,0xAA,0xAA,0xBB,0xCC,0xDD,};

/* Variables to hold the user message and the corresponding encrypted message */
CY_ALIGN(4) uint8_t packet[MAX_PACKET_SIZE];
CY_ALIGN(4) uint8_t encrypted_pkt[MAX_PACKET_SIZE];
// CY_ALIGN(4) uint8_t decrypted_pkt[MAX_PACKET_SIZE]; // debug only

/* Function prototypes */
void vADPD1080(void *pvParameters);
void vADC(void *pvParameters);
void vBQ34Z100(void *pvParameters);
uint8_t calculateCRC8(uint8_t opCode, uint8_t dataLength, uint8_t* data);
void wrap_data(uint8_t opcode, uint8_t* data, uint8_t length);
void send_data(uint16_t adc_value, uint16_t adc_channel);
float32_t movingAvg(uint16_t *ptrArrNumbers, uint32_t *ptrSum, uint32_t pos, uint32_t len, uint16_t nextNum);
void float2Bytes(float32_t val, uint8_t *bytes_array);
void PrintData(uint8_t* data, uint8_t len);

/* Interrupt service routines */
/**
 * @brief Program foreground, handles timer overflow interrupt with frequency 100 Hz
 * reads from ADPD1080 with frequency 100 Hz, and ADC with frequency 10 Hz
 * every ADC_SAMPLE_RATE_DIV interrupts, gets previous conversion results and starts next conversion
 * 
 * @param TimerInt_Handler - interrupt handler function name
 *
 * @return none
 */
CY_ISR (Timer_Int_Handler) {
    Cy_GPIO_Write(Int_Debug_PORT, Int_Debug_NUM, 1);
    
    // Clear timer overflow interrupt
    Timer_ClearInterrupt(CY_TCPWM_INT_ON_TC);
    // printf("%u", timerCount); debug only
    
    // Read ADPD1080 data registers with data hold mechanism (6 ms)
    if (turbidity_ReadDataNoInterrupt(ADPD_NUM_CHANNELS)) {
        adpdDataA[timerCount] = au16DataSlotA[0];
        adpdDataB[timerCount] = au16DataSlotB[0];
    }
    else {
        adpdDataA[timerCount] = 0;
        adpdDataB[timerCount] = 0;
    }
    
    // Increment timer count
    timerCount++;
    
    // Read ADC conversion results with frequency 10 Hz (10 us latency)
    if (timerCount == ADC_SAMPLE_RATE_DIV) {
        timerCount = 0;
        // Check conversion status without blocking
        uint32_t conversionStatus = ADC_IsEndConversion(CY_SAR_RETURN_STATUS);
        if (conversionStatus) {
            for (uint16_t i = 0; i < ADC_NUM_CHANNELS; i++) {
                ADCData[i] = ADC_GetResult16(i);
            }
            dataReady = true;
            VDAC_SetValueBuffered(ADCData[0]); // debug only
        }
        else {
            printf("***Conversion not finished yet! Status is %lu***\r\n", conversionStatus);
            return;
        }        
        // Start next conversion
        ADC_StartConvert();
    }
    
    // Flush write to hardware by reading from same register
    Timer_GetInterruptStatus();
    
    Cy_GPIO_Write(Int_Debug_PORT, Int_Debug_NUM, 0);
}

/**
 * @brief main function
 */
int main(void) {
    // Variables for calculating oxygen saturation and hemoglobin concentration
    uint16_t L680 ; // Time Slot A Channel 1 (680 nm laser)
    uint16_t L850 ; // Time Slot B Channel 1 (850 nm laser)

    float32_t R;                 // Ratio of L680 to L850
    float32_t SO2;               // Oxygen saturation
    float32_t R_avg;             // Running average R
    float32_t SO2_avg;           // Running average SO2
    float32_t del680;            // L680 Hemoglobin concentration
    float32_t del850;            // L850 Hemoglobin concentration
    // unsigned long t;
    // unsigned long t0;

    // Maintain running average
    uint16_t slotA_avg[SMOOTHED_SAMPLE_SIZE] = {0};
    uint16_t slotB_avg[SMOOTHED_SAMPLE_SIZE] = {0};

    uint32_t posA = 0;
    uint32_t posB = 0;
    float32_t avg_valA = 0;
    float32_t avg_valB = 0;
    uint32_t sumA = 0;
    uint32_t sumB = 0;
    uint32_t lenA = sizeof(slotA_avg)/sizeof(uint16_t);
    uint32_t lenB = sizeof(slotB_avg)/sizeof(uint16_t);
    
    // Store current size of sensor data UART packet and number of AES blocks
    uint8_t packetsize = 0, AESBlock_count = 0;
    
    // Initialize UART for serial communication
    UART_Start();
    
    // Initialize I2C for digital sensor communication
    I2C_Start();
    
    // Intialize ADC for receiving analog sensor data
    ADC_Start();
    
    // Initialize DAC (debug only)
    VDAC_Start();
    
    // Register Timer interrupt handler
    Cy_SysInt_Init(&Timer_Int_cfg, Timer_Int_Handler);
    NVIC_EnableIRQ(Timer_Int_cfg.intrSrc);
    
    __enable_irq();  // Enable global interrupts

    // Initialize and configure the ADPD1080 sensor
    printf("Initializing ADPD1080 sensor...\r\n");

    if (!ADPD1080_Begin(ADPD1080_ADDRESS, 0)) {
        printf("ADPD1080 initialization failed!\r\n");
        while (1); // Loop forever on failure
    }
    
    // Initialize sensor registers
    turbidity_Init();
    //turbidity_ChannelOffsetCalibration();
    
    printf("ADPD1080 initialization successful.\r\n");
    
    /* Initialization of Crypto Driver */
	Cy_Crypto_Init(&cryptoConfig, &cryptoScratch);

	/* Enable Crypto Hardware */
	Cy_Crypto_Enable();

	/* Wait for Crypto Block to be available */
	Cy_Crypto_Sync(CY_CRYPTO_SYNC_BLOCKING); // TODO: consider non-blocking, error-checking
    
    /* Initializes the AES operation by setting key and key length */
	while (Cy_Crypto_Aes_Init((uint32_t*)AES_Key, CY_CRYPTO_KEY_AES_128, &cryptoAES) != CY_CRYPTO_SUCCESS) {}

	/* Wait for Crypto Block to be available */
	Cy_Crypto_Sync(CY_CRYPTO_SYNC_BLOCKING); // TODO: consider non-blocking, error-checking
    
    // Begin first ADC scan
    ADC_StartConvert();
    
    // Initialize and start Timer to periodically handle ADC conversion results
    Timer_Start();
    
    // Background loop
    for (;;) {
        // When data is ready, process, encrypt, and transmit packet
        if (dataReady) {
            Cy_GPIO_Write(Debug_PORT, Debug_NUM, 1);
            dataReady = false;
            
            // Process adpd1080 data
            for (uint8_t i = 0; i < ADPD_SAMPLES_PER_PACKET; i++) {
                // Raw and time avg light intensity
                L680 = adpdDataA[i];
                L850 = adpdDataB[i];
            
                avg_valA = movingAvg(slotA_avg, &sumA, posA, lenA, L680);
                avg_valB = movingAvg(slotB_avg, &sumB, posB, lenB, L850);
                
                // Update ring buffer current position
                posA++;
                if (posA >= lenA) posA = 0;
                posB++;
                if (posB >= lenB) posB = 0;
                
                // SO2%
                R = log(L680)/log(L850);
                R_avg = log(avg_valA)/log(avg_valB);
                
                SO2 = 188.1*(R) - 89.95;
                SO2_avg = 188.1*(R_avg) - 89.95;
                
                // Hemoglobin concentration
                del680 = -3.412*log(64*avg_valA / PULSE_A) + 43.02; // TODO: is PULSE_A/PULSE_B = 127?
                del850 = -2.701*log(64*avg_valB / PULSE_B) + 35.27; // -0.1049
                
                // Populate packet buffer
                float2Bytes(SO2, &packet[packetsize]);
                packetsize += sizeof(float32_t);
                
                float2Bytes(SO2_avg, &packet[packetsize]);
                packetsize += sizeof(float32_t);
                
                float2Bytes(del680, &packet[packetsize]);
                packetsize += sizeof(float32_t);
                
                float2Bytes(del850, &packet[packetsize]);
                packetsize += sizeof(float32_t);
                
                // printf("L850: %d, L850_Avg: %f, SO2: %f, SO2_Avg: %f, del680: %f, del850: %f\r\n",
                       // L850, avg_valB, SO2, SO2_avg, del680, del850); // debug only
            }
            
            // Process ADC data
            for (uint8_t i = 0; i < ADC_NUM_CHANNELS; i++) {
                float2Bytes(Cy_SAR_CountsTo_Volts(SAR, i, ADCData[i]), &packet[packetsize]);
                packetsize += sizeof(float32_t);
            }
            
            // Encrypt packet
            AESBlock_count =  (packetsize % AES128_ENCRYPTION_LENGTH == 0) ? \
								  (packetsize/AES128_ENCRYPTION_LENGTH) \
								  : (1 + packetsize/AES128_ENCRYPTION_LENGTH);
                                
			for (int i = 0; i < AESBlock_count ; i++) {
				/* Perform AES ECB Encryption mode of operation */
				cy_en_crypto_status_t status;
                status = Cy_Crypto_Aes_Ecb_Run(CY_CRYPTO_ENCRYPT,\
				(uint32_t*) (encrypted_pkt + AES128_ENCRYPTION_LENGTH * i),\
				(uint32_t*) (packet + AES128_ENCRYPTION_LENGTH * i), &cryptoAES);
                if (status != CY_CRYPTO_SUCCESS) {
                    printf("big oops!\r\n");
                }

				/* Wait for Crypto Block to be available */
				Cy_Crypto_Sync(CY_CRYPTO_SYNC_BLOCKING);
			}
            
            // debug only
            printf("\r\n\nPacket being Encrypted:\r\n");
            PrintData(packet, packetsize);
            printf("\r\n\nKey used for Encryption:\r\n");
            PrintData(AES_Key, AES128_KEY_LENGTH);
            printf("\r\nResult of Encryption:\r\n");
            PrintData((uint8_t*)encrypted_pkt, AESBlock_count*AES128_ENCRYPTION_LENGTH);
            
            // Transmit packet
            wrap_data(OPCODE_ALL, encrypted_pkt, AESBlock_count*AES128_ENCRYPTION_LENGTH);        
            
            Cy_GPIO_Write(Debug_PORT, Debug_NUM, 0);
        }
    }
}

/* Helper functions */
uint8_t calculateCRC8(uint8_t opCode, uint8_t dataLength, uint8_t* data) {
  uint8_t crc = 0x00; // Initialize CRC value

  // Update CRC with the opcode
  crc = crcTable[crc ^ opCode];
  
  // Update CRC with the data length
  crc = crcTable[crc ^ dataLength];
  
  // Update CRC with each data byte
  for (uint8_t i = 0; i < dataLength; i++) {
    crc = crcTable[crc ^ data[i]];
  }

  return crc;
}

void wrap_data(uint8_t opcode, uint8_t* data, uint8_t length) {
    uint8_t packet[2 + length + 1];
    packet[0] = opcode;
    packet[1] = length;
    memcpy(&packet[2], data, length);
    packet[2 + length] = calculateCRC8(opcode, length, data);
    Cy_SCB_UART_PutArray(UART_HW, packet, 2 + length + 1);
}

/*  send data (UNUSED)
 *  adc_channel goes from 0 - 8 
 *  0x0-0x7 are 8 ADC channels, 0x8 is the I2C ADPD readings
 */
void send_data(uint16_t adc_value, uint16_t adc_channel) {
    uint8_t data[2];
    data[0] = (adc_value >> 8) & 0xFF;
    data[1] = adc_value & 0xFF;
    wrap_data(OPCODE_ADC_0 + adc_channel, data, 2);
}

/**
 * @brief Helper function for ADPD1080 task calculating moving average
 * 
 * @author <bmccormack> - https://gist.github.com/bmccormack/d12f4bf0c96423d03f82
 * 
 * @param int *ptrArrNumbers - pointer to current data points
 * @param long &ptrSum - pointer to current sum of data points
 * @param int pos - current position within array of data points
 * @param int len - total number of data points
 * @param int nextNum - next data point to add
 *
 * @return int - the moving average after nextNum added
 */
float32_t movingAvg(uint16_t *ptrArrNumbers, uint32_t *ptrSum, uint32_t pos, uint32_t len, uint16_t nextNum) {
    // Subtract the oldest number from the prev sum, add the new number
    *ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
    
    // Assign the nextNum to the position in the array
    ptrArrNumbers[pos] = nextNum;
    
    // return the average
    return (float32_t)*ptrSum / len;
}

/**
 * @brief Helper function for main background loop converting float to uint8_t array
 * 
 * @author <Floris> - https://stackoverflow.com/questions/24420246/c-function-to-convert-float-to-byte-array
 * 
 * @param float32_t val - single precision float value to be converted to uint8_t array of length 4
 * @param uint8_t *bytesArray - pointer to uint8_t array in memory
 *
 * @return none
 */
void float2Bytes(float32_t val, uint8_t *bytes_array) {
  // Create union of shared memory space
  union {
    float32_t float_variable;
    uint8_t temp_array[4];
  } u;
  // Overite bytes of union with float variable
  u.float_variable = val;
  // Assign bytes to input array
  memcpy(bytes_array, u.temp_array, 4);
}

/**
 * @brief: Debug-only function used to display the data in hexadecimal format
 *
 * @param: uint8_t* - Pointer to location of data to be printed
 *
 * @return: void
*/
void PrintData(uint8_t* data, uint8_t len) {
	printf("\r\n");
	char print[10];
	for(uint32 i=0; i < len; i++) {
		sprintf(print,"0x%02X ", *(data+i));
		printf("%s", print);
	}
	printf("\r\n");
}

/* FreeRTOS Task Functions */
/* UNUSED */
void vBQ34Z100(void *pvParameters) {
    (void) pvParameters;

    // Declare and initialize BQ34Z100 device structure
    BQ34Z100_t bq34z100Device;

    for (;;) {
        // Ensure the device is ready and communicate with it
        BQ34Z100_Unseal(&bq34z100Device);  // Unseal the device to enable full access

        // Read and print the battery voltage
        uint16_t voltage = BQ34Z100_GetVoltage(&bq34z100Device);
        printf("Battery Voltage: %u mV\n", voltage);

        // Read and print the State of Charge
        uint8_t soc = BQ34Z100_GetSOC(&bq34z100Device);
        printf("State of Charge: %u%%\n", soc);

        // Read and print the battery current
        int16_t current = BQ34Z100_GetCurrent(&bq34z100Device);
        printf("Battery Current: %d mA\n", current);

        // Read and print device type and firmware version as an example of control commands
        uint16_t deviceType = BQ34Z100_ReadDeviceType(&bq34z100Device);
        uint16_t fwVersion = BQ34Z100_ReadFWVersion(&bq34z100Device);
        printf("Device Type: 0x%04X, Firmware Version: 0x%04X\n", deviceType, fwVersion);

        // Delay between reads to prevent overwhelming the device and the I2C bus
        vTaskDelay(pdMS_TO_TICKS(BQ34Z100_READ_INTERVAL_MS));
    }
}

/* [] END OF FILE */
