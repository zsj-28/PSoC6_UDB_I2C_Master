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

typedef struct command{   // Structure declaration
  uint8_t* Data;       // Member (char variable)
  uint8_t* Time_Stamp;
}command_struct;

command_struct Command_Matrix[256] = {
  {ADCCH0_data, &ADCCH0_stamp},
  {ADCCH1_data, &ADCCH1_stamp},
  {ADCCH2_data, &ADCCH2_stamp},
  {ADCCH3_data, &ADCCH3_stamp},
  {ADCCH4_data, &ADCCH4_stamp},
  {ADCCH5_data, &ADCCH5_stamp},
  {ADCCH6_data, &ADCCH6_stamp},
  {ADCCH7_data, &ADCCH7_stamp},
  {ADCCH8_data, &ADCCH8_stamp},
  

};