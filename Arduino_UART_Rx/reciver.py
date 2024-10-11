"""
Biorobotics Lab Project 4.2 Summer 2024
@file: raspberry_pi_uart.py
@brief: Raspberry Pi UART receiver demo code for communicating with PSoC

@author: Zhaonan Shi <zhaonans>
@author: Haoran Zheng <hzheng5>
@author: Haoen Li <haoenl>
@author: Ching-Han Chou <chingha2>
@author: Steven Zhang <sijinz>
@author: Thomas Li <tyli>
"""

import serial
import time
import struct
from Cryptodome.Cipher import AES

# UART Rx variables
opCode = None
dataLength = 0
data = bytearray()
buffer_index = 0
UART_buffer = bytearray()
receivedCRC = 0
calculatedCRC = 0
UART_timeout = 0

# AES key (must be 16 bytes for AES-128)
aes_key = bytes([0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xFF, 0xEE,
                 0xDD, 0xCC, 0xBB, 0xAA, 0xAA, 0xBB, 0xCC, 0xDD])
cipher = AES.new(aes_key, AES.MODE_ECB)

# CRC-8 calculation table
crcTable = [
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31,
    0x24, 0x23, 0x2A, 0x2D, 0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
    0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D, 0xE0, 0xE7, 0xEE, 0xE9,
    0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1,
    0xB4, 0xB3, 0xBA, 0xBD, 0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
    0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA, 0xB7, 0xB0, 0xB9, 0xBE,
    0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16,
    0x03, 0x04, 0x0D, 0x0A, 0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
    0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A, 0x89, 0x8E, 0x87, 0x80,
    0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8,
    0xDD, 0xDA, 0xD3, 0xD4, 0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
    0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44, 0x19, 0x1E, 0x17, 0x10,
    0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F,
    0x6A, 0x6D, 0x64, 0x63, 0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
    0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13, 0xAE, 0xA9, 0xA0, 0xA7,
    0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF,
    0xFA, 0xFD, 0xF4, 0xF3
]

# Function definitions
def bytes2Float(bytes_array):
    """Convert 4 bytes to a float."""
    return struct.unpack('<f', bytes_array)[0]

def calculateCRC8(opCode, dataLength, data):
    """Calculate the CRC8 for an incoming UART packet."""
    crc = 0x00  # Initialize CRC value

    # Update CRC with the opcode
    crc = crcTable[crc ^ opCode]

    # Update CRC with the data length
    crc = crcTable[crc ^ dataLength]

    # Update CRC with each data byte
    for i in range(dataLength):
        crc = crcTable[crc ^ data[i]]

    return crc

def UART_receive(ser):
    """Receive incoming UART data and print to console."""
    global buffer_index, UART_buffer, UART_timeout
    # Check for UART timeout
    if UART_timeout >= 100:
        buffer_index = 0
        UART_buffer = bytearray()

    bytesAvail = ser.in_waiting
    if bytesAvail > 0:
        UART_timeout = 0
        # Read available bytes
        read_data = ser.read(bytesAvail)
        UART_buffer.extend(read_data)
        buffer_index += bytesAvail

        # Check if we have enough data to process
        if buffer_index > 1 and buffer_index >= UART_buffer[1] + 3:
            process_data()
            buffer_index = 0
            UART_buffer = bytearray()

def process_data():
    """Process the received data packet."""
    global UART_buffer
    opCode = UART_buffer[0]
    dataLength = UART_buffer[1]
    data = UART_buffer[2:2+dataLength]
    receivedCRC = UART_buffer[2+dataLength]

    # Calculate CRC8
    calculatedCRC = calculateCRC8(opCode, dataLength, data)

    # Decrypt the received packet
    decrypted_packet = bytearray()
    for i in range(0, dataLength, 16):
        block = data[i:i+16]
        if len(block) < 16:
            # Padding if necessary
            block += bytes(16 - len(block))
        decrypted_block = cipher.decrypt(bytes(block))
        decrypted_packet.extend(decrypted_block[:len(block)])

    # Check if the received CRC matches the calculated one
    if receivedCRC == calculatedCRC:
        dataElem = 0
        for i in range(0, dataLength, 4):
            float_bytes = decrypted_packet[i:i+4]
            value = bytes2Float(float_bytes)
            if dataElem == 0:
                if i + 16 >= dataLength:
                    print("\n   ADC CH1: ", end='')
                else:
                    print("Slot A avg: ", end='')
            elif dataElem == 1:
                if i + 16 >= dataLength:
                    print("   ADC CH2: ", end='')
                else:
                    print("Slot B avg: ", end='')
            elif dataElem == 2:
                if i + 16 >= dataLength:
                    print("ADC CH3: ", end='')
                else:
                    print("SO2_avg: ", end='')
            elif dataElem == 3:
                if i + 16 >= dataLength:
                    print("ADC CH4: ", end='')
                else:
                    print("HBT_avg: ", end='')
            print(f"{value:15f}", end='')
            dataElem = (dataElem + 1) % 4
            if dataElem == 0:
                print()
            else:
                print(",\t", end='')
        print()
    else:
        print("CRC check failed.")

# Initialize UART serial port
def setup_serial():
    # Adjust the serial port and baud rate as per your setup
    ser = serial.Serial('/dev/serial0', 115200, timeout=0)
    return ser

def main():
    global UART_timeout
    ser = setup_serial()
    last_time = time.time()
    while True:
        UART_receive(ser)

        # Simulate the 1ms timer interrupt
        current_time = time.time()
        if current_time - last_time >= 0.001:  # 1ms
            last_time = current_time
            UART_timeout += 1
            if UART_timeout > 100:
                UART_timeout = 100  # Limit the timeout

        time.sleep(0.0001)  # Small delay to prevent CPU overload

if __name__ == '__main__':
    main()
