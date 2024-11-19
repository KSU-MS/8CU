#pragma once

#ifdef TEENSYDUINO
#include "board_defs.hpp"
#include <Arduino.h>
#include <SPI.h>

class adc {
private:
  int ADC_SPI_CS;
  unsigned int SPI_SPEED;

public:
  void adc::setup() {
    ADC_SPI_CS = ADC_CS;
    SPI_SPEED = DEFAULT_SPI_SPEED;

    pinMode(ADC_SPI_CS, OUTPUT);
    pinMode(ADC_SPI_CS, HIGH);

    // Initialize SPI:
    SPI.begin();
  }

  uint16_t read(int channel) {
    // Gain control of the SPI port
    // and configure settings
    SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));

    // Take the SS pin low to select the chip:
    digitalWrite(ADC_SPI_CS, LOW);

    // Set up channel
    byte b = B01100000;
    b |= ((channel << 2));

    // Send in the channel via SPI:
    SPI.transfer(b);

    // Read data from SPI
    byte result1 = SPI.transfer(0);
    byte result2 = SPI.transfer(0);

    // Take the SS pin high to de-select the chip:
    digitalWrite(ADC_SPI_CS, HIGH);

    // Release control of the SPI port
    SPI.endTransaction();

    return (result1 << 4) | (result2 >> 4);
  }
};
#endif
