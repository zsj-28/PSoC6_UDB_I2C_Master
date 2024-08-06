# Final Report
## Demo setup
- [Demo branch](https://github.com/zsj-28/PSoC6_UDB_I2C_Master/tree/demo)
- [Demo video](https://drive.google.com/file/d/1Bv1FpYkNOPGebD9OkA_9PS9wtUVst6f3/view?usp=drive_link)
- [Osama’s optical sensor](https://drive.google.com/file/d/17TWZSqO9HJqURGxlbdL6PXHE-bGl5WQ6/view?usp=drive_link)
- [cdmrpsensor](https://drive.google.com/drive/folders/141UBDyeI10y1gl2nfjNEcas7z4vmrYz4?usp=drive_link) (open with Altium online viewer)
  - Connector clip
    - Red - power (3.3V, ideally 5V)
    - Black - ground
    - Green -  SDA (1.8V, 3.6V maximum)
    - Yellow - SCL (1.8V, 3.6V maximum)
    - Blue - GPIO0 (1.8V, 2.2V maximum), unused
    - Purple - GPIO1 (1.8V, 2.2V maximum), unused
- [PSoC 6 evaluation board](https://www.infineon.com/cms/en/product/evaluation-boards/cy8cproto-063-ble/)
  - I2C_v3_50 (I2C) - UDB component
    - I2C bus lines must be pulled up to 3.3V externally, or the PSoC 6 will not properly read incoming I2C data, adpd1080 drives 1.8V. We are using 4.7k pull-ups.
    - Use 16 MHz external clock for 1000 kbps bus rate, 100 kbps is too slow
  - SCB_UART_PDL_v2_0 (UART, UART_1)
    - UART used for printing debug statements, UART_1 for packet Tx, sharing UART components will result in data corruption. 115200 bps.
  - TCPWM_Counter_PDL_v1_0 (Timer)
    - 1000 kHz clock with 10000u period, trigger overflow interrupt with 100 Hz frequency
  - Scan_ADC_v3_10 (ADC)
    - Single-shot, 4 single-ended channels enabled for 0V to 3.3V range, 16 samples averaged. All 4 channels connected to waveform generator
- [ESP32 receiver](https://www.amazon.com/HiLetgo-ESP-WROOM-32-Development-Microcontroller-Integrated/dp/B0718T232Z)
  - Common ground with PSoC
  - Rx port (pin 16, GPIO16, RX2) connected with Tx port of PSoC
  - Arduino IDE board selection: Node32s, other settings default except tab width 4 spaces

## Demo code
- [Style updates branch](https://github.com/zsj-28/PSoC6_UDB_I2C_Master/tree/thomas_setup)
- [Flowchart](https://miro.com/app/board/uXjVK4z4ljY=/?share_link_id=96529304312) [1]
- Macros:
  - ADC_NUM_CHANNELS 4U
    - Number of channels enabled in the on-chip SAR ADC. Up to 16 channels possible. Determines size of ADCData[]
  - ADC_SAMPLES_PER_PACKET 1U
    - Number of ADC samples sent per data packet to the ESP32. Determines size of ADCData[]
  - ADC_SAMPLE_RATE_DIV 10U
    - Number of timer interrupts to fire until the ADC is sampled. Determines ADC sample rate
  - ADPD_NUM_CHANNELS 1U
    - Number of channels enabled on the adpd1080 chip, per time slot. Up to 4 channels possible. Determines size of adpdDataA[] and adpdDataB[]
  - ADPD_SAMPLES_PER_PACKET 10U
    - Number of adpd1080 samples sent per data packet to the ESP32. Determines size of adpdDataA[] and adpdDataB[]
  - SMOOTHED_SAMPLE_SIZE 200U
    - Number of adpd1080 samples in application’s moving average sliding window
  - MAX_PACKET_SIZE 255U
    - Maximum number of bytes sent in a data packet payload to the ESP32. Limited by the size of the dataLength field in the UART frame format (currently 1 byte)
- Global variables:
  - au16DataSlotA[], au16DataSlotB[] (volatile uint16_t)
    - Holds most recent data read from the adpd1080’s data registers
  - dataReady (volatile bool)
    - Set by Timer_Int_Handler when data is ready, cleared by main
  - ADCData[] (volatile int16_t)
    - Shared buffer holding ADC conversion results for main to read
  - adpdDataA[], adpdDataB[] (volatile uint16_t)
    - Shared buffer holding adpd1080 data for main to read
  - timerCount (volatile uint16_t)
    - Records number of times Timer_Int_Handler has run
  - txBuffer[] (uint8_t)
    - Holds outgoing packet until transmission is completed, must remain allocated and unmodified until then
  - packet[] (uint8_t)
    - Holds byte-aligned data read by main from shared buffers after clearing dataReady 
  - encrypted_pkt[] (uint8_t)
    - Holds byte-aligned encrypted version of packet[]

## Application next steps (highest to lowest priority)
Track PSoC resources: Create a spreadsheet containing current PSoC resource usage and overall plan for resource allocation. According to Resource Meter 1 UDB I2C component uses 51% of the available UDB, which could be a limitation. Could also be paired with a plan for pin allocation.

Error handling: Currently adpd1080 initialization failure results in infinite loop, encryption failure prints an error message, ADC conversion failure prints an error message and returns (potentially resulting in stale data being transmitted), and adpd1080 read failure substitutes data with 0s. Device driver I2C read error could potentially cause the Timer_Int_Handler to block. Errors should be handled consistently and in a way that meets safety requirements. A spreadsheet of possible errors/faults and how they are handled could be created.

Implement true random Crypto key generation on initialization: Supported by PSoC Crypto hardware block. Current code uses a fixed key for AES encryption which may not be secure enough. Newly generated key could be communicated with the receiver via RSA-encrypted UART every time the PSoC resets. Receiver would need to implement an RSA public/private key. [2]

Watchdog timer for safety-critical application: [PSoC 6 Peripheral Driver Library: WDT (Watchdog Timer)](https://infineon.github.io/psoc6pdl/pdl_api_reference_manual/html/group__group__wdt.html)

Scaling to additional sensors: Adding more interrupt service routines with shared or independent hardware timers (up to 32) and adding code to the data processing stage of main allows scaling of the firmware to additional sensors. Examples of possible ISR-to-sensor allocation strategies include an ISR for each connected sensor, a shared ISR for each group of sensors with the same sampling rate, one ISR containing different sections corresponding to different sensors, or some combination of these strategies. If code complexity reaches a certain point, porting the firmware to FreeRTOS could also be done.

State machine: Could implement a state machine for fault-handling mode, firmware update mode, different operation modes corresponding to detected events in the patient-ECMO system. Depending on the state, can enable/disable interrupts, dynamically update sensor sample rates, and run different code in main, achieving the different configurations necessary for each state.

Eliminate race conditions: Currently race conditions may exist due to shared data between Timer_Int_Handler and main, but in practice the program remains deterministic because timing constraints are always met such that Timer_Int_Handler never preempts main, and data would only be lost if multiple consecutive preemptions occurred. Could disable interrupts during accesses of shared data as a rudimentary synchronization method just in case main takes longer than expected to complete its task (e.g. if the Crypto block becomes unavailable and encryption takes much longer than expected), but then the problem of postponing Timer_Int_Handler needs to be addressed.

Implement DFU module/bootloader to support firmware updates in the field: [PSoC™ 6 MCU Device Firmware Update (DFU) software development kit guide](https://www.infineon.com/dgdl/Infineon-AN213924_PSoC_6_MCU_Device_Firmware_Update_Software_Development_Kit_Guide-ApplicationNotes-v09_00-EN.pdf?fileId=8ac78c8c7cdc391c017d0d3171906557)

PSoC-layer data storage using SMIF: [PSoC 6 Peripheral Driver Library: SMIF (Serial Memory Interface)](https://infineon.github.io/psoc6pdl/pdl_api_reference_manual/html/group__group__smif.html)

PSoC-level user interface: If a rudimentary UI is implemented, users could potentially interface directly with the PSoC using a Bluetooth connection where their laptop is the master and the PSoC is the slave. This could be useful for adjusting the PSoC operation mode without going through the ESP32.

## adpd1080 device driver next steps
Error checking: Currently device driver errors are unchecked, checking them consistently allows the main application to handle errors better.

Data read interrupts: GPIO0 and GPIO1 on the adpd1080 can be used to trigger interrupts in the PSoC when data is available to be read. This eliminates the possibility of reading the same data twice (currently the configured sensor sample rate is equivalent to the data polling rate). The hardware design will have to include level shifters to use these GPIO pins because they have a maximum voltage range of 2.2V. This interrupt could replace Timer_Int_Handler as it is expected to run at approximately the configured adpd1080 sampling rate anyways.

Multiword read: Device driver currently only implements single word read I2C operations. If reading from the adpd1080’s FIFO is done in the future, multiword read cuts down on I2C overhead and decreases read latency when consecutive reads are made (see adpd1080 datasheet).

Support for multiple adpd1080 devices on the same I2C bus: Currently unimplemented in the device driver. Also requires use of GPIO0 and GPIO1 on the adpd1080.

## Other next steps
Clean up Git repository: add README, add gitignore file, merge branches into main, delete unused branches

## References
[1] Miro Samek, [Programming embedded systems: Foreground-background architecture (“superloop”)](https://www.embedded.com/programming-embedded-systems-foreground-background-architecture-superloop/)

[2] Harigovind A and Rakshith M B, [Securing UART communication interface in embedded IoT devices - EDN](https://www.edn.com/securing-uart-communication-interface-in-embedded-iot-devices/)

## APIs
[PDL API Reference - PSoC 6 Peripheral Driver Library](https://infineon.github.io/psoc6pdl/pdl_api_reference_manual/html/index.html)

[Arduino Cryptography Library: AESCommon Class Reference](https://rweather.github.io/arduinolibs/classAESCommon.html#a95a806adf42f975765ff62907efdc639)

Arduino HardwareSerial Library: https://github.com/espressif/arduino-esp32/tree/master

