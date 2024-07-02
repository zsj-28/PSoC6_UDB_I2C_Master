/* ADPD1080.c */

/* Include Files */
#include "project.h"  // Include PSoC project header for PSoC-specific functions
#include "ADPD1080.h"
#include <stdio.h>

/* Initialize data slot arrays */
volatile uint16_t au16DataSlotA[4] = {0,0,0,0};
volatile uint16_t au16DataSlotB[4] = {0,0,0,0};

/* Sensor Function Definitions */
/**
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  i2cAddr
 *            The I2C address to be used.
 *    @param  sensorId
 *            The unique ID to differentiate the sensors from others
 *    @return True if initialization was successful, otherwise false.
 */
bool ADPD1080_Begin(uint8_t i2cAddr, int32_t sensorId) {
    (void) i2cAddr;
    (void) sensorId;
    
    uint16_t regValue = ADPD1080_ReadReg(ADPD1080_DEVID);  // Device ID register
    uint8_t deviceID = (uint8_t) regValue;
    printf("Device ID is 0x%x\r\n", deviceID);
    return deviceID == ADPD1080_CHIP_ID;
}

/**
 * @brief Writes a register from the ADPD1080 using I2C bus.
 * 
 * @param regAddr - register address
 * @param regValue - register data to write
 * 
 * @return true if successful - false if failure 
 */
bool ADPD1080_WriteReg(uint8_t regAddr, uint16_t regValue) {
    //uint8_t err;
    I2C_MasterSendStart(ADPD1080_ADDRESS, I2C_WRITE_XFER_MODE);
    I2C_MasterWriteByte(regAddr);
    I2C_MasterWriteByte((regValue >> 8) & 0xFF);
    I2C_MasterWriteByte(regValue & 0xFF);
    I2C_MasterSendStop();
    return true;
}

/**
 * @brief Reads a register from the ADPD1080 using I2C bus.
 * 
 * @param regAddr - register address
 * 
 * @return uint16_t - register value
 */
uint16_t ADPD1080_ReadReg(uint8_t regAddr) {
    uint8_t buffer[2] = {0};
    I2C_MasterSendStart(ADPD1080_ADDRESS, I2C_WRITE_XFER_MODE);
    I2C_MasterWriteByte(regAddr);
    I2C_MasterSendRestart(ADPD1080_ADDRESS, I2C_READ_XFER_MODE);
    buffer[0] = I2C_MasterReadByte(I2C_ACK_DATA);
    buffer[1] = I2C_MasterReadByte(I2C_NAK_DATA);
    I2C_MasterSendStop();
    return (buffer[0] << 8) | buffer[1];
    /*
    uint8_t regByte[2];
    uint8_t wrBuf[1] = {regAddr};
    I2C_MasterWriteBuf(ADPD1080_ADDRESS, wrBuf, 1, I2C_MODE_NO_STOP);
    I2C_MasterReadBuf(ADPD1080_ADDRESS, regByte, 2, I2C_MODE_REPEAT_START | I2C_MODE_NO_STOP);
    I2C_MasterSendStop();
    uint16_t regValue = ((uint16_t)regByte[0] << 8) | ((uint16_t)regByte[1]);
    return regValue
    */
}

/**
 * @brief Sets the operation mode of the ADPD1080
 * 
 * @param enMode = STANDBY, PROGRAM or NORMAL_OPERATION.
 * 
 * @return true if successful - false if failure
 */
bool ADPD1080_SetOperationMode(ADPD1080_OperationMode enMode) {
    uint16_t regValue = ADPD1080_ReadReg(ADPD1080_MODE);
    regValue &= ~(0x3 << 0);
    regValue |= (enMode << 0);
    return ADPD1080_WriteReg(ADPD1080_MODE, regValue);
}
/**
 * @brief Configure the Time Slot Switch register.
 *
 * @param u8SlotASel - SlotA input
 * 
 * @param u8SlotBSel - SlotB input
 * 
 * @return true if successful - false if failure 
 */
bool ADPD1080_SetTimeSlotSwitch(ADPD1080_TimeSlotPD u8SlotASel, ADPD1080_TimeSlotPD u8SlotBSel){
    uint16_t regValue = ADPD1080_ReadReg(ADPD1080_PD_LED_SELECT);
    regValue &= ~(0x0F << 4);        // Clear bits 4-7
    regValue |= (u8SlotASel << 4); // Set the new value for bits 4-7
    regValue &= ~(0x0F << 8);        // Clear bits 8-11
    regValue |= (u8SlotBSel << 8); // Set the new value for bits 8-11
    
    // Write the updated value back to the ADPD1080_PD_LED_SELECT register
    return ADPD1080_WriteReg(ADPD1080_PD_LED_SELECT, regValue);
}

