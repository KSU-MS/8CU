#pragma once
#include "ksu_can.h"

#ifdef TEENSYDUINO
#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> iso_can;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can;

template <typename FlexCAN_T4> class canMan {
  FlexCAN_T4 *controller;
  CAN_message_t msg;

  void setup(uint32_t buad) {
    controller->begin();
    controller->setbaudrate(buad);
  }

  bool check_mail() {
    if (controller->read(msg)) {
      return true;
    } else {
      return false;
    }
  }

  uint8_t *read_message() {}

  void send_message(uint8_t *msg, uint16_t id, uint16_t length) {}
};
#endif
