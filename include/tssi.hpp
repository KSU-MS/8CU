#pragma once

#include "Pin_Defs.hpp"

class tssi {
  bool has_fault = false;

public:
  void update_status(bool imd_is_chillin, bool bms_is_chillin);
  void update_lights();
};

void tssi::update_status(bool imd_is_chillin, bool bms_is_chillin) {
  if (imd_is_chillin && bms_is_chillin) {
    has_fault = false;
  } else {
    has_fault = true;
  }
}

#ifdef ARDUINO
#include <Arduino.h>
#include <Metro.h>

// 4hz as per rules section EV.5.11.5
Metro blink_4hz = Metro(200);
bool blink_is_high = false;

void tssi::update_lights() {
  if (has_fault) {
    digitalWrite(SSI_GREEN, LOW);

    if (blink_4hz.check() && !blink_is_high) {
      digitalWrite(SSI_RED, HIGH);
      blink_is_high = true;
    } else {
      blink_is_high = false;
      digitalWrite(SSI_RED, LOW);
    }
  } else {
    digitalWrite(SSI_GREEN, HIGH);
    digitalWrite(SSI_RED, LOW);
  }
}
#endif
