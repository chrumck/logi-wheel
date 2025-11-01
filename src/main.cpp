#include <Arduino.h>
#include <Wire.h>
#include <HX711.h>   

#define LOAD_CELL_DOUT_PIN 0
#define LOAD_CELL_SCK_PIN 1
#define LOAD_CELL_TARE_REPS 10
#define LOAD_CELL_SCALING 660000.0

#define DAC_ADDR 0x60   
const uint8_t DAC_CTRL_BYTE_WRITE = 0b01000000;

#define V_VCC 3.3
#define V_BRAKE_OFF 2.93
#define V_BRAKE_FULL 1.48

#define DAC_RESOLUTION 4096
const float DAC_STEP = V_VCC / DAC_RESOLUTION;
const float DAC_BRAKE_OFF = V_BRAKE_OFF / DAC_STEP;
const float DAC_BRAKE_FULL = V_BRAKE_FULL / DAC_STEP;
const float DAC_RANGE = DAC_BRAKE_OFF - DAC_BRAKE_FULL;

#define DAC_CORRECTION_CUTOFF 0.80
#define DAC_CORRECTION_FACTOR 0.5976
const float DAC_CORRECTION_OFFSET = DAC_CORRECTION_CUTOFF * (1 - DAC_CORRECTION_FACTOR);

HX711 lcReader;
float lcValue;
uint16_t dacValue; 

void setup()
{
  lcReader.begin(LOAD_CELL_DOUT_PIN, LOAD_CELL_SCK_PIN);
  lcReader.tare(LOAD_CELL_TARE_REPS);
  Wire.begin();
} 

void loop()
{
  lcValue = lcReader.get_value(1) / LOAD_CELL_SCALING;
  if(lcValue < 0){ lcValue = 0;      }
  if (lcValue > 1){ lcValue = 1; } 

  if (lcValue > DAC_CORRECTION_CUTOFF){ lcValue = lcValue *DAC_CORRECTION_FACTOR + DAC_CORRECTION_OFFSET; }

  dacValue = DAC_BRAKE_OFF - (DAC_RANGE *  lcValue);
  if (dacValue * DAC_STEP > V_BRAKE_OFF) { dacValue = V_BRAKE_OFF; }

  Wire.beginTransmission(DAC_ADDR);
  Wire.write(DAC_CTRL_BYTE_WRITE);
  Wire.write(dacValue >> 4);
  Wire.write(dacValue << 4);
  Wire.endTransmission();
} 