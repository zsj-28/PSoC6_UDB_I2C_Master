#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ADPD1080.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

// Task configuration
#define ADPD1080_TASK_STACK_SIZE 1024
#define ADC_TASK_STACK_SIZE 1024
#define ADPD1080_TASK_PRIORITY   2
#define ADC_Task_PRIORITY   3
#define READ_INTERVAL_MS         500
#define ADC_READ_INTERVAL_MS 1000

#define OPCODE_ADC_0 0x01
#define OPCODE_ADC_1 0x02
#define OPCODE_ADC_2 0x03
#define OPCODE_ADC_3 0x04
#define OPCODE_ADC_4 0x05
#define OPCODE_ADC_5 0x06
#define OPCODE_ADC_6 0x07
#define OPCODE_ADC_7 0x08
#define OPCODE_I2C 0x09

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


// Function prototypes
void ADPD1080_Task(void *pvParameters);
void ADC_TASK(void *pvParameters);

int main(void)
{
    // Initialize UART for serial communication
    UART_Start();
    
    // Initialize I2C for sensor communication
    I2C_Start();

    // Start ADC module 
    ADC_Start();

    __enable_irq();  // Enable global interrupts
    // Create the ADPD1080 task
    xTaskCreate(ADPD1080_Task, "ADPD1080 Task", ADPD1080_TASK_STACK_SIZE, NULL, ADPD1080_TASK_PRIORITY, NULL);
    xTaskCreate(ADC_TASK, "ADC Task", ADC_TASK_STACK_SIZE, NULL, ADC_Task_PRIORITY, NULL);
    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    for (;;) {
        // This point should never be reached
    }
}


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
    adc_channel goes from 0 - 8 
    0x0-0x7 are 8 ADC channels, 0x8 is the I2C ADPD readings*/
void send_data(uint16_t adc_value, uint16_t adc_channel) {
    uint8_t data[2];
    data[0] = (adc_value >> 8) & 0xFF;
    data[1] = adc_value & 0xFF;
    wrap_data(OPCODE_ADC_0 + adc_channel, data, 2);
}


// Task to handle ADPD1080 sensor data
void ADPD1080_Task(void *pvParameters)
{
    (void) pvParameters;

    char buffer[128];
    volatile uint16_t au16DataSlotA[4] = {0, 0, 0, 0};
    volatile uint16_t au16DataSlotB[4] = {0, 0, 0, 0};

    // Initialize and configure the ADPD1080 sensor
    //printf("Initializing ADPD1080 sensor...\n");

    //if (!ADPD1080_Init()) {
    //    printf("ADPD1080 initialization failed!\n");
    //    vTaskSuspend(NULL); // Suspend task on failure
    //}

    //printf("ADPD1080 initialization successful.\n");
    
    turbidity_init();
    //while(1){}
    turbidity_ChannelOffsetCalibration();
    
    for (;;) {
        // Read data from the sensor
        ADPD1080_ReadData(au16DataSlotA, au16DataSlotB, 4);

        // Format and print the data via UART
        snprintf(buffer, sizeof(buffer), "Slot A: %d | Slot B: %d\r\n",
                 au16DataSlotA[0],
                 au16DataSlotB[0]);
        
        //float R = log(au16DataSlotA[0]) / log(au16DataSlotB[0]);
        send_data(au16DataSlotB[0], 8);
        //printf("The R is %f\r\n",R);

        //printf(buffer);

        // Delay task for the specified interval
        vTaskDelay(pdMS_TO_TICKS(READ_INTERVAL_MS));
    }
}

void ADC_TASK(void *pvParameter){
    (void) pvParameter;
    int16_t adc_val0, adc_val1, adc_val2, adc_val3, adc_val4, adc_val5, adc_val6, adc_val7;
    ADC_StartConvert();
    //printf("Start ADC Convertion\n");
    for(;;){
        //wait for conversion
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
        
        send_data(result0, 0);
        send_data(result1, 1);
        send_data(result2, 2);
        send_data(result3, 3);
        send_data(result4, 4);
        send_data(result5, 5);
        send_data(result6, 6);
        send_data(result7, 7);
    
        //printf("0 to 7 is %f, %f, %f, %f, %f, %f, %f\r\n", result0, result1, result2, result3, result4, result5, result6, result7);
        
        
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}