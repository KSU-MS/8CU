#pragma once

#include "Pin_Defs.hpp"

class tssi {
  bool has_fault = false;
  bool relays_chill = true;
  bool last_imd_state = false;
  bool last_bms_state = false;

public:
  void update_status(bool imd_is_chillin, bool bms_is_chillin,
                     bool imd_relay_closed, bool bms_relay_closed) {
    if ((!imd_relay_closed && (last_imd_state == true)) ||
        (!bms_relay_closed && (last_bms_state == true))) {
      relays_chill = false;
    } else if ((imd_relay_closed && (last_imd_state == false)) ||
               (bms_relay_closed && (last_bms_state == false))) {
      relays_chill = true;
    }

    if (imd_is_chillin && bms_is_chillin && relays_chill) {
      has_fault = false;
    } else {
      has_fault = true;
    }

    last_imd_state = imd_relay_closed;
    last_bms_state = bms_relay_closed;
  }

  void update_lights();
};

#ifdef ARDUINO
#include <Arduino.h>
#include <Metro.h>

// 4hz as per rules section EV.5.11.5
Metro blink_4hz = Metro(200);
bool blink_is_high = false;

void tssi::update_lights() {
  if (has_fault) {
    digitalWrite(SSI_GREEN, LOW);

    if (blink_4hz.check()) {
      digitalToggle(SSI_RED);
      blink_is_high = false;
    }
  } else {
    digitalWrite(SSI_GREEN, HIGH);
    digitalWrite(SSI_RED, LOW);
  }
}
#endif
