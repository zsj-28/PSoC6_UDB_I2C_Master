/**
 * @file main_cm4.c 
 */

/* Include files */
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ADPD1080.h"
#include "bq34z100.h"
#include <stdio.h>
#include <math.h>

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

#define OPCODE_ADC_0 0x01
#define OPCODE_ADC_1 0x02
#define OPCODE_ADC_2 0x03
#define OPCODE_ADC_3 0x04
#define OPCODE_ADC_4 0x05
#define OPCODE_ADC_5 0x06
#define OPCODE_ADC_6 0x07
#define OPCODE_ADC_7 0x08
#define OPCODE_I2C   0x09

/* Function prototypes */
void vADPD1080(void *pvParameters);
void vADC(void *pvParameters);
void vBQ34Z100(void *pvParameters);
uint8_t calculateCRC8(uint8_t opCode, uint8_t dataLength, uint8_t* data);
void wrap_data(uint8_t opcode, uint8_t* data, uint8_t length);
void send_data(uint16_t adc_value, uint16_t adc_channel);
float movingAvg(uint16_t *ptrArrNumbers, uint32_t *ptrSum, size_t pos, size_t len, uint16_t nextNum);

/* Constants */
#define SMOOTHED_SAMPLE_SIZE   200U
#define ADC_NUM_CHANNELS         4U
#define ADPD_TO_ADC_RATIO       10U

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
volatile bool ADCDataReady = false;
volatile int16_t ADCResults[ADC_NUM_CHANNELS];

/* Interrupt service routines */
/**
 * @brief handles interrupt on timer overflow with frequency 100Hz
 * every ADPD_TO_ADC_RATIO interrupts, gets previous conversion results and starts next conversion
 * 
 * @param TimerInt_Handler - interrupt handler function name
 *
 * @return none
 */
