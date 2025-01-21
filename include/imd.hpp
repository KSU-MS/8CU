#pragma once
#include "main.hpp"

uint32_t startMillis;
uint32_t widthHigh;
uint32_t width;

#ifdef TEENSYDUINO
void interrupt_handle() {
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

void init_imd() { attachInterrupt(IMD_SENSE, interrupt_handle, CHANGE); }
#endif
