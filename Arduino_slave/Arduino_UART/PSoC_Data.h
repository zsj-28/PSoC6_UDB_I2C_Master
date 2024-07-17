//Global variables for Command_Matrix
uint8_t ADCCH0_data[2];
uint8_t ADCCH0_stamp;
uint8_t ADCCH1_data[2];
uint8_t ADCCH1_stamp;
uint8_t ADCCH2_data[2];
uint8_t ADCCH2_stamp;
uint8_t ADCCH3_data[2];
uint8_t ADCCH3_stamp;
uint8_t ADCCH4_data[2];
uint8_t ADCCH4_stamp;
uint8_t ADCCH5_data[2];
uint8_t ADCCH5_stamp;
uint8_t ADCCH6_data[2];
uint8_t ADCCH6_stamp;
uint8_t ADCCH7_data[2];
uint8_t ADCCH7_stamp;
uint8_t ADCCH8_data[2];
uint8_t ADCCH8_stamp;
uint8_t ADCCH9_data[2];
uint8_t ADCCH9_stamp;
uint8_t sO2_data[4];
uint8_t sO2_stamp;
uint8_t sO2_avg_data[4];
uint8_t sO2_avg_stamp;
uint8_t HemoglobinA_data[4];
uint8_t HemoglobinA_stamp;
uint8_t HemoglobinB_data[4];
uint8_t HemoglobinB_stamp;
uint8_t UART_error[1];
uint8_t Reserved_data[2] = {0,0};
uint8_t Reserved_stamp = 0;
typedef struct command{   // Structure declaration
  uint8_t* Data;       // Member (char variable)
  uint8_t* Time_Stamp;
}command_struct;

