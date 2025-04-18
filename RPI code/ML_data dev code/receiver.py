"""
Biorobotics Lab Project 4.2 Fall 2024
@file: raspberry_pi_uart.py
@brief: Raspberry Pi UART receiver demo code for communicating with PSoC and uploading data to AWS S3.

@author: Zhaonan Shi <zhaonans>
@author: Haoran Zheng <hzheng5>
@author: Haoen Li <haoenl>
@author: Ching-Han Chou <chingha2>
@author: Steven Zhang <sijinz>
@author: Thomas Li <tyli>
@author: Marina Li <muyaol>

"""

import os
import struct
import time
from datetime import datetime

import pandas as pd

import boto3  # For AWS S3 upload
import serial  # For serial communication
from botocore.exceptions import NoCredentialsError
from Crypto.Cipher import AES  # For AES decryption

# Constants and Configurations
LOG_DIRECTORY = "/home/pas/Desktop/ecmo/log"
UART_PORT = '/dev/ttyAMA0'
UART_BAUDRATE = 115200
AES_KEY = bytes([
    0xAA, 0xBB, 0xCC, 0xDD,
    0xEE, 0xFF, 0xFF, 0xEE,
    0xDD, 0xCC, 0xBB, 0xAA,
    0xAA, 0xBB, 0xCC, 0xDD
])
S3_BUCKET = 'bio-data-fa24'  # Replace with your actual bucket name

# Initialize AWS S3 client
s3 = boto3.client('s3')

# Initialize AES cipher
cipher = AES.new(AES_KEY, AES.MODE_ECB)

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

# Global variables
opCode = None
dataLength = 0
data = bytearray()
buffer_index = 0
UART_buffer = bytearray()
receivedCRC = 0
calculatedCRC = 0
UART_timeout = 0
current_filename = None
log_file = None


def bytes2Float(bytes_array):
    """
    Convert a sequence of 4 bytes to a float using little-endian format.

    Parameters:
        bytes_array (bytes): A sequence of 4 bytes representing a float.

    Returns:
        float: The converted floating-point number.
    """
    return struct.unpack('<f', bytes_array)[0]


def calculateCRC8(opCode, dataLength, data):
    """
    Calculate the CRC8 for an incoming UART packet.

    Parameters:
        opCode (int): The operation code of the packet.
        dataLength (int): The length of the data payload.
        data (bytes): The data payload.

    Returns:
        int: The calculated CRC8 value.
    """
    crc = 0x00  # Initialize CRC value

    # Update CRC with the opcode
    crc = crcTable[crc ^ opCode]

    # Update CRC with the data length
    crc = crcTable[crc ^ dataLength]

    # Update CRC with each data byte
    for i in range(dataLength):
        crc = crcTable[crc ^ data[i]]

    return crc


def generate_filename():
    """
    Generate a filename based on the current date and hour.

    Returns:
        str: The full path to the log file.
    """
    # Ensure the directory exists; create it if it doesn't
    if not os.path.exists(LOG_DIRECTORY):
        os.makedirs(LOG_DIRECTORY)
        print(f"Created directory: {LOG_DIRECTORY}")

    current_time = time.strftime("%Y-%m-%d_%H")  # e.g., 2024-10-31_16
    filename = f"psoc_data_log_{current_time}.txt"
    return os.path.join(LOG_DIRECTORY, filename)


def generate_s3_key(file_name):
    """
    Generate the S3 key for uploading the log file.

    Parameters:
        file_name (str): The local filename.

    Returns:
        str: The S3 key (path in the S3 bucket).
    """
    current_time = time.strftime("%Y-%m-%d_%H-%M-%S")
    s3_key = f"uploads/{current_time}_{os.path.basename(file_name)}"
    return s3_key


def upload_to_s3(file_name, bucket):
    """
    Upload a file to an AWS S3 bucket.

    Parameters:
        file_name (str): The local path to the file.
        bucket (str): The name of the S3 bucket.

    Returns:
        None
    """
    s3_key = generate_s3_key(file_name)
    try:
        s3.upload_file(file_name, bucket, s3_key)
        print(f"File {file_name} uploaded to S3 bucket {bucket} at {s3_key}")
    except FileNotFoundError:
        print(f"File {file_name} was not found.")
    except NoCredentialsError:
        print("AWS credentials not available.")
    except Exception as e:
        print(f"Error uploading file to S3: {e}")

# Function to parse lines with both timestamp and ADC values
def parse_adc_line(line):
    timestamp_part, adc_part = line.split(': ', 1)
    values = ('ADC' + adc_part.split('ADC', 1)[1]).strip().split(', ')
    adc_values = []
    for v in values:
        if 'ADC' in v:
            adc_values.append(float(v.split(': ')[1].strip(',')))
    return [timestamp_part + ':'] + adc_values 