/**
 * @brief Enable the internal 32kHz internal clock from ADPD105 via I2C.
 * 
 * @param enableSampleClk - enables/disables 32kHz clock
 *  
 * @return true if successful - false if failure 
 */
bool ADPD1080_Set32KCLK(bool enableSampleClk){
    uint16_t regValue = ADPD1080_ReadReg(ADPD1080_SAMPLE_CLK);
    regValue &= ~(0x01 << 7);
    regValue |= (enableSampleClk << 7);
    return ADPD1080_WriteReg(ADPD1080_SAMPLE_CLK, regValue);
}

/**
 * @brief Setup FIFO for data reading
 * 
 * @param 
 *  
 * @return none
 * 
 * @remarks INT_MASK settings are a bit weird ... (see ADPD_1    ADPD1080_ino code)
 */
void ADPD1080_SetFIFO(void) {
    ADPD1080_SetOperationMode(PROGRAM);
    // Slot A enable and 32 bit extend sample data to FIFO
    ADPD1080_WriteReg(ADPD1080_SLOT_EN, 0x3131); // Slot A and Slot B enabled
    ADPD1080_WriteReg(ADPD1080_FIFO_THRESH, 0x1F00); // set FIFO threshold to 8 words data
    ADPD1080_WriteReg(ADPD1080_INT_MASK, 0x1DF); // Sends an interrupt on the Time Slot A sample 
    ADPD1080_WriteReg(ADPD1080_GPIO_CTRL, 0x101); // interrupt function provided on GPIO0, as defined in Register 0x01
                                                  // GPIO1 set to 0
    ADPD1080_WriteReg(ADPD1080_GPIO_DRV, 0x05); // GPIO0 pin enabled, always driven, active high.
}

/**
 * @brief Select the LED to be used for a time slot
 * 
 * @param enLEDNumber - specifies the LED to be used.
 * 
 * @param enSlot - time slot to be used (SlotA or SlotB)
 *  
 * @return true if successful - false if failure 
 */
bool ADPD1080_SelectLED(ADPD1080_LED enLEDNumber, ADPD1080_TimeSlot enSlot) {
    uint16_t regValue = ADPD1080_ReadReg(ADPD1080_PD_LED_SELECT);
    if (enSlot == SLOTA) {
        regValue &= ~(0x03 << 0);
        regValue |= (enLEDNumber << 0);
    } else { // SLOTB
        regValue &= ~(0x03 << 2);
        regValue |= (enLEDNumber << 2);
    }
    return ADPD1080_WriteReg(ADPD1080_PD_LED_SELECT, regValue);
}

/**
 * @brief Unselect the LEDs for each time slot.
 * 
 * @param 
 *  
 * @return true if successful - false if failure 
 */
bool ADPD1080_DeselectLEDs(void){
    return ADPD1080_WriteReg(ADPD1080_PD_LED_SELECT, 0xFFF0);
}

/**
 * @brief Reads the data register for each time slot
 * 
 * @param *dataSlotA - 
 * 
 * @param *dataSlotB - 
 * 
 * @param count - 
 *  
 * @return none
 * 
 * @remarks timer_sleep has been commented but it may be required ...
 */
void ADPD1080_ReadDataRegs(volatile uint16_t *dataSlotA, volatile uint16_t *dataSlotB, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        dataSlotA[i] = ADPD1080_ReadReg(ADPD1080_SLOTA_PD1_16BIT + i);
        dataSlotB[i] = ADPD1080_ReadReg(ADPD1080_SLOTB_PD1_16BIT + i);
    }
}

/**
 * @brief Set the width and offset for led pulse.
 * 
 * @param enSlot - time slot (SlotA or SlotB)
 * @param u8Width - the width of the led pulse (1us step)
 * @param u8Offset - the offset of the led pulse (1us step)
 * 
 * @return true if successful - false if failure 
 */
bool ADPD1080_SetLEDWidthOffset(ADPD1080_TimeSlot enSlot, uint8_t width, uint8_t offset) {
    uint16_t regValue = offset + ((width & 0x1F) << 8);
    if (enSlot == SLOTA) {
        return ADPD1080_WriteReg(ADPD1080_SLOTA_LED_PULSE, regValue);
    } 
    else { // SLOTB
        return ADPD1080_WriteReg(ADPD1080_SLOTB_LED_PULSE, regValue);
    }
}

