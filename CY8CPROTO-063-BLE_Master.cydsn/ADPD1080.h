#ifndef _ADPD1080_H
#define _ADPD1080_H

#include "project.h"  // Include PSoC project header for PSoC-specific functions
#include <stdint.h>
#include <stdbool.h>

/* Registers for ADPD1080 */
#define ADPD1080_STATUS             0x00
#define ADPD1080_INT_MASK           0x01
#define ADPD1080_GPIO_DRV           0x02
#define ADPD1080_FIFO_THRESH        0x06
#define ADPD1080_DEVID              0x08
#define ADPD1080_CHIP_ID            0x16  // Default Device ID value (LSB of DEVID register)
#define ADPD1080_I2CS_ID            0x09
#define ADPD1080_CLK_RATIO          0x0A
#define ADPD1080_GPIO_CTRL          0x0B
#define ADPD1080_SLAVE_ADDRESS_KEY  0x0D
#define ADPD1080_SW_RESET           0x0F
#define ADPD1080_MODE               0x10
#define ADPD1080_SLOT_EN            0x11
#define ADPD1080_FSAMPLE            0x12
#define ADPD1080_PD_LED_SELECT      0x14
#define ADPD1080_NUM_AVG            0x15
#define ADPD1080_SLOTA_CH1_OFFSET   0x18
#define ADPD1080_SLOTA_CH2_OFFSET   0x19
#define ADPD1080_SLOTA_CH3_OFFSET   0x1A
#define ADPD1080_SLOTA_CH4_OFFSET   0x1B
#define ADPD1080_SLOTB_CH1_OFFSET   0x1E
#define ADPD1080_SLOTB_CH2_OFFSET   0x1F
#define ADPD1080_SLOTB_CH3_OFFSET   0x20
#define ADPD1080_SLOTB_CH4_OFFSET   0x21
#define ADPD1080_ILED3_COARSE       0x22
#define ADPD1080_ILED1_COARSE       0x23
#define ADPD1080_ILED2_COARSE       0x24
#define ADPD1080_ILED_FINE          0x25
#define ADPD1080_SLOTA_LED_PULSE    0x30
#define ADPD1080_SLOTA_NUMPULSES    0x31
#define ADPD1080_LED_DISABLE        0x34
#define ADPD1080_SLOTB_LED_PULSE    0x35
#define ADPD1080_SLOTB_NUMPULSES    0x36
#define ADPD1080_ALT_PWR_DN         0x37
#define ADPD1080_EXT_SYNC_STARTUP   0x38
#define ADPD1080_SLOTA_AFE_WINDOW   0x39
#define ADPD1080_SLOTB_AFE_WINDOW   0x3B
#define ADPD1080_AFE_PWR_CFG1       0x3C
#define ADPD1080_SLOTA_TIA_CFG      0x42
#define ADPD1080_SLOTA_AFE_CFG      0x43
#define ADPD1080_SLOTB_TIA_CFG      0x44
#define ADPD1080_SLOTB_AFE_CFG      0x45
#define ADPD1080_SAMPLE_CLK         0x4B
#define ADPD1080_CLK32M_ADJUST      0x4D
#define ADPD1080_ADC_CLOCK          0x4E
#define ADPD1080_EXT_SYNC_SEL       0x4F
#define ADPD1080_CLK32M_CAL_EN      0x50
#define ADPD1080_AFE_PWR_CFG2       0x54
#define ADPD1080_TIA_INDEP_GAIN     0x55
#define ADPD1080_DIGITAL_INT_EN     0x58
#define ADPD1080_DIG_INT_CFG        0x5A
#define ADPD1080_DATA_ACCESS_CTL    0x5F
#define ADPD1080_FIFO_ACCESS        0x60
#define ADPD1080_SLOTA_PD1_16BIT    0x64
#define ADPD1080_SLOTA_PD2_16BIT    0x65
#define ADPD1080_SLOTA_PD3_16BIT    0x66
#define ADPD1080_SLOTA_PD4_16BIT    0x67
#define ADPD1080_SLOTB_PD1_16BIT    0x68
#define ADPD1080_SLOTB_PD2_16BIT    0x69
#define ADPD1080_SLOTB_PD3_16BIT    0x6A
#define ADPD1080_SLOTB_PD4_16BIT    0x6B
#define ADPD1080_A_PD1_LOW          0x70
#define ADPD1080_A_PD2_LOW          0x71
#define ADPD1080_A_PD3_LOW          0x72
#define ADPD1080_A_PD4_LOW          0x73
#define ADPD1080_A_PD1_HIGH         0x74
#define ADPD1080_A_PD2_HIGH         0x75
#define ADPD1080_A_PD3_HIGH         0x76
#define ADPD1080_A_PD4_HIGH         0x77
#define ADPD1080_B_PD1_LOW          0x78
#define ADPD1080_B_PD2_LOW          0x79
#define ADPD1080_B_PD3_LOW          0x7A
#define ADPD1080_B_PD4_LOW          0x7B
#define ADPD1080_B_PD1_HIGH         0x7C
#define ADPD1080_B_PD2_HIGH         0x7D
#define ADPD1080_B_PD3_HIGH         0x7E

