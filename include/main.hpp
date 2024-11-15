#pragma once // Make sure the file is only included once

#ifdef TEENSYDUINO
#include "board.h"
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

class debug {
  Adafruit_NeoPixel *pixels;

  void setup();
};

void debug::setup() {
  pixels =
      new Adafruit_NeoPixel(NUM_NEOPIXEL, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

  pixels->begin();
}
#endif