/**
 * @brief Set the width and offset for AFE integration.
 * 
 * @param enSlot - time slot (SlotA or SlotB)
 * @param u8Width - the width of the AFE integration window (1us step)
 * @param u8Offset - the offset of the AFE integration window (1us step)
 * @param u8FineOffset - the fine offset of the AFE integration window (31.25 ns step)
 * 
 * @return true if successful - false if failure 
 */
bool ADPD1080_SetAFEWidthOffset(ADPD1080_TimeSlot enSlot, uint8_t width, uint8_t offset, uint8_t fineOffset) {
    uint16_t regValue = 0;

    if (enSlot == SLOTA) {
        regValue = (uint16_t)(fineOffset & 0x1F) + 
            (uint16_t)((offset & 0x3F) << 5) +
            (uint16_t)((width & 0x1F) << 11);
        return ADPD1080_WriteReg(ADPD1080_SLOTA_AFE_WINDOW, regValue);        
    } 
    else { // SLOTB
        regValue = (uint16_t)(fineOffset & 0x1F) +
            (uint16_t)((offset & 0x3F) << 5) +
            (uint16_t)((width & 0x1F) << 11);
        return ADPD1080_WriteReg(ADPD1080_SLOTB_AFE_WINDOW, regValue);        
    }
}

/**
 * @brief Set the transimpendance amplifier gain.
 * 
 * @param enSlot - time slot (SlotA or SlotB)
 * @param enTIAGain - TIA gain value
 * 
 * @return true if successful - false if failure 
 */
bool ADPD1080_SetTIAGain(uint8_t enSlot, ADPD1080_TIAGain enTIAGain) {
    if (enSlot == SLOTA) {
        return ADPD1080_WriteReg(ADPD1080_SLOTA_TIA_CFG, enTIAGain);
    } 
    else { // SLOTB
        return ADPD1080_WriteReg(ADPD1080_SLOTB_TIA_CFG, enTIAGain);
    }
}

/**
 * @brief Set the sampling frequency value.
 * 
 * @param u16Frequency - sampling frequency value
 * 
 * @return true if successful - false if failure 
 */
bool ADPD1080_SetSamplingFrequency(uint16_t frequency) {
    uint16_t regValue = 32000 / frequency / 4;

    return ADPD1080_WriteReg(ADPD1080_FSAMPLE, regValue);
}

/**
 * @brief Set the value of the average factor N.
 * 
 * @param enAverage - average factor value
 * 
 * @return true if successful - false if failure 
 */
bool ADPD1080_SetAverageFactor(ADPD1080_AverageN enAverage) {
    uint16_t regValue = (enAverage << 4) | (enAverage << 8);

    return ADPD1080_WriteReg(ADPD1080_NUM_AVG, regValue);
}

/**
 * @brief Set the ADC clock speed.
 * 
 * @param enADCClock - ADC clock speed value
 * 
 * @return true if successful - false if failure 
 */
bool ADPD1080_SetADCClock(ADPD1080_ADCClockSpeed enADCClock){
  return ADPD1080_WriteReg(ADPD1080_ADC_CLOCK, enADCClock);
}

/**
 * @brief Enable the digital clock.
 * 
 * @return true if successful - false if failure 
 */
bool ADPD1080_SetDigitalClock(void) {
    uint16_t origin = ADPD1080_ReadReg(ADPD1080_DATA_ACCESS_CTL);
    uint16_t new = origin | 0x1;
    return ADPD1080_WriteReg(ADPD1080_DATA_ACCESS_CTL, new);
}

/**
 * @brief Software reset of the ADPD1080
 * 
 * @return true if successful - false if failure 
 */
bool ADPD1080_Reset(void) {
    return ADPD1080_WriteReg(ADPD1080_SW_RESET, 1); // write 0x0001; see datasheet p.68
}

/**
 * @brief Set channel offset.
 * 
 * @param enSlot - time slot (SlotA or SlotB)
 * @param stOffset - offset value
 * 
 * @return true if successful - false if failure
 */
