#include "main.hpp"

uint32_t startMillis;
uint32_t widthHigh;
uint32_t width;

void interruptHandle() {
  int state = digitalReadFast(IMD_SENSE);

  if (state == HIGH) {
    // Start of pulse
    width = micros() - startMillis;

    startMillis = micros();
  } else {
    // End of pulse
    widthHigh = micros() - startMillis;
  }
}

void init_imd() { attachInterrupt(IMD_SENSE, interruptHandle, CHANGE); }
