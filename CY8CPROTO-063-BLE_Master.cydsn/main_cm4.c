/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================*/
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>

#define TASK_I2C_PRIORITY (configMAX_PRIORITIES - 1)
#define I2C_QUEUE_LENGTH  (5)
#define I2C_ITEM_SIZE     (sizeof(I2C_Message_t))

QueueHandle_t i2cQueue;

typedef struct {
    uint8_t address;
    uint8_t reg;
    uint8_t data;
} I2C_Message_t;

TaskHandle_t i2cWriteTaskHandle;
TaskHandle_t i2cReadTaskHandle;

void Write_I2C(void *pvParameters);
void Read_I2C(void *pvParameters);

int main()
{   
    I2C_Start();
    UART_Start();
    setvbuf(stdin, NULL, _IONBF, 0);
    xTaskCreate(Write_I2C, "I2C Write", configMINIMAL_STACK_SIZE, NULL, TASK_I2C_PRIORITY, &i2cWriteTaskHandle);
    xTaskCreate(Read_I2C, "I2C Read", configMINIMAL_STACK_SIZE, NULL, TASK_I2C_PRIORITY, &i2cReadTaskHandle);

    i2cQueue = xQueueCreate(I2C_QUEUE_LENGTH, I2C_ITEM_SIZE);
    if (i2cQueue == NULL)
    {
        CY_ASSERT(0);
    }

    vTaskStartScheduler();
    
    CY_ASSERT(0);

    for(;;)
    {
    }    
}

void Write_I2C(void *pvParameters)
{
    I2C_Message_t i2cMessage;
    uint8_t randomAddress = 0x27; // I2C address of Arduino
    uint8_t randomReg = 0x01;     // Arbitrary register, not used in this context

    for (;;)
    {
        // Generate random data
        i2cMessage.address = randomAddress;
        i2cMessage.reg = randomReg;
        i2cMessage.data = 0x2; // Random data

        // Perform I2C write operation
        uint8_t writeData[2] = {i2cMessage.reg, i2cMessage.data};
        I2C_MasterWriteBuf(i2cMessage.address, writeData, 2, I2C_MODE_COMPLETE_XFER);

        // Wait for the transfer to complete
        while (I2C_MasterStatus() & I2C_MSTAT_XFER_INP);
        // Add a delay to observe on logic analyzer
        vTaskDelay(pdMS_TO_TICKS(100)); // 100ms delay for clear signal observation
    }
}

void Read_I2C(void *pvParameters)
{
    uint8_t slaveAddress = 0x27; // Use the same address as your Arduino
    uint8_t readData[1];         // Buffer to store read data

    for (;;)
    {
        // Send read request to slave
        I2C_MasterReadBuf(slaveAddress, readData, 1, I2C_MODE_COMPLETE_XFER);

        // Wait for the transfer to complete
        while (I2C_MasterStatus() & I2C_MSTAT_XFER_INP);

        // Print the read data or process it
        printf("Received data: 0x%02X\n", readData[0]);

        // Add a delay to avoid constant polling
        vTaskDelay(pdMS_TO_TICKS(200)); // 500ms delay for next read
    }
}

void vApplicationIdleHook(void)
{
    Cy_SysPm_Sleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
}

void vApplicationStackOverflowHook(TaskHandle_t *pxTask, signed char *pcTaskName)
{
    (void)pxTask;
    CY_ASSERT(0);
}

void vApplicationMallocFailedHook(void)
{
    CY_ASSERT(0);
}


/* [] END OF FILE */