void ADPD1080_SetOffset(ADPD1080_TimeSlot enSlot, struct ADPD1080_ChannelOffset stOffset) {
    if (enSlot == SLOTA) {
        ADPD1080_WriteReg(ADPD1080_SLOTA_CH1_OFFSET, stOffset.CH1Offset);
        ADPD1080_WriteReg(ADPD1080_SLOTA_CH2_OFFSET, stOffset.CH2Offset);
        ADPD1080_WriteReg(ADPD1080_SLOTA_CH3_OFFSET, stOffset.CH3Offset);
        ADPD1080_WriteReg(ADPD1080_SLOTA_CH4_OFFSET, stOffset.CH4Offset);
    } else { // SLOTB
        ADPD1080_WriteReg(ADPD1080_SLOTB_CH1_OFFSET, stOffset.CH1Offset);
        ADPD1080_WriteReg(ADPD1080_SLOTB_CH2_OFFSET, stOffset.CH2Offset);
        ADPD1080_WriteReg(ADPD1080_SLOTB_CH3_OFFSET, stOffset.CH3Offset);
        ADPD1080_WriteReg(ADPD1080_SLOTB_CH4_OFFSET, stOffset.CH4Offset);
    }
}

/**
 * @brief Disables the LEDs
 *  
 * @param enSlot - time slot (SlotA or SlotB)
 * 
 * @return true if successful - false if failure 
 */
bool ADPD1080_DisableLed(ADPD1080_TimeSlot enSlot) {
    uint16_t regValue = ADPD1080_ReadReg(ADPD1080_LED_DISABLE);
    (void) enSlot;
    //if (enSlot == SLOTA) {
        //regValue = regValue & (0xFCFF | (0x01 << 8));//!!!!!!!!!!!!!!!!!!!!! 8 bit 1
        //return ADPD1080_WriteReg(ADPD1080_LED_DISABLE, regValue);
    //} else { // SLOTB
        //regValue = regValue & (0xFCFF | (0x01 << 9));//!!!!!!!!!!!!!!!!!!!!!9 bit 1
        //return ADPD1080_WriteReg(ADPD1080_LED_DISABLE, regValue);
    //}
    regValue = (regValue & 0xFCFF) | (0x0300);//!!!!!!!!!!!!!!!!!!!!! 8 bit 1
    return ADPD1080_WriteReg(ADPD1080_LED_DISABLE, regValue);
}

/**
 * @brief Enable the LEDs
 *  
 * @param enSlot - time slot (SlotA or SlotB)
 * 
 * @return true if successful - false if failure 
 */
bool ADPD1080_EnableLed(ADPD1080_TimeSlot enSlot) {
    (void) enSlot;
    uint16_t regValue = ADPD1080_ReadReg(ADPD1080_LED_DISABLE);
    //if (enSlot == SLOTA) {
        //regValue = regValue & (0xFCFF | (0x00 << 8));//!!!!!!!!!!!!!!!!!!!!!8 bit 0
        //return ADPD1080_WriteReg(ADPD1080_LED_DISABLE, regValue);
    //} else { // SLOTB
        //regValue = regValue & (0xFCFF | (0x00 << 9));//!!!!!!!!!!!!!!!!!!!!!9 bit 0
        //return ADPD1080_WriteReg(ADPD1080_LED_DISABLE, regValue);
    //}
    regValue = (regValue & 0xFCFF);//!!!!!!!!!!!!!!!!!!!!! 8 bit 1
    return ADPD1080_WriteReg(ADPD1080_LED_DISABLE, regValue);
}

/**
 * @brief Set the pulse number and period
 *  
 * @param enSlot - time slot (SlotA or SlotB)
 * @param u8PulseCount - number of pulses in selected time slot
 * @param u8PulsePeriod - period of the pulse
 * 
 * @return true if successful - false if failure 
 */
bool ADPD1080_SetPulseNumberPeriod(ADPD1080_TimeSlot enSlot, uint8_t pulseCount, uint8_t pulsePeriod) {
    uint16_t regValue = ((uint16_t)pulseCount << 8) | pulsePeriod;

    if (enSlot == SLOTA) {
        return ADPD1080_WriteReg(ADPD1080_SLOTA_NUMPULSES, regValue);
    } else { // SLOTB
        return ADPD1080_WriteReg(ADPD1080_SLOTB_NUMPULSES, regValue);
    }
}

