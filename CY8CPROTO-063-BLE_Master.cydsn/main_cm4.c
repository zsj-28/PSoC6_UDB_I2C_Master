/* main_cm4.c */

/* Include Files */
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ADPD1080.h"
#include <stdio.h>

// Task configuration
#define ADPD1080_TASK_STACK_SIZE 1024
#define ADC_TASK_STACK_SIZE 1024
#define ADPD1080_TASK_PRIORITY   2
#define ADC_Task_PRIORITY   3
#define READ_INTERVAL_MS         500
#define ADC_READ_INTERVAL_MS 1000
// Function prototypes
void ADPD1080_Task(void *pvParameters);
void ADC_TASK(void *pvParameters);

int main(void)
{
    __enable_irq();  // Enable global interrupts

    // Initialize UART for serial communication
    UART_Start();
    
    // Initialize I2C for sensor communication
    I2C_Start();

    // Start ADC module 
    ADC_Start();
    // Create the ADPD1080 task
    xTaskCreate(ADPD1080_Task, "ADPD1080 Task", ADPD1080_TASK_STACK_SIZE, NULL, ADPD1080_TASK_PRIORITY, NULL);
    xTaskCreate(ADC_TASK, "ADC Task", ADC_TASK_STACK_SIZE, NULL, ADC_Task_PRIORITY, NULL);
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

    //if (!ADPD1080_Init()) {
    //    printf("ADPD1080 initialization failed!\n");
    //    vTaskSuspend(NULL); // Suspend task on failure
    //}

    printf("ADPD1080 initialization successful.\n");
    
    turbidity_init();
    //while(1){}
    turbidity_ChannelOffsetCalibration();
    
    for (;;) {
        // Read data from the sensor
        ADPD1080_ReadData(au16DataSlotA, au16DataSlotB, 4);

        // Format and print the data via UART
        snprintf(buffer, sizeof(buffer), "Slot A: %d | Slot B: %d\n",
                 au16DataSlotA[0],
                 au16DataSlotB[0]);

        printf(buffer);

        // Delay task for the specified interval
        vTaskDelay(pdMS_TO_TICKS(READ_INTERVAL_MS));
    }
}

void ADC_TASK(void *pvParameter){
    (void) pvParameter;
    int16_t adc_val0, adc_val1, adc_val2, adc_val3, adc_val4, adc_val5, adc_val6, adc_val7;
    ADC_StartConvert();
    printf("Start ADC Convertion\n");
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
    
        printf("0 to 7 is %f, %f, %f, %f, %f, %f, %f\r\n", result0, result1, result2, result3, result4, result5, result6, result7);
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}