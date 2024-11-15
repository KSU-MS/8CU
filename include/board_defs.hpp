#pragma once
#include "main.hpp"
#include <Adafruit_NeoPixel.h>
#include <SPI.h>

//
////
////// CONTROLLER PINS N SHIT
#define CAN_CTX 0
#define CAN_CRX 1
#define NEOPIXEL_PIN 2
#define SSI_GREEN 3
#define SSI_RED 4
#define BMS_CS 9
#define ADC_CS 10
#define IMD_SENSE 17
#define IMD_RELAY 18
#define BMS_RELAY 19
#define IMD_GPIO 20
#define BMS_GPIO 21
#define ISO_CAN_CTX 22
#define ISO_CAN_CRX 23

#define DEFAULT_SPI_SPEED 1000000

//
////
////// BOARD THINGS
#define NUM_NEOPIXEL 4

class debug {
  Adafruit_NeoPixel *pixels;

  void setup();
};

void debug::setup() {
  pixels =
      new Adafruit_NeoPixel(NUM_NEOPIXEL, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

  pixels->begin();
}

class adc {
public:
  void setup();
  uint16_t read(int channel);

private:
  int ADC_SPI_CS;
  unsigned int SPI_SPEED;
};

void adc::setup() {
  ADC_SPI_CS = ADC_CS;
  SPI_SPEED = DEFAULT_SPI_SPEED;

  pinMode(ADC_SPI_CS, OUTPUT);
  pinMode(ADC_SPI_CS, HIGH);

  // Initialize SPI:
  SPI.begin();
}

uint16_t adc::read(int channel) {
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