/* Controller Function Definitions */
void turbidity_Init(void) {
    
    ADPD1080_SetOperationMode(PROGRAM);  // Set to program mode
    ADPD1080_Reset();
    ADPD1080_SetOperationMode(PROGRAM);  // Set to program mode
    register_settings();
    
    ADPD1080_Set32KCLK(true);            // Enable 32K clock
    uint16_t clkRegValue = ADPD1080_ReadReg(ADPD1080_SAMPLE_CLK);
    printf("Clk Reg Value: 0x%x\r\n", clkRegValue);
    ADPD1080_SetTimeSlotSwitch(PD_1_4_CONNECTED, PD_1_4_CONNECTED); // Select photodiodes 1-4, datasheet p.21
    
    /* Select LEDs for each time slot */
    ADPD1080_SelectLED(LEDX1, SLOTA);
    ADPD1080_SelectLED(LEDX2, SLOTB);
    
    /* Set led coarse value 100% and 10% */
    ADPD1080_WriteReg(ADPD1080_ILED1_COARSE, 0x1031);  // LED configuration
    ADPD1080_WriteReg(ADPD1080_ILED2_COARSE, 0x1034);  // LED configuration
    ADPD1080_WriteReg(ADPD1080_ILED_FINE, 0x67DF);  // LED fine

    /* Set LED pulse count */
    ADPD1080_SetPulseNumberPeriod(SLOTA, 127, 0x19);    // One pulse, 19 µs period   0x0113  275
    ADPD1080_SetPulseNumberPeriod(SLOTB, 127, 0x19);    // One pulse, 19 µs period

    /* Set LED pulse width and offset */
    ADPD1080_SetLEDWidthOffset(SLOTA, 3, 25);           // 3 µs pulse width, 23 µs LED Offset  
    ADPD1080_SetLEDWidthOffset(SLOTB, 3, 25);           // 3 µs pulse width, 23 µs LED Offset

    /* Set AFE width and offset */
    ADPD1080_SetAFEWidthOffset(SLOTA, 4, 16, 4);        // (slot, AFE width, offset, fine offset) - AFE width is usually LED pulse width + 1µs
    ADPD1080_SetAFEWidthOffset(SLOTB, 4, 16, 4);        // 3 µs AFE width, 13 µs offset

    /* Set ADC clock speed */
    ADPD1080_SetADCClock(ADC_CLOCK_100MHz);
    
    /* Enable digital clock */
    ADPD1080_SetDigitalClock();

    /* Set sampling frequency */
    ADPD1080_SetSamplingFrequency(400); // For 400 Hz sampling frequency set to 1600. Currently 100 Hz
    
    /* Set TIA gain */
    ADPD1080_SetTIAGain(SLOTA, TIA_25);
    ADPD1080_SetTIAGain(SLOTB, TIA_25);

    /* Set sample clock */
    ADPD1080_WriteReg(ADPD1080_SAMPLE_CLK, 0x2695);  // 0080,2695
    ADPD1080_WriteReg(ADPD1080_CLK32M_ADJUST, 0x0098);  // 0x005e,0098
    ADPD1080_WriteReg(ADPD1080_EXT_SYNC_SEL, 0x2090);  // 20d0,2090
    ADPD1080_WriteReg(ADPD1080_CLK32M_CAL_EN, 0x0000);  // 0040,0000

    // Set V_CATHODE to 1.3V, enable all channels
    ADPD1080_WriteReg(ADPD1080_AFE_PWR_CFG1, 0x7006);  // 1806,7006
    ADPD1080_SetAverageFactor(AVERAGE2);
    
    struct ADPD1080_ChannelOffset stOffsetA  = {8201,0,0,0};      //set to be <1% of full scale ADC (65535)
    struct ADPD1080_ChannelOffset stOffsetB  = {8201,0,0,0};    
    ADPD1080_SetOffset(SLOTA, stOffsetA);
    ADPD1080_SetOffset(SLOTB, stOffsetB);
    
    /* Set FIFO and start measurement */
    ADPD1080_SetFIFO();
    
    ADPD1080_SetOperationMode(NORMAL_OPERATION);
}

