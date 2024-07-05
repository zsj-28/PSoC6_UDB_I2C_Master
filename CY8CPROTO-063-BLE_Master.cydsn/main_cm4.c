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

float movingAvg(uint16_t *ptrArrNumbers, uint32_t *ptrSum, size_t pos, size_t len, uint16_t nextNum);

/* Constants */
static size_t SMOOTHED_SAMPLE_SIZE = 50;

int main(void) {
    __enable_irq();  // Enable global interrupts

    // Initialize UART for serial communication
    UART_Start();
    
    // Initialize I2C for sensor communication
    I2C_Start();

    // Start ADC module 
    ADC_Start();
    
    // Create the ADPD1080 task
    xTaskCreate(vADPD1080, "ADPD1080", ADPD1080_TASK_STACK_SIZE, NULL, ADPD1080_TASK_PRIORITY, NULL);
    xTaskCreate(vADC, "ADC", ADC_TASK_STACK_SIZE, NULL, ADC_TASK_PRIORITY, NULL);
    
    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    for (;;) {
        // This point should never be reached
    }
}

/* Task to handle ADPD1080 sensor data */
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
    uint16_t slotA_avg[SMOOTHED_SAMPLE_SIZE];
    uint16_t slotB_avg[SMOOTHED_SAMPLE_SIZE];
    
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
            RawSO2:%f. AvgSO2:%f, ConA:%f, ConB:%f",
            L680, avg_valA, L850, avg_valB, R, SO2, SO2_avg, del680, del850);

        printf("%s\r\n", buffer);

        // Delay task for the specified interval
        vTaskDelay(pdMS_TO_TICKS(ADPD1080_READ_INTERVAL_MS));
    }
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

/* Task to handle analog sensor data via SAR ADC */
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
