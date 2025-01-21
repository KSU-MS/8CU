#pragma once

#ifdef TEENSYDUINO
#include <FlexCAN_T4.h>

template <typename FlexCAN_T4> class canMan {
  FlexCAN_T4 *controller;
  CAN_message_t msg;

public:
  void setup(uint32_t buad) {
    controller->begin();
    controller->setBaudRate(buad);
  }

  uint16_t check_mail() {
    if (controller->read(msg)) {
      return msg.id;
    } else {
      return msg.id;
    }
  }

  uint8_t *read_message() {
    uint8_t *out;
    for (int i; i < msg.len; i++) {
      out[i] = msg.buf[i];
    }

    return out;
  }

  void send_message(uint8_t *out, uint16_t id, uint16_t length) {
    memcpy(msg.buf, out, length);
    msg.id = id;
    msg.len = length;

    controller->write(msg);
  }
};

FlexCAN_T4<CAN1> MDB_CAN;
FlexCAN_T4<CAN2> ACC_CAN;
CAN_message_t msg;
#endif
