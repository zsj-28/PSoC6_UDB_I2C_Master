/**
 * @file: Arduino_UART.ino
 * @brief: ESP32 UART receiver demo code for communicating with PSoC
 * 
 * @author: Ching-Han Chou <chingha2>
 * @author: Steven Zhang <sijinz>
 * @author: Thomas Li <tyli>
 *
*/

#include "PSoC_Data.h"

// CRC-8 calculation table
void onSysTikTimer(void);
const uint8_t crcTable[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

void setup() {
    // Initialize the built-in Serial port for communication with the Serial Monitor
    Serial.begin(115200);

    // Initialize the UART port (Serial1) with the desired baud rate
    Serial1.begin(115200, SERIAL_8N1, 16, 17); // Adjust RX and TX pin numbers if necessary

    // Timer 0, 1ms timer
    hw_timer_t *systiktimer = timerBegin(1000000); // was Timer 1, prescaler 80 (1MHz clock), now 1000 Hz
    timerAttachInterrupt(systiktimer, &onSysTikTimer);
    timerAlarm(systiktimer, 1000ULL, true, 0ULL); 
    timerStart(systiktimer); // Enable the timer interrupt
}

/// UART glocal variables
uint8_t opCode;
uint8_t dataLength = 0;
uint8_t *data;
uint8_t buffer_index = 0;
uint8_t UART_buffer[200];
uint8_t receivedCRC = 0;
uint8_t calculatedCRC = 0;
uint8_t UART_timeout = 0;


void UART_receive() {
    //This reset mechanism prevents incorrect datastream format from 
    //disrupting future datastream
    //If UART does not receive data within 10ms, reset buffer index 
    if (UART_timeout >= 10) {
        buffer_index = 0;
    }
    while (Serial1.available() > 0) {
        UART_timeout = 0;
        UART_buffer[buffer_index] = Serial1.read();
        // Serial.printf("incoming byte: 0x%x\r\n", UART_buffer[buffer_index]);
        buffer_index++;
        //Reset buffer index if buffer is full 
        //If data stream exceeds buffer size, data will not be parsed
        if (buffer_index >= 200) {
            buffer_index = 0;
        }

        // The following conditions check that all the data bytes are received and
        // that buffer index count has exceeded the "length" byte, which is the second byte in the data stream
        // UART_buffer[1] + 3 means dataLength bytes + opcode byte + length byte + crc byte
        if ((buffer_index > 1) && (buffer_index == UART_buffer[1] + 3)) {
            // Parse opcode, length, CRC
            opCode = UART_buffer[0];
            dataLength = UART_buffer[1];
            data = &UART_buffer[2];
            receivedCRC = UART_buffer[dataLength + 2];
            
            // Calculate CRC8
            calculatedCRC = calculateCRC8(opCode, dataLength, data);
            buffer_index = 0;

            Serial.printf("opCode: %d\r\n", opCode);
            Serial.printf("dataLength: %d\r\n", dataLength);

            Serial.print("Received CRC: 0x");
            Serial.println(receivedCRC, HEX);
            Serial.print("Calculated CRC: 0x");
            Serial.println(calculatedCRC, HEX);

            // Check if the received CRC mathces the calculated one
            if (receivedCRC == calculatedCRC) {
                Serial.println("CRC check passed.");
                for (uint8_t i = 0; i < dataLength; i++) {
                    Serial.print("0x");
                    if (UART_buffer[i + 2] < 0x10) {
                        Serial.print("0");
                    }
                    Serial.print(UART_buffer[i + 2], HEX);
                    Serial.print(" ");
                }
                switch (opCode) {
                    case 0x00:
                        Command_Matrix[0].Data[0] = UART_buffer[2]; //ADC CH0
                        Command_Matrix[0].Data[1] = UART_buffer[3];
                        *(Command_Matrix[0].Time_Stamp) = UART_buffer[12];
                        Command_Matrix[1].Data[0] = UART_buffer[4]; //ADC CH1
                        Command_Matrix[1].Data[1] = UART_buffer[5];
                        *(Command_Matrix[1].Time_Stamp) = UART_buffer[12];
                        Command_Matrix[2].Data[0] = UART_buffer[6]; //ADC CH2
                        Command_Matrix[2].Data[1] = UART_buffer[7];
                        *(Command_Matrix[2].Time_Stamp) = UART_buffer[12];
                        Command_Matrix[3].Data[0] = UART_buffer[8]; //ADC CH3
                        Command_Matrix[3].Data[1] = UART_buffer[9];
                        *(Command_Matrix[3].Time_Stamp) = UART_buffer[12];
                        Command_Matrix[4].Data[0] = UART_buffer[10]; //ADC CH4
                        Command_Matrix[4].Data[1] = UART_buffer[11];
                        *(Command_Matrix[4].Time_Stamp) = UART_buffer[12];
                    break;
                    case 0x01:
                        Command_Matrix[5].Data[0] = UART_buffer[2]; //ADC CH5
                        Command_Matrix[5].Data[1] = UART_buffer[3];
                        *(Command_Matrix[5].Time_Stamp) = UART_buffer[12];
                        Command_Matrix[6].Data[0] = UART_buffer[4]; //ADC CH6
                        Command_Matrix[6].Data[1] = UART_buffer[5];
                        *(Command_Matrix[6].Time_Stamp) = UART_buffer[12];
                        Command_Matrix[7].Data[0] = UART_buffer[6]; //ADC CH7
                        Command_Matrix[7].Data[1] = UART_buffer[7];
                        *(Command_Matrix[7].Time_Stamp) = UART_buffer[12];
                        Command_Matrix[8].Data[0] = UART_buffer[8]; //ADC CH8
                        Command_Matrix[8].Data[1] = UART_buffer[9];
                        *(Command_Matrix[8].Time_Stamp) = UART_buffer[12];
                        Command_Matrix[9].Data[0] = UART_buffer[10]; //ADC CH9
                        Command_Matrix[9].Data[1] = UART_buffer[11];
                        *(Command_Matrix[9].Time_Stamp) = UART_buffer[12];
                    break;
                    case 0x03:
                        Command_Matrix[0x10].Data[0] = UART_buffer[2]; //s02
                        Command_Matrix[0x10].Data[1] = UART_buffer[3];
                        Command_Matrix[0x10].Data[2] = UART_buffer[4]; 
                        Command_Matrix[0x10].Data[3] = UART_buffer[5];
                        *(Command_Matrix[0x10].Time_Stamp) = UART_buffer[18];
                        Command_Matrix[0x11].Data[0] = UART_buffer[6]; //s02_avg
                        Command_Matrix[0x11].Data[1] = UART_buffer[7];
                        Command_Matrix[0x11].Data[2] = UART_buffer[8]; 
                        Command_Matrix[0x11].Data[3] = UART_buffer[9];
                        *(Command_Matrix[0x11].Time_Stamp) = UART_buffer[18];
                        Command_Matrix[0x12].Data[0] = UART_buffer[10]; //Hemoglobin A
                        Command_Matrix[0x12].Data[1] = UART_buffer[11];
                        Command_Matrix[0x12].Data[2] = UART_buffer[12]; 
                        Command_Matrix[0x12].Data[3] = UART_buffer[13];
                        *(Command_Matrix[0x12].Time_Stamp) = UART_buffer[18];
                        Command_Matrix[0x13].Data[0] = UART_buffer[14]; //Hemoglobin B
                        Command_Matrix[0x13].Data[1] = UART_buffer[15];
                        Command_Matrix[0x13].Data[2] = UART_buffer[16]; 
                        Command_Matrix[0x13].Data[3] = UART_buffer[17];
                        *(Command_Matrix[0x13].Time_Stamp) = UART_buffer[18];
                    break;
                    default:
                    break;
                }
            }
            else {
                Serial.println("CRC check failed.");
            }
        }
    }

}

void loop() {
    //This part sends dummy data and is used for testing, delete if not needed
    /*
    uint8_t crc = 0;
    uint8_t op_code = 0x00;
    const uint8_t length = 0x0B;
    uint8_t data[length] = {0,1,2,3,4,5,6,7,8,9,10};
    Serial1.write(op_code);
    Serial1.write(length); //LENGTH
    for(int i = 0; i < length; i++){
        Serial1.write(data[i]);
    }
    crc = calculateCRC8(op_code, length, data);
    Serial1.write(crc);
    delay(110);
    */

    UART_receive();
    // Add a small delay to avoid overwhelming the Serial Monitor
    delay(100);
}

uint8_t calculateCRC8(uint8_t opCode, uint8_t dataLength, uint8_t* data) {
    uint8_t crc = 0x00; // Initialize CRC value

    // Update CRC with the opcode
    crc = crcTable[crc ^ opCode];

    // Update CRC with the data length
    crc = crcTable[crc ^ dataLength];

    // Update CRC with each data byte
    for (uint8_t i = 0; i < dataLength; i++) {
        crc = crcTable[crc ^ data[i]];
    }

    return crc;
}

//1ms timer interrupt
void onSysTikTimer() {
    UART_timeout++;
    if (UART_timeout > 100) {
        UART_timeout = 100; // 10ms UART timeout
        //Set UART error timeout bit
        Command_Matrix[0xFE].Data[0] |= 0x01;
    }
    else {
        //Clear UART error timeout bit
        Command_Matrix[0xFE].Data[0] &= 0xFE;
    }
}