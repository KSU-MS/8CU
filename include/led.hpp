#pragma once
#include "Pin_Defs.hpp"

#ifdef ARDUINO
#include <Adafruit_NeoPixel.h>

class debug {
  Adafruit_NeoPixel *pixels;

public:
  void setup();
};

void debug::setup() {
  pixels =
      new Adafruit_NeoPixel(NUM_NEOPIXEL, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

  pixels->begin();
}
#endif
