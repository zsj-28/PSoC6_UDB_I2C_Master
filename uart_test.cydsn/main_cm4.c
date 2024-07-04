#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
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

// Function prototypes
void ADPD1080_Task(void *pvParameters);
void ADC_TASK(void *pvParameters);
void send_task(void *pvParameters);

int main(void)
{
    // Initialize UART for serial communication
    UART_Start();
    
    // Initialize I2C for sensor communication
    //I2C_Start();

    // Start ADC module 
    //ADC_Start();

    __enable_irq();  // Enable global interrupts
    // Create the ADPD1080 task
    //xTaskCreate(ADPD1080_Task, "ADPD1080 Task", ADPD1080_TASK_STACK_SIZE, NULL, ADPD1080_TASK_PRIORITY, NULL);
    //xTaskCreate(ADC_TASK, "ADC Task", ADC_TASK_STACK_SIZE, NULL, ADC_Task_PRIORITY, NULL);
    xTaskCreate(send_task, "send data", 1024, NULL, 2, NULL);
    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    for (;;) {
        // This point should never be reached
    }
}

#define OPCODE_ADC_0 0x01
#define OPCODE_ADC_1 0x02
#define OPCODE_ADC_2 0x03
#define OPCODE_ADC_3 0x04
#define OPCODE_ADC_4 0x05
#define OPCODE_ADC_5 0x06
#define OPCODE_ADC_6 0x07
#define OPCODE_ADC_7 0x08
#define OPCODE_I2C 0x09

/* calculate crc8 for specific data*/
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

void send_task(void *pvParameters){
    while(1){
        uint16_t data = 0x1234;
        send_data(data, 0x0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
   }