/* Device address */
#define ADPD1080_ADDRESS 0x64

/* Sensitivity */
#define ADPD1080_SENSITIVITY 1.64f

/* Enumeration and structure definitions */
typedef enum {
  STANDBY = 0,
  PROGRAM,
  NORMAL_OPERATION
} ADPD1080_OperationMode;

typedef enum {
  ALL_INPUTS_FLOATING = 0,
  ALL_PD_CONNECTED,
  PD_5_8_CONNECTED = 0x05,
  PD_1_4_CONNECTED = 0x05
} ADPD1080_TimeSlotPD;

typedef enum {
  SLOTA = 0,
  SLOTB
} ADPD1080_TimeSlot;

typedef enum {
  LEDX1 = 0x02,
  LEDX2 = 0x01,
  LEDX3
} ADPD1080_LED;

typedef enum {
  TIA_200 = 0,
  TIA_100,
  TIA_50,
  TIA_25
} ADPD1080_TIAGain;

typedef enum {
  AVERAGE1 = 0,
  AVERAGE2,
  AVERAGE4,
  AVERAGE8,
  AVERAGE16,
  AVERAGE32,
  AVERAGE64,
  AVERAGE128
} ADPD1080_AverageN;

typedef enum {
  ADC_CLOCK_100MHz = 0x40,
  ADC_CLOCK_500KHz = 0x60
} ADPD1080_ADCClockSpeed;

struct ADPD1080_ChannelOffset {
  uint16_t CH1Offset;
  uint16_t CH2Offset;
  uint16_t CH3Offset;
  uint16_t CH4Offset;
};

/* Function prototypes */
bool ADPD1080_Init(void);
bool ADPD1080_WriteReg(uint8_t u8Address, uint16_t u16Value);
uint16_t ADPD1080_ReadReg(uint8_t u8Address);
void ADPD1080_SetTimeSlotSwitch(uint8_t slotA, uint8_t slotB);
void ADPD1080_Set32KCLK(int set);
void ADPD1080_SetFIFO(void);
bool ADPD1080_SelectLED(ADPD1080_LED enLEDNumber, ADPD1080_TimeSlot enSlot);
bool ADPD1080_DeselectLEDs(void);
void ADPD1080_ReadDataRegs(volatile uint16_t *data_Slot_A, volatile uint16_t *data_Slot_B, uint8_t count);
bool ADPD1080_SetLEDWidthOffset(ADPD1080_TimeSlot enSlot, uint8_t u8Width, uint8_t u8Offset);
bool ADPD1080_SetAFEWidthOffset(ADPD1080_TimeSlot enSlot, uint8_t u8Width, uint8_t u8Offset, uint8_t u8FineOffset);
bool ADPD1080_SetTIAGain(ADPD1080_TimeSlot enSlot, ADPD1080_TIAGain enTIAGain);
bool ADPD1080_SetSamplingFrequency(uint16_t u16Frequency);
bool ADPD1080_SetAverageFactor(ADPD1080_AverageN enAverage);
bool ADPD1080_SetADCClock(ADPD1080_ADCClockSpeed enADCClock);
bool ADPD1080_SetDigitalClock(void);
bool ADPD1080_Reset(void);
void ADPD1080_SetOffset(ADPD1080_TimeSlot enSlot, struct ADPD1080_ChannelOffset stOffset);
bool ADPD1080_DisableLed(ADPD1080_TimeSlot enSlot);
bool ADPD1080_EnableLed(ADPD1080_TimeSlot enSlot);
bool ADPD1080_SetPulseNumberPeriod(ADPD1080_TimeSlot enSlot, uint8_t u8PulseCount, uint8_t u8PulsePeriod);
void turbidity_init(void);
void ADPD1080_ReadData(volatile uint16_t *data_Slot_A, volatile uint16_t *data_Slot_B, uint8_t count);
void ADPD1080_SetOperationMode(uint8_t mode);
void turbidity_ChannelOffsetCalibration(void);
#endif /* _ADPD1080_H */
