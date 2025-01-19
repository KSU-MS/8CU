#include "main.hpp"
#include "asyncPulse.hpp"

void setup() {
  Serial.begin(9600);

  adc imd_relay;
  adc bms_relay;
  adc imd_gpio;
  adc bms_gpio;

  imd_relay.setup(avr, IMD_RELAY);
  bms_relay.setup(avr, BMS_RELAY);
  imd_gpio.setup(avr, IMD_GPIO);
  bms_gpio.setup(avr, BMS_GPIO);

  debug pixels;
  pixels.setup();

  adc i12v;
  adc iSDC;
  adc v12v;
  adc v5v;
  adc v3v;
  adc vSDC;
  adc humidity;
  adc temp;

  i12v.setup(mcp, ISENSE_12V);
  iSDC.setup(mcp, SDC_ISENSE);
  v12v.setup(mcp, SENSE_12V);
  v5v.setup(mcp, SENSE_5V);
  v3v.setup(mcp, SENSE_3V);
  vSDC.setup(mcp, SDC_SENSE);
  humidity.setup(mcp, HUMIDITY);
  temp.setup(mcp, TEMP);

  pinMode(IMD_SENSE, INPUT_PULLUP);
  init_imd();
}

void loop() {}
