#pragma once

#ifdef TEENSYDUINO
#include "8CU.hpp"
#include <Arduino.h>
#include <SPI.h>

enum adc_method {
  avr,
  mcp,
};

// Helper functions for the pointer
uint16_t avr_helper(uint8_t pin) { return (uint16_t)analogRead(pin); }

uint16_t mcp_helper(uint8_t pin) {
  // Gain control of the SPI port
  // and configure settings
  SPI.beginTransaction(SPISettings(DEFAULT_SPI_SPEED, MSBFIRST, SPI_MODE0));

  // Take the SS pin low to select the chip:
  digitalWrite(ADC_CS, LOW);

  // Set up channel
  byte b = B01100000;
  b |= ((pin << 2));

  // Send in the channel via SPI:
  SPI.transfer(b);

  // Read data from SPI
  byte result1 = SPI.transfer(0);
  byte result2 = SPI.transfer(0);

  // Take the SS pin high to de-select the chip:
  digitalWrite(ADC_CS, HIGH);

  // Release control of the SPI port
  SPI.endTransaction();

  return (result1 << 4) | (result2 >> 4);
}

class adc {
private:
  uint8_t pin;
  uint16_t (*reader)(uint8_t);
  uint16_t rawval;
  double alpha;
  bool is_filtered;

public:
  union {
    uint16_t in;
    byte b[2];
  } value;

  void setup(adc_method guy, uint8_t target_pin) {
    pin = target_pin;

    switch (guy) {
    case avr:
      reader = &avr_helper;
      break;

    case mcp:
      pinMode(ADC_CS, OUTPUT);
      pinMode(ADC_CS, HIGH);

      // Initialize SPI:
      SPI.begin();

      reader = &mcp_helper;
      break;
    }
  }

  void update() {
    // Vars to hold some data
    if (is_filtered) {
      rawval = reader(pin);
      value.in = alpha * value.in + (1 - alpha) * rawval;
    } else {
      value.in = reader(pin);
    }
  }
};
#endif
