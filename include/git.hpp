#pragma once

// These values are provided by the python script ran by the lib_dep
// https://github.com/KSU-MS/pio-git-hash-gen
#ifndef AUTO_VERSION
#warning "AUTO_VERSION was not defined by the generator!"
#define AUTO_VERSION 0xdeadbeef
#endif

#ifndef FW_PROJECT_IS_DIRTY
#warning "FW_PROJECT_IS_DIRTY was not defined by the generator!"
#define FW_PROJECT_IS_DIRTY 1
#endif

#ifndef FW_PROJECT_IS_MAIN_OR_MASTER
#warning "FW_PROJECT_IS_MAIN_OR_MASTER was not defined by the generator!"
#define FW_PROJECT_IS_MAIN_OR_MASTER 0
#endif

byte board_status[8];

void set_time() {
  uint16_t fella = uint16_t((millis() / 1000));

  board_status[2] = fella & 0xFF;        // Store the low byte
  board_status[3] = (fella >> 8) & 0xFF; // Store the high byte

  board_status[4] = AUTO_VERSION & 0xFF;         // LSB (Least Significant Byte)
  board_status[5] = (AUTO_VERSION >> 8) & 0xFF;  // Next byte
  board_status[6] = (AUTO_VERSION >> 16) & 0xFF; // Next byte
  board_status[7] = (AUTO_VERSION >> 24) & 0xFF; // MSB (Most Significant Byte)
}

void set_temp(uint8_t value) {
  // Ensure the value is within the 7-bit range (0-127)
  if (value <= 127) {
    board_status[0] = ((value & 0x7F) << 1) | FW_PROJECT_IS_DIRTY;
  } else {
#ifdef ARDUINO
    Serial.println("Value out of range for 7 bits (0-127).");
#endif
  }
}

void set_humidity(uint8_t value) {
  // Ensure the value is within the 7-bit range (0-127)
  if (value <= 127) {
    board_status[1] = ((value & 0x7F) << 1) | FW_PROJECT_IS_MAIN_OR_MASTER;
  } else {
#ifdef ARDUINO
    Serial.println("Value out of range for 7 bits (0-127).");
#endif
  }
}