CY_ISR (Timer_Int_Handler) {
    //Cy_GPIO_Write(Int_Debug_PORT, Int_Debug_NUM, 1);
    
    // Clear timer overflow interrupt
    Timer_ClearInterrupt(CY_TCPWM_INT_ON_TC);
    timerCount++;
    // printf("%u", timerCount); debug only
    
    // Read ADC conversion results with frequency 10 Hz
    if (timerCount == ADPD_TO_ADC_RATIO) {
        timerCount = 0;
        // Check conversion status without blocking
        uint32_t conversionStatus = ADC_IsEndConversion(CY_SAR_RETURN_STATUS);
        if (conversionStatus) {
            for (uint16_t i = 0; i < ADC_NUM_CHANNELS; i++) {
                ADCResults[i] = ADC_GetResult16(i);
            }
            ADCDataReady = true;
            VDAC_SetValueBuffered(ADCResults[0]); // debug only
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
    
    //Cy_GPIO_Write(Int_Debug_PORT, Int_Debug_NUM, 0);
}

/**
 * @brief main function
 */
int main(void) {
    __enable_irq();  // Enable global interrupts

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
    
    // Stores output data to print to terminal
    char buffer[1024];
    
    // Stores valid ADC conversion results
    int16_t ADCNewReading[ADC_NUM_CHANNELS];
    float ADCNewVolts[ADC_NUM_CHANNELS];
    
    // Variables for SO2 and hemoglobin concentration calculations
    volatile uint16_t L680 ; // Time Slot A Channel 1 (680 nm laser)
    volatile uint16_t L850 ; // Time Slot B Channel 1 (850 nm laser)
    
    float R;
    float SO2;
    float R_avg;
    float SO2_avg;
    float del680;
    float del850;
    // unsigned long t;
    // unsigned long t0;
    
    // Maintain running average
    uint16_t slotA_avg[SMOOTHED_SAMPLE_SIZE] = {0};
    uint16_t slotB_avg[SMOOTHED_SAMPLE_SIZE] = {0};
    
    size_t posA = 0;
    size_t posB = 0;
    float avg_valA = 0;
    float avg_valB = 0;
    uint32_t sumA = 0;
    uint32_t sumB = 0;
    size_t lenA = sizeof(slotA_avg) / sizeof(uint16_t);
    size_t lenB = sizeof(slotB_avg) / sizeof(uint16_t);

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
    
    // Begin first ADC scan
    ADC_StartConvert();
    
    // Initialize and start Timer to periodically handle ADC conversion results
    Timer_Start();
    
    // Begin superloop
    uint16_t ADPDCount = 0; // Number of ADPD sensor reads in the current 10 Hz period
    for (;;) {
        // Do a data register read if not synced up with the 100 Hz timer interrupt
        if (timerCount != ADPDCount) {
            Cy_GPIO_Write(Int_Debug_PORT, Int_Debug_NUM, 1);
            //ADPDCount = (ADPDCount + 1) % ADPD_TO_ADC_RATIO;
            ADPDCount = timerCount;
            
            // Read data from the sensor data registers
            turbidity_ReadDataNoInterrupt();
            
            Cy_GPIO_Write(Int_Debug_PORT, Int_Debug_NUM, 0);
            
            // Raw and time avg light intensity
            L680 = au16DataSlotA[0];
            L850 = au16DataSlotB[0];
        
            avg_valA = movingAvg(slotA_avg, &sumA, posA, lenA, L680);
            avg_valB = movingAvg(slotB_avg, &sumB, posB, lenB, L850);
            
            // Update ring buffer current position
            posA++;
            if (posA >= lenA) posA = 0;
            posB++;
            if (posB >= lenB) posB = 0;
            
            // SO2%
            R = log(L680) / log(L850);
            R_avg = log(avg_valA) / log(avg_valB);
            
            SO2 = 188.1*(R) - 89.95;
            SO2_avg = 188.1*(R_avg) - 89.95;
            
            // Hemoglobin concentration
            del680 = -3.412*log(64*avg_valA / 127) + 43.02; // TODO: is PULSE_A = 127?
            del850 = -2.701*log(64*avg_valB / 127) + 35.27; // -0.1049

            // Format and print the data via UART
            snprintf(buffer, sizeof(buffer), "RawA:%d, AvgA:%f, RawB:%d, AvgB:%f, LogR:%f, \
                RawSO2:%f. AvgSO2:%f, ConA:%f, ConB:%f, #%u\r\n",
                L680, avg_valA, L850, avg_valB, R, SO2, SO2_avg, del680, del850, ADPDCount);

            printf("%s\r\n", buffer);
        }
        // Process ADC results if ready
        if (ADCDataReady) {
            Cy_GPIO_Write(Debug_PORT, Debug_NUM, 1);
            ADCDataReady = false;
            for (uint16_t i = 0; i < ADC_NUM_CHANNELS; i++) {
                ADCNewReading[i] = ADCResults[i];
                ADCNewVolts[i] = Cy_SAR_CountsTo_Volts(SAR, i, ADCNewReading[i]);
            }
            // Format and print the data via UART
            printf("***\r\nADC: CH1 %f, CH2 %f, CH3 %f, CH4 %f #%u\r\n***\r\n", ADCNewVolts[0], 
                ADCNewVolts[1], ADCNewVolts[2], ADCNewVolts[3], ADPDCount);
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

/*  send data 
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
float movingAvg(uint16_t *ptrArrNumbers, uint32_t *ptrSum, size_t pos, size_t len, uint16_t nextNum) {
    // Subtract the oldest number from the prev sum, add the new number
    *ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
    
    // Assign the nextNum to the position in the array
    ptrArrNumbers[pos] = nextNum;
    
    // return the average
    return (float)*ptrSum / len;
}

/* FreeRTOS Task Functions */
/* UNUSED - Task to handle ADPD1080 sensor data */
void vADPD1080(void *pvParameters) {
    (void) pvParameters;

    // Stores sensor data to print to stdout
    char buffer[1024];
    
    // Variables for SO2 calc in loop()
    volatile uint16_t L680 ; // Time Slot A Channel 1 (680 nm laser)
    volatile uint16_t L850 ; // Time Slot B Channel 1 (850 nm laser)
    
    float R;
    float SO2;
    float R_avg;
    float SO2_avg;
    float del680;
    float del850;
    // unsigned long t;
    // unsigned long t0;
    
    // Running average
    uint16_t slotA_avg[SMOOTHED_SAMPLE_SIZE] = { 0 };
    uint16_t slotB_avg[SMOOTHED_SAMPLE_SIZE] = { 0 };
    
    size_t posA = 0;
    size_t posB = 0;
    float avg_valA = 0;
    float avg_valB = 0;
    uint32_t sumA = 0;
    uint32_t sumB = 0;
    size_t lenA = sizeof(slotA_avg) / sizeof(uint16_t);
    size_t lenB = sizeof(slotB_avg) / sizeof(uint16_t);

    // Initialize and configure the ADPD1080 sensor
    printf("Initializing ADPD1080 sensor...\r\n");

    if (!ADPD1080_Begin(ADPD1080_ADDRESS, 0)) {
        printf("ADPD1080 initialization failed!\r\n");
        vTaskSuspend(NULL); // Suspend task on failure
    }
    
    // Initialize sensor registers
    turbidity_Init();
    //turbidity_ChannelOffsetCalibration();
    
    printf("ADPD1080 initialization successful.\r\n");
    
    for (;;) {
        // Read data from the sensor
        turbidity_ReadDataInterrupt();
        
        // Raw and time avg light intensity
        L680 = au16DataSlotA[0];
        L850 = au16DataSlotB[0];
    
        avg_valA = movingAvg(slotA_avg, &sumA, posA, lenA, L680);
        avg_valB = movingAvg(slotB_avg, &sumB, posB, lenB, L850);
        
        // Update ring buffer current position
        posA++;
        if (posA >= lenA) posA = 0;
        posB++;
        if (posB >= lenB) posB = 0;
        
        // SO2%
        R = log(L680) / log(L850);
        R_avg = log(avg_valA) / log(avg_valB);
        
        SO2 = 188.1*(R) - 89.95;
        SO2_avg = 188.1*(R_avg) - 89.95;
        
        // Hemoglobin concentration
        del680 = -3.412*log(64*avg_valA / 127) + 43.02; // TODO: is PULSE_A = 127?
        del850 = -2.701*log(64*avg_valB / 127) + 35.27; // -0.1049

        // Format and print the data via UART
        snprintf(buffer, sizeof(buffer), "RawA:%d, AvgA:%f, RawB:%d, AvgB:%f, LogR:%f, \
            RawSO2:%f. AvgSO2:%f, ConA:%f, ConB:%f\r\n",
            L680, avg_valA, L850, avg_valB, R, SO2, SO2_avg, del680, del850);

        printf("%s\r\n", buffer);

        // Delay task for the specified interval
        vTaskDelay(pdMS_TO_TICKS(ADPD1080_READ_INTERVAL_MS));
    }
}

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

/* UNUSED - Task to handle analog sensor data via SAR ADC */
void vADC(void *pvParameter) {
    (void) pvParameter;
    int16_t adc_val0, adc_val1, adc_val2, adc_val3, adc_val4, adc_val5, adc_val6, adc_val7;
    ADC_StartConvert();
    printf("Start ADC Conversion\n");
    for (;;) {
        // wait for conversion
        while(!ADC_IsEndConversion(CY_SAR_WAIT_FOR_RESULT));
        adc_val0 = Cy_SAR_GetResult16(SAR, 0);
        float32_t result0 = Cy_SAR_CountsTo_Volts(SAR,0,adc_val0);
        adc_val1 = Cy_SAR_GetResult16(SAR, 1);
        float32_t result1 = Cy_SAR_CountsTo_Volts(SAR,1,adc_val1);
        adc_val2 = Cy_SAR_GetResult16(SAR, 2);
        float32_t result2 = Cy_SAR_CountsTo_Volts(SAR,2,adc_val2);
        adc_val3 = Cy_SAR_GetResult16(SAR, 3);
        float32_t result3 = Cy_SAR_CountsTo_Volts(SAR,3,adc_val3);
        adc_val4 = Cy_SAR_GetResult16(SAR, 4);
        float32_t result4 = Cy_SAR_CountsTo_Volts(SAR,4,adc_val4);
        adc_val5 = Cy_SAR_GetResult16(SAR, 5);
        float32_t result5 = Cy_SAR_CountsTo_Volts(SAR,5,adc_val5);
        adc_val6 = Cy_SAR_GetResult16(SAR, 6);
        float32_t result6 = Cy_SAR_CountsTo_Volts(SAR,6,adc_val6);
        adc_val7 = Cy_SAR_GetResult16(SAR, 7);
        float32_t result7 = Cy_SAR_CountsTo_Volts(SAR,7,adc_val7);
    
        printf("0 to 7 is %f, %f, %f, %f, %f, %f, %f, %f\r\n", 
            result0, result1, result2, result3, result4, result5, result6, result7);
        
        vTaskDelay(pdMS_TO_TICKS(ADC_READ_INTERVAL_MS));
    }
}
