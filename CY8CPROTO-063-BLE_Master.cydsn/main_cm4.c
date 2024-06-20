#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ADPD1080.h"
#include <stdio.h>

// Task configuration
#define ADPD1080_TASK_STACK_SIZE 1024
#define ADPD1080_TASK_PRIORITY   2
#define READ_INTERVAL_MS         500

// Function prototypes
void ADPD1080_Task(void *pvParameters);

int main(void)
{
    __enable_irq();  // Enable global interrupts

    // Initialize UART for serial communication
    UART_Start();
    
    // Initialize I2C for sensor communication
    I2C_Start();

    // Create the ADPD1080 task
    xTaskCreate(ADPD1080_Task, "ADPD1080 Task", ADPD1080_TASK_STACK_SIZE, NULL, ADPD1080_TASK_PRIORITY, NULL);

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    for (;;) {
        // This point should never be reached
    }
}

// Task to handle ADPD1080 sensor data
void ADPD1080_Task(void *pvParameters)
{
    (void) pvParameters;

    char buffer[128];
    volatile uint16_t au16DataSlotA[4] = {0, 0, 0, 0};
    volatile uint16_t au16DataSlotB[4] = {0, 0, 0, 0};

    // Initialize and configure the ADPD1080 sensor
    printf("Initializing ADPD1080 sensor...\n");

    if (!ADPD1080_Init()) {
        printf("ADPD1080 initialization failed!\n");
        vTaskSuspend(NULL); // Suspend task on failure
    }

    printf("ADPD1080 initialization successful.\n");
    turbidity_init();
    //turbidity_ChannelOffsetCalibration();
    
    for (;;) {
        // Read data from the sensor
        ADPD1080_ReadData(au16DataSlotA, au16DataSlotB, 4);

        // Format and print the data via UART
        snprintf(buffer, sizeof(buffer), "Slot A: %d, %d, %d, %d | Slot B: %d, %d, %d, %d\n",
                 au16DataSlotA[0], au16DataSlotA[1], au16DataSlotA[2], au16DataSlotA[3],
                 au16DataSlotB[0], au16DataSlotB[1], au16DataSlotB[2], au16DataSlotB[3]);

        printf(buffer);

        // Delay task for the specified interval
        vTaskDelay(pdMS_TO_TICKS(READ_INTERVAL_MS));
    }
}