def EMAprocess(filename):
    with open(filename, 'r') as f:
        adc_data = [parse_adc_line(line) for line in f if all(adc in line for adc in ['ADC 0', 'ADC 1', 'ADC 2', 'ADC 3'])]

    first_timestamp_part = adc_data[0][0].split(':')[0].replace(' ', '_').replace(':', '')

    df = pd.DataFrame(adc_data, columns=['Timestamp', 'ADC 0', 'ADC 1', 'ADC 2', 'ADC 3'])
    span = 10

    df['EMA_ADC 0'] = df['ADC 0'].ewm(span=span, adjust=False).mean()
    df['EMA_ADC 1'] = df['ADC 1'].ewm(span=span, adjust=False).mean()
    df['EMA_ADC 2'] = df['ADC 2'].ewm(span=span, adjust=False).mean()
    df['EMA_ADC 3'] = df['ADC 3'].ewm(span=span, adjust=False).mean()
    
    EMA_DIRECTORY = "/home/pas/Desktop/ecmo/EMA"
    output_filename = f'{EMA_DIRECTORY}/EMA_data_{first_timestamp_part}.txt'
    with open(output_filename, 'w') as f:
        for index, row in df.iterrows():
            f.write(f"{row['Timestamp']} ")
            # f.write(f"ADC 0: {row['ADC 0']:.6f}, ADC 1: {row['ADC 1']:.6f}, ")
            # f.write(f"ADC 2: {row['ADC 2']:.6f}, ADC 3: {row['ADC 3']:.6f}, ")
            f.write(f"EMA_ADC 0: {row['EMA_ADC 0']:.6f}, EMA_ADC 1: {row['EMA_ADC 1']:.6f}, ")
            f.write(f"EMA_ADC 2: {row['EMA_ADC 2']:.6f}, EMA_ADC 3: {row['EMA_ADC 3']:.6f},\n")


def log_message(message):
    """
    Write the message to the log file, handling log rotation and S3 upload.

    Parameters:
        message (str): The message to log.

    Returns:
        None
    """
    global current_filename, log_file

    # Generate the new filename based on the current hour
    new_filename = generate_filename()
    if new_filename != current_filename:
        if log_file:
            log_file.close()  # Close the previous file
            upload_to_s3(current_filename, S3_BUCKET)  # Upload to S3
            EMAprocess(current_filename)
        current_filename = new_filename
        try:
            log_file = open(current_filename, 'a')  # Open a new file
            print(f"Switched logging to new file: {current_filename}")
        except Exception as e:
            print(f"Error opening log file: {e}")
            exit(1)  # Exit if the log file cannot be opened

    try:
        log_file.write(message + "\n")  # Write message to file
        log_file.flush()  # Ensure it's written immediately
    except Exception as e:
        print(f"Error writing to log file: {e}")


def process_data():
    """
    Process the received data packet.

    Returns:
        None
    """
    global UART_buffer
    opCode = UART_buffer[0]
    dataLength = UART_buffer[1]
    data = UART_buffer[2:2 + dataLength]
    receivedCRC = UART_buffer[2 + dataLength]

    # Calculate CRC8
    calculatedCRC = calculateCRC8(opCode, dataLength, data)

    # Decrypt the received packet
    decrypted_packet = bytearray()
    for i in range(0, dataLength, 16):
        block = data[i:i + 16]
        if len(block) < 16:
            # Padding if necessary
            block += bytes(16 - len(block))
        decrypted_block = cipher.decrypt(bytes(block))
        decrypted_packet.extend(decrypted_block[:len(block)])

    # Check if the received CRC matches the calculated one
    if receivedCRC == calculatedCRC:
        data_element_index = 0
        log_entry = ""
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        for i in range(0, dataLength, 4):
            float_bytes = decrypted_packet[i:i + 4]
            value = bytes2Float(float_bytes)
            if data_element_index == 0:
                log_entry += f"{timestamp}: "
                log_entry += "ADC 0: " if i + 16 >= dataLength else "L680: "
            elif data_element_index == 1:
                log_entry += "ADC 1: " if i + 16 >= dataLength else "L850: "
            elif data_element_index == 2:
                log_entry += "ADC 2: " if i + 16 >= dataLength else "SO2_avg: "
            elif data_element_index == 3:
                log_entry += "ADC 3: " if i + 16 >= dataLength else "HBT_avg: "
            log_entry += f"{value:.6f}"
            data_element_index = (data_element_index + 1) % 4
            if data_element_index == 0:
                log_message(log_entry)
                log_entry = ""
            else:
                log_entry += ", "
    else:
        print("CRC check failed.")


def UART_receive(ser):
    """
    Receive incoming UART data and process it.

    Parameters:
        ser (serial.Serial): The serial port object.

    Returns:
        None
    """
    global buffer_index, UART_buffer, UART_timeout
    # Check for UART timeout
    if UART_timeout >= 100:
        buffer_index = 0
        UART_buffer = bytearray()
    bytes_available = ser.in_waiting
    if bytes_available > 0:
        UART_timeout = 0
        # Read available bytes
        read_data = ser.read(bytes_available)
        UART_buffer.extend(read_data)
        buffer_index += bytes_available
        # Check if we have enough data to process
        if buffer_index > 1 and buffer_index >= UART_buffer[1] + 3:
            process_data()
            buffer_index = 0
            UART_buffer = bytearray()


def setup_serial():
    """
    Initialize UART serial port.

    Returns:
        serial.Serial: The initialized serial port object.
    """
    try:
        ser = serial.Serial(UART_PORT, UART_BAUDRATE, timeout=0)
        print("Serial port opened successfully.")
        return ser
    except Exception as e:
        print(f"Error opening serial port: {e}")
        exit(1)  # Exit if the serial port cannot be opened


def main():
    """
    Main function to receive UART data and log it.

    Returns:
        None
    """
    global UART_timeout, log_file, current_filename
    ser = setup_serial()
    current_filename = generate_filename()
    try:
        log_file = open(current_filename, 'a')
        print(f"Logging to file: {current_filename}")
    except Exception as e:
        print(f"Error opening log file: {e}")
        exit(1)  # Exit if the log file cannot be opened

    last_time = time.time()

    try:
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

    except KeyboardInterrupt:
        # Stop the script gracefully with Ctrl+C
        print("Logging stopped.")

    finally:
        # Close the serial port and the log file when done
        ser.close()
        print("Serial port closed.")
        if log_file:
            log_file.close()
            print("Log file closed.")
            # Upload the last log file to S3
            upload_to_s3(current_filename, S3_BUCKET)


if __name__ == '__main__':
    main()