command_struct Command_Matrix[256] = {
  {ADCCH0_data, &ADCCH0_stamp}, //0x00
  {ADCCH1_data, &ADCCH1_stamp}, //0x01
  {ADCCH2_data, &ADCCH2_stamp}, //0x02
  {ADCCH3_data, &ADCCH3_stamp}, //0x03
  {ADCCH4_data, &ADCCH4_stamp}, //0x04
  {ADCCH5_data, &ADCCH5_stamp}, //0x05
  {ADCCH6_data, &ADCCH6_stamp}, //0x06
  {ADCCH7_data, &ADCCH7_stamp}, //0x07
  {ADCCH8_data, &ADCCH8_stamp}, //0x08
  {ADCCH9_data, &ADCCH9_stamp}, //0x09
  {Reserved_data, &Reserved_stamp}, //0x0A
  {Reserved_data, &Reserved_stamp}, //0x0B
  {Reserved_data, &Reserved_stamp}, //0x0C
  {Reserved_data, &Reserved_stamp}, //0x0D
  {Reserved_data, &Reserved_stamp}, //0x0E
  {Reserved_data, &Reserved_stamp}, //0x0F  
  {sO2_data, &sO2_stamp}, //0x10
  {sO2_avg_data, &sO2_avg_stamp}, //0x11
  {HemoglobinA_data, &HemoglobinA_stamp}, //0x12
  {HemoglobinB_data, &HemoglobinA_stamp}, //0x13
  {Reserved_data, &Reserved_stamp}, //0x14
  {Reserved_data, &Reserved_stamp}, //0x15
  {Reserved_data, &Reserved_stamp}, //0x16
  {Reserved_data, &Reserved_stamp}, //0x17
  {Reserved_data, &Reserved_stamp}, //0x18
  {Reserved_data, &Reserved_stamp}, //0x19
  {Reserved_data, &Reserved_stamp}, //0x1A
  {Reserved_data, &Reserved_stamp}, //0x1B
  {Reserved_data, &Reserved_stamp}, //0x1C
  {Reserved_data, &Reserved_stamp}, //0x1D
  {Reserved_data, &Reserved_stamp}, //0x1E
  {Reserved_data, &Reserved_stamp}, //0x1F
  {Reserved_data, &Reserved_stamp}, //0x20
  {Reserved_data, &Reserved_stamp}, //0x21
  {Reserved_data, &Reserved_stamp}, //0x22
  {Reserved_data, &Reserved_stamp}, //0x23
  {Reserved_data, &Reserved_stamp}, //0x24
  {Reserved_data, &Reserved_stamp}, //0x25
  {Reserved_data, &Reserved_stamp}, //0x26
  {Reserved_data, &Reserved_stamp}, //0x27
  {Reserved_data, &Reserved_stamp}, //0x28
  {Reserved_data, &Reserved_stamp}, //0x29
  {Reserved_data, &Reserved_stamp}, //0x2A
  {Reserved_data, &Reserved_stamp}, //0x2B
  {Reserved_data, &Reserved_stamp}, //0x2C
  {Reserved_data, &Reserved_stamp}, //0x2D
  {Reserved_data, &Reserved_stamp}, //0x2E
  {Reserved_data, &Reserved_stamp}, //0x2F
  {Reserved_data, &Reserved_stamp}, //0x30
  {Reserved_data, &Reserved_stamp}, //0x31
  {Reserved_data, &Reserved_stamp}, //0x32
  {Reserved_data, &Reserved_stamp}, //0x33
  {Reserved_data, &Reserved_stamp}, //0x34
  {Reserved_data, &Reserved_stamp}, //0x35
  {Reserved_data, &Reserved_stamp}, //0x36
  {Reserved_data, &Reserved_stamp}, //0x37
  {Reserved_data, &Reserved_stamp}, //0x38
  {Reserved_data, &Reserved_stamp}, //0x39
  {Reserved_data, &Reserved_stamp}, //0x3A
  {Reserved_data, &Reserved_stamp}, //0x3B
  {Reserved_data, &Reserved_stamp}, //0x3C
  {Reserved_data, &Reserved_stamp}, //0x3D
  {Reserved_data, &Reserved_stamp}, //0x3E
  {Reserved_data, &Reserved_stamp}, //0x3F
  {Reserved_data, &Reserved_stamp}, //0x40
  {Reserved_data, &Reserved_stamp}, //0x41
  {Reserved_data, &Reserved_stamp}, //0x42
  {Reserved_data, &Reserved_stamp}, //0x43
  {Reserved_data, &Reserved_stamp}, //0x44
  {Reserved_data, &Reserved_stamp}, //0x45
  {Reserved_data, &Reserved_stamp}, //0x46
  {Reserved_data, &Reserved_stamp}, //0x47
  {Reserved_data, &Reserved_stamp}, //0x48
  {Reserved_data, &Reserved_stamp}, //0x49
  {Reserved_data, &Reserved_stamp}, //0x4A
  {Reserved_data, &Reserved_stamp}, //0x4B
  {Reserved_data, &Reserved_stamp}, //0x4C
  {Reserved_data, &Reserved_stamp}, //0x4D
  {Reserved_data, &Reserved_stamp}, //0x4E
  {Reserved_data, &Reserved_stamp}, //0x4F
  {Reserved_data, &Reserved_stamp}, //0x50
  {Reserved_data, &Reserved_stamp}, //0x51
  {Reserved_data, &Reserved_stamp}, //0x52
  {Reserved_data, &Reserved_stamp}, //0x53
  {Reserved_data, &Reserved_stamp}, //0x54
  {Reserved_data, &Reserved_stamp}, //0x55
  {Reserved_data, &Reserved_stamp}, //0x56
  {Reserved_data, &Reserved_stamp}, //0x57
  {Reserved_data, &Reserved_stamp}, //0x58
  {Reserved_data, &Reserved_stamp}, //0x59
  {Reserved_data, &Reserved_stamp}, //0x5A
  {Reserved_data, &Reserved_stamp}, //0x5B
  {Reserved_data, &Reserved_stamp}, //0x5C
  {Reserved_data, &Reserved_stamp}, //0x5D
  {Reserved_data, &Reserved_stamp}, //0x5E
  {Reserved_data, &Reserved_stamp}, //0x5F
  {Reserved_data, &Reserved_stamp}, //0x60
  {Reserved_data, &Reserved_stamp}, //0x61
  {Reserved_data, &Reserved_stamp}, //0x62
  {Reserved_data, &Reserved_stamp}, //0x63
  {Reserved_data, &Reserved_stamp}, //0x64
  {Reserved_data, &Reserved_stamp}, //0x65
  {Reserved_data, &Reserved_stamp}, //0x66
  {Reserved_data, &Reserved_stamp}, //0x67
  {Reserved_data, &Reserved_stamp}, //0x68
  {Reserved_data, &Reserved_stamp}, //0x69
  {Reserved_data, &Reserved_stamp}, //0x6A
  {Reserved_data, &Reserved_stamp}, //0x6B
  {Reserved_data, &Reserved_stamp}, //0x6C
  {Reserved_data, &Reserved_stamp}, //0x6D
  {Reserved_data, &Reserved_stamp}, //0x6E
  {Reserved_data, &Reserved_stamp}, //0x6F
  {Reserved_data, &Reserved_stamp}, //0x70
  {Reserved_data, &Reserved_stamp}, //0x71
  {Reserved_data, &Reserved_stamp}, //0x72
  {Reserved_data, &Reserved_stamp}, //0x73
  {Reserved_data, &Reserved_stamp}, //0x74
  {Reserved_data, &Reserved_stamp}, //0x75
  {Reserved_data, &Reserved_stamp}, //0x76
  {Reserved_data, &Reserved_stamp}, //0x77
  {Reserved_data, &Reserved_stamp}, //0x78
  {Reserved_data, &Reserved_stamp}, //0x79
  {Reserved_data, &Reserved_stamp}, //0x7A
  {Reserved_data, &Reserved_stamp}, //0x7B
  {Reserved_data, &Reserved_stamp}, //0x7C
  {Reserved_data, &Reserved_stamp}, //0x7D
  {Reserved_data, &Reserved_stamp}, //0x7E
  {Reserved_data, &Reserved_stamp}, //0x7F
  {Reserved_data, &Reserved_stamp}, //0x80
  {Reserved_data, &Reserved_stamp}, //0x81
  {Reserved_data, &Reserved_stamp}, //0x82
  {Reserved_data, &Reserved_stamp}, //0x83
  {Reserved_data, &Reserved_stamp}, //0x84
  {Reserved_data, &Reserved_stamp}, //0x85
  {Reserved_data, &Reserved_stamp}, //0x86
  {Reserved_data, &Reserved_stamp}, //0x87
  {Reserved_data, &Reserved_stamp}, //0x88
  {Reserved_data, &Reserved_stamp}, //0x89
  {Reserved_data, &Reserved_stamp}, //0x8A
  {Reserved_data, &Reserved_stamp}, //0x8B
  {Reserved_data, &Reserved_stamp}, //0x8C
  {Reserved_data, &Reserved_stamp}, //0x8D
  {Reserved_data, &Reserved_stamp}, //0x8E
  {Reserved_data, &Reserved_stamp}, //0x8F
  {Reserved_data, &Reserved_stamp}, //0x90
  {Reserved_data, &Reserved_stamp}, //0x91
  {Reserved_data, &Reserved_stamp}, //0x92
  {Reserved_data, &Reserved_stamp}, //0x93
  {Reserved_data, &Reserved_stamp}, //0x94
  {Reserved_data, &Reserved_stamp}, //0x95
  {Reserved_data, &Reserved_stamp}, //0x96
  {Reserved_data, &Reserved_stamp}, //0x97
  {Reserved_data, &Reserved_stamp}, //0x98
  {Reserved_data, &Reserved_stamp}, //0x99
  {Reserved_data, &Reserved_stamp}, //0x9A
  {Reserved_data, &Reserved_stamp}, //0x9B
  {Reserved_data, &Reserved_stamp}, //0x9C
  {Reserved_data, &Reserved_stamp}, //0x9D
  {Reserved_data, &Reserved_stamp}, //0x9E
  {Reserved_data, &Reserved_stamp}, //0x9F
  {Reserved_data, &Reserved_stamp}, //0xA0
  {Reserved_data, &Reserved_stamp}, //0xA1
  {Reserved_data, &Reserved_stamp}, //0xA2
  {Reserved_data, &Reserved_stamp}, //0xA3
  {Reserved_data, &Reserved_stamp}, //0xA4
  {Reserved_data, &Reserved_stamp}, //0xA5
  {Reserved_data, &Reserved_stamp}, //0xA6
  {Reserved_data, &Reserved_stamp}, //0xA7
  {Reserved_data, &Reserved_stamp}, //0xA8
  {Reserved_data, &Reserved_stamp}, //0xA9
  {Reserved_data, &Reserved_stamp}, //0xAA
  {Reserved_data, &Reserved_stamp}, //0xAB
  {Reserved_data, &Reserved_stamp}, //0xAC
  {Reserved_data, &Reserved_stamp}, //0xAD
  {Reserved_data, &Reserved_stamp}, //0xAE
  {Reserved_data, &Reserved_stamp}, //0xAF
  {Reserved_data, &Reserved_stamp}, //0xB0
  {Reserved_data, &Reserved_stamp}, //0xB1
  {Reserved_data, &Reserved_stamp}, //0xB2
  {Reserved_data, &Reserved_stamp}, //0xB3
  {Reserved_data, &Reserved_stamp}, //0xB4
  {Reserved_data, &Reserved_stamp}, //0xB5
  {Reserved_data, &Reserved_stamp}, //0xB6
  {Reserved_data, &Reserved_stamp}, //0xB7
  {Reserved_data, &Reserved_stamp}, //0xB8
  {Reserved_data, &Reserved_stamp}, //0xB9
  {Reserved_data, &Reserved_stamp}, //0xBA
  {Reserved_data, &Reserved_stamp}, //0xBB
  {Reserved_data, &Reserved_stamp}, //0xBC
  {Reserved_data, &Reserved_stamp}, //0xBD
  {Reserved_data, &Reserved_stamp}, //0xBE
  {Reserved_data, &Reserved_stamp}, //0xBF
  {Reserved_data, &Reserved_stamp}, //0xC0
  {Reserved_data, &Reserved_stamp}, //0xC1
  {Reserved_data, &Reserved_stamp}, //0xC2
  {Reserved_data, &Reserved_stamp}, //0xC3
  {Reserved_data, &Reserved_stamp}, //0xC4
  {Reserved_data, &Reserved_stamp}, //0xC5
  {Reserved_data, &Reserved_stamp}, //0xC6
  {Reserved_data, &Reserved_stamp}, //0xC7
  {Reserved_data, &Reserved_stamp}, //0xC8
  {Reserved_data, &Reserved_stamp}, //0xC9
  {Reserved_data, &Reserved_stamp}, //0xCA
  {Reserved_data, &Reserved_stamp}, //0xCB
  {Reserved_data, &Reserved_stamp}, //0xCC
  {Reserved_data, &Reserved_stamp}, //0xCD
  {Reserved_data, &Reserved_stamp}, //0xCE
  {Reserved_data, &Reserved_stamp}, //0xCF
  {Reserved_data, &Reserved_stamp}, //0xD0
  {Reserved_data, &Reserved_stamp}, //0xD1
  {Reserved_data, &Reserved_stamp}, //0xD2
  {Reserved_data, &Reserved_stamp}, //0xD3
  {Reserved_data, &Reserved_stamp}, //0xD4
  {Reserved_data, &Reserved_stamp}, //0xD5
  {Reserved_data, &Reserved_stamp}, //0xD6
  {Reserved_data, &Reserved_stamp}, //0xD7
  {Reserved_data, &Reserved_stamp}, //0xD8
  {Reserved_data, &Reserved_stamp}, //0xD9
  {Reserved_data, &Reserved_stamp}, //0xDA
  {Reserved_data, &Reserved_stamp}, //0xDB
  {Reserved_data, &Reserved_stamp}, //0xDC
  {Reserved_data, &Reserved_stamp}, //0xDD
  {Reserved_data, &Reserved_stamp}, //0xDE
  {Reserved_data, &Reserved_stamp}, //0xDF
  {Reserved_data, &Reserved_stamp}, //0xE0
  {Reserved_data, &Reserved_stamp}, //0xE1
  {Reserved_data, &Reserved_stamp}, //0xE2
  {Reserved_data, &Reserved_stamp}, //0xE3
  {Reserved_data, &Reserved_stamp}, //0xE4
  {Reserved_data, &Reserved_stamp}, //0xE5
  {Reserved_data, &Reserved_stamp}, //0xE6
  {Reserved_data, &Reserved_stamp}, //0xE7
  {Reserved_data, &Reserved_stamp}, //0xE8
  {Reserved_data, &Reserved_stamp}, //0xE9
  {Reserved_data, &Reserved_stamp}, //0xEA
  {Reserved_data, &Reserved_stamp}, //0xEB
  {Reserved_data, &Reserved_stamp}, //0xEC
  {Reserved_data, &Reserved_stamp}, //0xED
  {Reserved_data, &Reserved_stamp}, //0xEE
  {Reserved_data, &Reserved_stamp}, //0xEF
  {Reserved_data, &Reserved_stamp}, //0xF0
  {Reserved_data, &Reserved_stamp}, //0xF1
  {Reserved_data, &Reserved_stamp}, //0xF2
  {Reserved_data, &Reserved_stamp}, //0xF3
  {Reserved_data, &Reserved_stamp}, //0xF4
  {Reserved_data, &Reserved_stamp}, //0xF5
  {Reserved_data, &Reserved_stamp}, //0xF6
  {Reserved_data, &Reserved_stamp}, //0xF7
  {Reserved_data, &Reserved_stamp}, //0xF8
  {Reserved_data, &Reserved_stamp}, //0xF9
  {Reserved_data, &Reserved_stamp}, //0xFA
  {Reserved_data, &Reserved_stamp}, //0xFB
  {Reserved_data, &Reserved_stamp}, //0xFC
  {Reserved_data, &Reserved_stamp}, //0xFD
  {UART_error, &Reserved_stamp}, //0xFE
  {Reserved_data, &Reserved_stamp}  //0xFF

};