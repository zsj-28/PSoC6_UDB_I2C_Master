#ifndef BQ34Z100_H
#define BQ34Z100_H

#include <stdint.h>
#include <math.h>

/**
 * Definitions for the I2C address and other constants
 */
#define BQ34Z100 0xAA // or 0xAB or 0x55

#define DESIGNCAP 1400
#define DESIGNENERGY 5180
#define CELLCOUNT 0x04
#define LEDCONFIG 0x4b
#define VOLTSEL 1
#define ZEROCHARGEVOLT 3375
#define FLASH_UPDATE_OK_VOLT 2800
#define QMAX0 1400

#define VOLTAGEGAIN 17818

#define LOADSELECT 0x01
#define LOADMODE 0x00

#define RESETVOLTAGE 22200
#define SENSE_RES 15.0f
#define USE_EXTERNAL_THERMISTOR 0

/**
 * Command definitions according to the BQ34Z100 datasheet.
 */
typedef enum {
    COMMAND_CONTROL = 0x00,
    COMMAND_STATEOFCHARGE = 0x02,
    COMMAND_MAXERROR = 0x03,
    COMMAND_REMAININGCAPACITY = 0x04,
    COMMAND_FULLCHARGECAPACITY = 0x06,
    COMMAND_VOLTAGE = 0x08,
    COMMAND_AVERAGECURRENT = 0x0A,
    COMMAND_TEMPERATURE = 0x0C,
    COMMAND_FLAGS = 0x0E,
    COMMAND_CURRENT = 0x10,
    COMMAND_FLAGSB = 0x12,
    COMMAND_SERIALNUMBER = 0x28,
    COMMAND_INTERNALTEMPERATURE = 0x2A,
    COMMAND_CYCLECOUNT = 0x2C,
    COMMAND_STATEOFHEALTH = 0x2E,
    COMMAND_DATAFLASHCLASS = 0x3E,
    COMMAND_DATAFLASHBLOCK = 0x3F,
    COMMAND_BLOCKDATA = 0x40,
    COMMAND_BLOCKDATACHECKSUM = 0x60,
    COMMAND_BLOCKDATACONTROL = 0x61
} Command_t;

typedef enum {
    CONTROL_CONTROL_STATUS = 0x0000,
    CONTROL_DEVICE_TYPE = 0x0001,
    CONTROL_FW_VERSION = 0x0002,
    CONTROL_HW_VERSION = 0x0003,
    CONTROL_RESET_DATA = 0x0005,
    CONTROL_PREV_MACWRITE = 0x0007,
    CONTROL_CHEM_ID = 0x0008,
    CONTROL_BOARD_OFFSET = 0x0009,
    CONTROL_CC_OFFSET = 0x000A,
    CONTROL_CC_OFFSET_SAVE = 0x000B,
    CONTROL_DF_VERSION = 0x000C,
    CONTROL_SET_FULLSLEEP = 0x0010,
    CONTROL_STATIC_CHEM_CHECKSUM = 0x0017,
    CONTROL_SEALED = 0x0020,
    CONTROL_IT_ENABLE = 0x0021,
    CONTROL_CAL_ENABLE = 0x002D,
    CONTROL_RESET = 0x0041,
    CONTROL_EXIT_CAL = 0x0080,
    CONTROL_ENTER_CAL = 0x0081,
    CONTROL_OFFSET_CAL = 0x0082,
    CONTROL_UNSEAL_KEY1 = 0x0414,
    CONTROL_UNSEAL_KEY2 = 0x3672
} Control_t;

/**
 * Struct for handling BQ34Z100 device operations.
 */
typedef struct {
    uint8_t flashbytes[32];
    uint8_t currFlashPage;
    uint8_t currFlashBlockIndex;
} BQ34Z100_t;

typedef struct {
    uint16_t flagA;
    uint16_t flagB;
} BQ34Z100_Flags_t;

/**
 * Function prototypes for interfacing with the BQ34Z100.
 */
void sendControlCommand(BQ34Z100_t *device, Control_t control);
uint16_t readControlCommand(BQ34Z100_t *device, Control_t control);
void write(BQ34Z100_t *device, Command_t command, uint8_t cmd);
void write_two_bytes(BQ34Z100_t *device, Command_t command, uint8_t cmd1, uint8_t cmd2);
uint32_t read(BQ34Z100_t *device, Command_t command, uint8_t length);
uint8_t calcChecksum(uint8_t *flashbytes);

void BQ34Z100_Init(BQ34Z100_t *device, uint32_t i2c_bus, uint32_t frequency);
uint16_t BQ34Z100_GetStatus(BQ34Z100_t *device);
void BQ34Z100_EnableCal(BQ34Z100_t *device);
void BQ34Z100_EnterCal(BQ34Z100_t *device);
void BQ34Z100_ExitCal(BQ34Z100_t *device);
void BQ34Z100_ITEnable(BQ34Z100_t *device);
uint8_t BQ34Z100_GetSOC(BQ34Z100_t *device);
uint16_t BQ34Z100_GetError(BQ34Z100_t *device);
uint16_t BQ34Z100_GetRemaining(BQ34Z100_t *device);
uint16_t BQ34Z100_GetVoltage(BQ34Z100_t *device);
int16_t BQ34Z100_GetCurrent(BQ34Z100_t *device);
double BQ34Z100_GetTemperature(BQ34Z100_t *device);
uint16_t BQ34Z100_GetChemID(BQ34Z100_t *device);
uint16_t BQ34Z100_GetStateOfHealth(BQ34Z100_t *device);
int BQ34Z100_GetSerial(BQ34Z100_t *device);
void BQ34Z100_Reset(BQ34Z100_t *device);
void BQ34Z100_Unseal(BQ34Z100_t *device);
void BQ34Z100_Seal(BQ34Z100_t *device);
void BQ34Z100_ChangePage(BQ34Z100_t *device, char subclass, uint16_t offset);
void BQ34Z100_UpdateChecksum(BQ34Z100_t *device);
void BQ34Z100_ReadFlash(BQ34Z100_t *device);
void BQ34Z100_WriteFlash(BQ34Z100_t *device, uint8_t index, uint32_t value, int len);
uint8_t* BQ34Z100_GetFlashBytes(BQ34Z100_t *device);
void BQ34Z100_ChangePage48(BQ34Z100_t *device);
void BQ34Z100_ChangePage64(BQ34Z100_t *device);
void BQ34Z100_ChangePage80(BQ34Z100_t *device);
void BQ34Z100_ChangePage82(BQ34Z100_t *device);
uint16_t BQ34Z100_CalibrateVoltage(BQ34Z100_t *device, uint16_t currentVoltage);
void BQ34Z100_ResetVoltageDivider(BQ34Z100_t *device);
void BQ34Z100_CalibrateShunt(BQ34Z100_t *device, int16_t calCurrent);
void BQ34Z100_SetSenseResistor(BQ34Z100_t *device);
uint16_t BQ34Z100_ReadDeviceType(BQ34Z100_t *device);
uint16_t BQ34Z100_ReadFWVersion(BQ34Z100_t *device);
uint16_t BQ34Z100_ReadHWVersion(BQ34Z100_t *device);
uint32_t floatToXemics(float value);
float xemicsToFloat(uint32_t xemics);
uint8_t BQ34Z100_GetUpdateStatus(BQ34Z100_t *device);
BQ34Z100_Flags_t BQ34Z100_GetFlags(BQ34Z100_t *device);

#endif // BQ34Z100_H
