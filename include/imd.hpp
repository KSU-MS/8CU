#pragma once
#include <stdint.h>

uint32_t startMicros = 1;
uint32_t widthHigh = 1;
uint32_t width = 1;

#ifdef TEENSYDUINO
#include <Arduino.h>

void interrupt_handle() {
  int state = digitalReadFast(IMD_SENSE);

  if (state == 1) {
    // End of last pulse, total width of high and low
    width = micros() - startMicros;

    // Start of new pulse
    startMicros = micros();
  } else {
    // End of the High side of the new pulse
    widthHigh = micros() - startMicros;
  }
}

void init_imd() {
  pinMode(IMD_SENSE, INPUT_PULLUP);
  attachInterrupt(IMD_SENSE, interrupt_handle, CHANGE);
}
#endif

// Convert the micro seconds to seconds then take inverse for frequncey
uint8_t get_imd_hz() { return uint8_t(1 / (width * 1000000)); }

// duty cycle is just a percentage of how long the signal is high vs low
uint8_t get_imd_duty() { return uint8_t((widthHigh / width) * 100); }