void turbidity_init(void) {
    
    ADPD1080_SetOperationMode(0x01);  // Set to program mode
    ADPD1080_WriteReg(0x0,0xFF);
    ADPD1080_WriteReg(0x0F, 0x01);    // Reset
    
    ADPD1080_SetOperationMode(0x01);  // Set to program mode
    ADPD1080_Set32KCLK(1);            // Enable 32K clock
    ADPD1080_SetTimeSlotSwitch(0x5, 0x5);
  /* Select LEDs for each time slot */
    ADPD1080_SelectLED(0x02, 0x0);
    //ADPD1080_SelectLED(0x01, 0x1);
    
    ADPD1080_WriteReg(0x24, 0x1004);  // LED configuration
    ADPD1080_WriteReg(0x23, 0x1000);  // LED configuration
    ADPD1080_WriteReg(0x25, 0x67DF);  // LED fine

    ADPD1080_WriteReg(0x31, 0x1019);  // Pulse number and period
    ADPD1080_WriteReg(0x36, 0x1019);  // Pulse number and period

    ADPD1080_WriteReg(0x30, 0x0220);  // LED width and offset
    ADPD1080_WriteReg(0x35, 0x0220);  // LED width and offset

    ADPD1080_WriteReg(0x39, 0x1AE0);  // AFE width and offset
    ADPD1080_WriteReg(0x3B, 0x1AE0);  // AFE width and offset

    //ADPD1080_WriteReg(0x4E, 0x40);  // ADC clock
    //ADPD1080_WriteReg(0x58, 0x01);    // Digital clock

    ADPD1080_WriteReg(0x12, 0x5);     // Sampling frequency
    
    ADPD1080_WriteReg(0x42, 0x1C37);  // TIA gain
    ADPD1080_WriteReg(0x44, 0x1C37);  // TIA gain

    ADPD1080_WriteReg(0x4B, 0x2695);  // Sample clock
    ADPD1080_WriteReg(0x4D, 0x0098);  // Adjust clock
    ADPD1080_WriteReg(0x4F, 0x2090);  // External sync
    ADPD1080_WriteReg(0x50, 0x0000);  // Calibration

    ADPD1080_WriteReg(0x3C, 0x7006);  // AFE power configuration
    // FIFO setup
    ADPD1080_WriteReg(0x11, 0x3131); 
    ADPD1080_WriteReg(0x06, 0x1F00);
    ADPD1080_WriteReg(0x01, 0xC0FF);
    ADPD1080_WriteReg(0x0B, 0x101);
    ADPD1080_WriteReg(0x02, 0x05);
    
    ADPD1080_SetOperationMode(0x02);  // Set to normal operation
}


/* 
The interrupt handler must perform the following:
a. Read Register 0x00 and observe Bit 5 or Bit 6 to confirm
which interrupt occurred. This step is not required if
only one interrupt is in use.
b. Read the data registers before the next sample can be
written. The system must have interrupt latency and service time short enough to respond before 
the next data update, based on the output data rate.
c. Write a 1 to Bit 5 or Bit 6 in Register 0x00 to clear the interrupt.
*/
void turbidity_ReadDataInterrupt(void) {
    volatile uint16_t status = ADPD1080_ReadReg(ADPD1080_STATUS);
    printf("Status: 0x%x\r\n", status);
    ADPD1080_ReadDataRegs(au16DataSlotA, au16DataSlotB, 4);
  
    // Clear all interrupts
    ADPD1080_WriteReg(ADPD1080_STATUS, 0xFF);      
}

void turbidity_ChannelOffsetCalibration(void) {
    ADPD1080_SetOperationMode(0x01);  // Set to program mode
    // ADPD1080_WriteReg(0x34, 0x0100);  // Disable LED
    // ADPD1080_WriteReg(0x34, 0x0100);  // Disable LED/////////
    //uint16_t reg34Value = ADPD1080_ReadReg(0x34);
    //reg34Value |= 0x0300;
    //ADPD1080_WriteReg(0x34, reg34Value);  // Disable LED/////////
    ADPD1080_DisableLed(0);
    //ADPD1080_DisableLed(1);

    //ADPD1080_WriteReg(0x18, 0x2078);  // Channel offset////////////////
    //ADPD1080_WriteReg(0x19, 0x0000);  // Channel offset
    //ADPD1080_WriteReg(0x1A, 0x0000);  // Channel offset
    //ADPD1080_WriteReg(0x1B, 0x0000);  // Channel offset

    //ADPD1080_WriteReg(0x1E, 0x2078);  // Channel offset
    //ADPD1080_WriteReg(0x1F, 0x0000);  // Channel offset
    //ADPD1080_WriteReg(0x20, 0x0000);  // Channel offset
    //ADPD1080_WriteReg(0x21, 0x0000);  // Channel offset

    ADPD1080_WriteReg(0x31, 0x1019);  // Pulse number and period/////////////////
    ADPD1080_WriteReg(0x36, 0x1019);  // Pulse number and period////////////////////

    ADPD1080_SetOperationMode(0x02);  // Set to normal operation
    CyDelay(1000);

    ADPD1080_ReadDataRegs(au16DataSlotA, au16DataSlotB, 4);////////////////

    ADPD1080_SetOperationMode(0x01);  // Set to program mode
    //ADPD1080_WriteReg(0x18, au16DataSlotA[0] - 100);  // Set offset for slot A
    //ADPD1080_WriteReg(0x1E, au16DataSlotB[0] - 100);  // Set offset for slot B
    
    ADPD1080_EnableLed(0);
    //ADPD1080_EnableLed(1);

    //reg34Value = ADPD1080_ReadReg(0x34);
    //reg34Value |= 0x0000;
    //ADPD1080_WriteReg(0x34, reg34Value);  // Enable LED
    //ADPD1080_WriteReg(0x34, 0x0300);  // Enable LED

    ADPD1080_SelectLED(0x02, 0x0);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //ADPD1080_SelectLED(0x01, 0x1);//????????????
    
    //uint16_t reg14Value = ADPD1080_ReadReg(0x14);
    //reg14Value = (reg14Value & 0xFCFF) | 0x0300;//!!!!!!!!!!!!!!!!!!!!!!!
    //ADPD1080_WriteReg(0x14, reg14Value);
    //ADPD1080_WriteReg(0x14, 0x0002);//??????????????????????????
    // ADPD1080_WriteReg(0x14, 0x000F);////////

    ADPD1080_WriteReg(0x31, 0x1019);  // Pulse number and period/////////////////
    ADPD1080_WriteReg(0x36, 0x1019);  // Pulse number and period////////////////////

    ADPD1080_WriteReg(0x15, 0x0110); //?//////////////////

    ADPD1080_SetOperationMode(0x02);  // Set to normal operation
}

