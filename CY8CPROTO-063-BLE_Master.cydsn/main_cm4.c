/**
 * @file main_cm4.c 
 */

/* Include Files */
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ADPD1080.h"
#include <stdio.h>
#include <math.h>

/* Task configuration */
#define ADPD1080_TASK_STACK_SIZE          1024
#define ADC_TASK_STACK_SIZE               1024
#define ADPD1080_TASK_PRIORITY            2
#define ADC_TASK_PRIORITY                 3
#define ADPD1080_READ_INTERVAL_MS         15
#define ADC_READ_INTERVAL_MS              100

/* Function prototypes */
void vADPD1080(void *pvParameters);
void vADC(void *pvParameters);

/* Constants */
static size_t SMOOTHED_SAMPLE_SIZE = 50;

int main(void)
{
    __enable_irq();  // Enable global interrupts

    // Initialize UART for serial communication
    UART_Start();
    
    // Initialize I2C for sensor communication
    I2C_Start();

    // Start ADC module 
    //ADC_Start();
    
    // Create the ADPD1080 task
    xTaskCreate(vADPD1080, "ADPD1080", ADPD1080_TASK_STACK_SIZE, NULL, ADPD1080_TASK_PRIORITY, NULL);
    //xTaskCreate(vADC, "ADC", ADC_TASK_STACK_SIZE, NULL, ADC_TASK_PRIORITY, NULL);
    
    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    for (;;) {
        // This point should never be reached
    }
}

/* Task to handle ADPD1080 sensor data */
void vADPD1080(void *pvParameters)
{
    (void) pvParameters;

    // Stores sensor data to print to stdout
    char buffer[1024];
    
    // Variables for SO2 calc in loop()
    float R;
    float SO2;
    float R_avg;
    float SO2_avg;
    float del680;
    float del850;
    unsigned long t;
    unsigned long t0;
    float avg_valA;
    float avg_valB;
    
    volatile uint16_t L680 ; // Time Slot A Channel 1 (680 nm laser)
    volatile uint16_t L850 ; // Time Slot B Channel 1 (850 nm laser)
    
    // Running average

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
        
        // SO2%
        R = log(L680)/log(L850);
        SO2 = 188.1*(R) - 89.95;
        
        // Hemoglobin concentration

        // Format and print the data via UART
        snprintf(buffer, sizeof(buffer), "RawA: %d, RawB: %d, LogR: %f, RawSO2: %f\n",
                 L680, L850, R, SO2);

        printf("%s\r\n", buffer);

        // Delay task for the specified interval
        vTaskDelay(pdMS_TO_TICKS(ADPD1080_READ_INTERVAL_MS));
    }
}

void vADC(void *pvParameter){
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