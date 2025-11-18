#include <Arduino.h>
#include <Wire.h>
#include <HX711.h>

#define LOAD_CELL_DOUT_PIN PD3
#define LOAD_CELL_SCK_PIN PD2
#define LOAD_CELL_TARE_REPS 10
#define LOAD_CELL_SCALING 1.00e-6

#define DAC_ADDR 0x60
const uint8_t DAC_CTRL_BYTE_WRITE = 0b01000000;

#define V_VCC 3.26
#define V_BRAKE_OFF 2.93
#define V_BRAKE_FULL 1.10

#define DAC_RESOLUTION 4096
#define DAC_STEP (V_VCC / DAC_RESOLUTION)
const float DAC_BRAKE_OFF = V_BRAKE_OFF / DAC_STEP;
const float DAC_BRAKE_FULL = V_BRAKE_FULL / DAC_STEP;
const float DAC_RANGE = DAC_BRAKE_OFF - DAC_BRAKE_FULL;

HX711 lcReader;
float lcValue;
uint16_t dacValue;

void setup()
{
  Serial.begin(115200);
  Serial.println("Logitech Wheel Brake Controller Starting...");

  lcReader.begin(LOAD_CELL_DOUT_PIN, LOAD_CELL_SCK_PIN);
  Serial.println("LC reader initialized...");
  
  lcReader.tare(LOAD_CELL_TARE_REPS);
  Serial.println("LC zeroed...");

  Wire.begin();
}

void loop()
{
  lcValue = lcReader.get_value(2) * LOAD_CELL_SCALING;

  if (lcValue < 0) { lcValue = 0; }
  if (lcValue > 1) { lcValue = 1; }

  // Serial.print(">lcValue:");
  // Serial.print(millis());
  // Serial.print(":");
  // Serial.println(lcValue, 3);

  dacValue = DAC_BRAKE_OFF - DAC_RANGE * lcValue;

  Wire.beginTransmission(DAC_ADDR);
  Wire.write(DAC_CTRL_BYTE_WRITE);
  Wire.write(dacValue >> 4);
  Wire.write(dacValue << 4);
  Wire.endTransmission();
} 