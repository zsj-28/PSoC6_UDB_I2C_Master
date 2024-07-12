/**
 * @file main_cm4.c 
 */

/* Include files */
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
#define SMOOTHED_SAMPLE_SIZE 50
#define ADCNumChannels        4u

/* Global variables */
volatile uint16_t timerCount = 0;
volatile bool ADCDataReady = false;
volatile int16_t ADCResults[ADCNumChannels];

/* Functions */
/**
 * @brief handles interrupt on timer overflow with frequency 100Hz
 * every 10 interrupts, gets previous conversion results and starts next conversion
 * 
 * @param TimerInt_Handler - interrupt handler function name
 *
 * @return none
 */
CY_ISR (Timer_Int_Handler) {
    Cy_GPIO_Write(Int_Debug_PORT, Int_Debug_NUM, 1);
    
    // Clear timer overflow interrupt
    Timer_ClearInterrupt(CY_TCPWM_INT_ON_TC);
    timerCount++;
    
    // Read ADC conversion results with frequency 10 Hz
    if (timerCount == 10) {
        timerCount = 0;
        // Check conversion status without blocking
        if (ADC_IsEndConversion(CY_SAR_RETURN_STATUS)) {
            for (uint16_t i = 0; i < ADCNumChannels; i++) {
                ADCResults[i] = ADC_GetResult16(i);
            }
            ADCDataReady = true;
        }
        else {
            printf("Conversion not finished yet!\r\n");
            return;
        }
        
        // Start next conversion
        ADC_StartConvert();
    }
    
    // Flush write to hardware by reading from same register
    Timer_GetInterruptStatus();
    
    Cy_GPIO_Write(Int_Debug_PORT, Int_Debug_NUM, 0);
}

int main(void) {
    __enable_irq();  // Enable global interrupts

    // Initialize UART for serial communication
    UART_Start();
    
    // Initialize I2C for digital sensor communication
    I2C_Start();
    
    // Intialize ADC for receiving analog sensor data
    ADC_Start();

    // Initialize Timer for periodic handling of ADC conversion results
    Timer_Start();
    
    // Register Timer interrupt handler
    Timer_Int_StartEx(Timer_Int_Handler);
    
    // Stores output data to print to terminal
    char buffer[1024];
    
    // Stores valid ADC conversion results
    int16_t ADCNewReading[ADCNumChannels];
    float ADCNewVolts[ADCNumChannels];
    
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
    
    // TODO: trigger SW start of timer?
    
    // Begin superloop
    uint16_t ADPDCount = 0; // Number of ADPD sensor reads in the current 10 Hz period
    for (;;) {
        // Process ADC results if ready
        if (ADCDataReady) {
            ADCDataReady = false;
            for (uint16_t i = 0; i < ADCNumChannels; i++) {
                ADCNewReading[i] = ADCResults[i];
                ADCNewVolts[i] = Cy_SAR_CountsTo_Volts(SAR, i, ADCNewReading[i]);
            }
            // Format and print the data via UART
            printf("ADC: CH1 %f, CH2 %f, CH3 %f, CH4 %f\r\n", ADCNewVolts[0], ADCNewVolts[1],
                ADCNewVolts[2], ADCNewVolts[3]);
        }
        // Do a data register read if not synced up with the 100 Hz timer interrupt
        if (timerCount != ADPDCount) {
            ADPDCount = (ADPDCount + 1) % 10;
            
            // Read data from the sensor data registers
            turbidity_ReadDataNoInterrupt();
            
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
        }
    }
}

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