/* Helper function for turbidity_Init() */
void register_settings(void) {

    ADPD1080_WriteReg(0x00,0x0000); // TODO: 0x8000 didn't seem to help
    ADPD1080_WriteReg(0x01,0xC0FF);
    ADPD1080_WriteReg(0x02,0x0005);
    ADPD1080_WriteReg(0x03,0x0000);
    ADPD1080_WriteReg(0x04,0x0000);
    ADPD1080_WriteReg(0x05,0x0000);
    ADPD1080_WriteReg(0x06,0x1F00);
    ADPD1080_WriteReg(0x07,0x0060);
    ADPD1080_WriteReg(0x08,0x0A16);
    ADPD1080_WriteReg(0x09,0x00C8);
    ADPD1080_WriteReg(0x0A,0x0000);
    ADPD1080_WriteReg(0x0B,0x0000);
    ADPD1080_WriteReg(0x0C,0x0000);
    ADPD1080_WriteReg(0x0D,0x0000);
    ADPD1080_WriteReg(0x0E,0x0000);
    ADPD1080_WriteReg(0x0F,0x0000);
    ADPD1080_WriteReg(0x10,0x0000);
    ADPD1080_WriteReg(0x11,0x3131);
    ADPD1080_WriteReg(0x12,0x0050);
    ADPD1080_WriteReg(0x13,0x0320);
    ADPD1080_WriteReg(0x14,0x0336);
    ADPD1080_WriteReg(0x15,0x0110);
    ADPD1080_WriteReg(0x16,0x3000);
    ADPD1080_WriteReg(0x17,0x0000);
    ADPD1080_WriteReg(0x18,0x0000);
    ADPD1080_WriteReg(0x19,0x0000);
    ADPD1080_WriteReg(0x1A,0x0000);
    ADPD1080_WriteReg(0x1B,0x0000);
    ADPD1080_WriteReg(0x1C,0x0000);
    ADPD1080_WriteReg(0x1D,0x0000);
    ADPD1080_WriteReg(0x1E,0x0000);
    ADPD1080_WriteReg(0x1F,0x0000);
    ADPD1080_WriteReg(0x20,0x0000);
    ADPD1080_WriteReg(0x21,0x0000);
    ADPD1080_WriteReg(0x26,0x0000);
    ADPD1080_WriteReg(0x27,0x0700);
    ADPD1080_WriteReg(0x28,0x0000);
    ADPD1080_WriteReg(0x29,0x4000);
    ADPD1080_WriteReg(0x2A,0x0000);
    ADPD1080_WriteReg(0x2B,0x0000);
    ADPD1080_WriteReg(0x2C,0x0000);
    ADPD1080_WriteReg(0x2D,0x0000);
    ADPD1080_WriteReg(0x2E,0x0000);
    ADPD1080_WriteReg(0x2F,0x0000);
    ADPD1080_WriteReg(0x30,0x0219);
    ADPD1080_WriteReg(0x31,0x7F1E);
    ADPD1080_WriteReg(0x32,0x0320);
    ADPD1080_WriteReg(0x33,0x0818);
    ADPD1080_WriteReg(0x34,0x0000);
    ADPD1080_WriteReg(0x35,0x0219);
    ADPD1080_WriteReg(0x36,0x7F1E);
    ADPD1080_WriteReg(0x37,0x0000);
    ADPD1080_WriteReg(0x38,0x0000);
    ADPD1080_WriteReg(0x39,0x1A04);
    ADPD1080_WriteReg(0x3A,0x22FC);
    ADPD1080_WriteReg(0x3B,0x1A04);
    ADPD1080_WriteReg(0x3C,0x7006);
    ADPD1080_WriteReg(0x3D,0x0000);
    ADPD1080_WriteReg(0x3E,0x0320);
    ADPD1080_WriteReg(0x3F,0x0320);
    ADPD1080_WriteReg(0x40,0x1010);
    ADPD1080_WriteReg(0x41,0x004C);
    ADPD1080_WriteReg(0x42,0x1C37);
    ADPD1080_WriteReg(0x43,0xADA5);
    ADPD1080_WriteReg(0x44,0x1C37);
    ADPD1080_WriteReg(0x45,0xADA5);
    ADPD1080_WriteReg(0x46,0x0000);
    ADPD1080_WriteReg(0x47,0x0080);
    ADPD1080_WriteReg(0x48,0x0000);
    ADPD1080_WriteReg(0x49,0x0000);
    ADPD1080_WriteReg(0x4A,0x0000);
    ADPD1080_WriteReg(0x4B,0x2695);
    ADPD1080_WriteReg(0x4C,0x0004);
    ADPD1080_WriteReg(0x4D,0x0098);
    ADPD1080_WriteReg(0x4E,0x7040);
    ADPD1080_WriteReg(0x4F,0x2090);
    ADPD1080_WriteReg(0x50,0x0000);
    ADPD1080_WriteReg(0x51,0x0000);
    ADPD1080_WriteReg(0x52,0x0040);
    ADPD1080_WriteReg(0x53,0xE400);
    ADPD1080_WriteReg(0x54,0x0AA0);
    ADPD1080_WriteReg(0x55,0x0000);
    ADPD1080_WriteReg(0x56,0x0000);
    ADPD1080_WriteReg(0x57,0x0000);
    ADPD1080_WriteReg(0x58,0x0000);
    ADPD1080_WriteReg(0x59,0x0808);
    ADPD1080_WriteReg(0x5A,0x0010);
    ADPD1080_WriteReg(0x5B,0x0000);
    ADPD1080_WriteReg(0x5C,0x0000);
    ADPD1080_WriteReg(0x5D,0x0000);
    ADPD1080_WriteReg(0x5E,0x0808);
    ADPD1080_WriteReg(0x5F,0x0000);
    ADPD1080_WriteReg(0x60,0x0000);
    ADPD1080_WriteReg(0x61,0x0000);
    ADPD1080_WriteReg(0x62,0x0000);
    ADPD1080_WriteReg(0x63,0x0000);
    ADPD1080_WriteReg(0x64,0x0000);
    ADPD1080_WriteReg(0x65,0x0000);
    ADPD1080_WriteReg(0x66,0x0000);
    ADPD1080_WriteReg(0x67,0x0000);
    ADPD1080_WriteReg(0x68,0x0000);
    ADPD1080_WriteReg(0x69,0x0000);
    ADPD1080_WriteReg(0x6A,0x0000);
    ADPD1080_WriteReg(0x6B,0x0000);
    ADPD1080_WriteReg(0x6C,0x0000);
    ADPD1080_WriteReg(0x6D,0x0000);
    ADPD1080_WriteReg(0x6E,0x0000);
    ADPD1080_WriteReg(0x6F,0x0000);
    ADPD1080_WriteReg(0x70,0x0000);
    ADPD1080_WriteReg(0x71,0x0000);
    ADPD1080_WriteReg(0x72,0x0000);
    ADPD1080_WriteReg(0x73,0x0000);
    ADPD1080_WriteReg(0x74,0x0000);
    ADPD1080_WriteReg(0x75,0x0000);
    ADPD1080_WriteReg(0x76,0x0000);
    ADPD1080_WriteReg(0x77,0x0000);
    ADPD1080_WriteReg(0x78,0x0000);
    ADPD1080_WriteReg(0x79,0x0000);
    ADPD1080_WriteReg(0x7A,0x0000);
    ADPD1080_WriteReg(0x7B,0x0000);
    ADPD1080_WriteReg(0x7C,0x0000);
    ADPD1080_WriteReg(0x7D,0x0000);
    ADPD1080_WriteReg(0x7E,0x0000);
    ADPD1080_WriteReg(0x7F,0x0000);
}
