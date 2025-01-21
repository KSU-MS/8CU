#include "main.hpp"

// Critical GPIO
adc imd_relay;
adc bms_relay;
adc imd_gpio;
adc bms_gpio;

// Non-critical GPIO
adc i12v;
adc iSDC;
adc v12v;
adc v5v;
adc v3v;
adc vSDC;
adc humidity;
adc temp;

debug pixels;

Metro send_status_1hz = Metro(1000);

void setup() {
  Serial.begin(9600);

  imd_relay.setup(avr, IMD_RELAY);
  bms_relay.setup(avr, BMS_RELAY);
  imd_gpio.setup(avr, IMD_GPIO);
  bms_gpio.setup(avr, BMS_GPIO);

  i12v.setup(mcp, ISENSE_12V);
  iSDC.setup(mcp, SDC_ISENSE);
  v12v.setup(mcp, SENSE_12V);
  v5v.setup(mcp, SENSE_5V);
  v3v.setup(mcp, SENSE_3V);
  vSDC.setup(mcp, SDC_SENSE);
  humidity.setup(mcp, HUMIDITY);
  temp.setup(mcp, TEMP);

  pixels.setup();

  pinMode(IMD_SENSE, INPUT_PULLUP);
  init_imd();

  MDB_CAN.begin();
  MDB_CAN.setBaudRate(500000);
  ACC_CAN.begin();
  ACC_CAN.setBaudRate(500000);
}

void loop() {
  if (MDB_CAN.read(msg)) {
  }
}
