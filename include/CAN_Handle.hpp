#pragma once

#include <cstdint>

struct can_message {
  uint16_t id;
  uint8_t *buf;
  uint8_t len;
};

#ifdef TEENSYDUINO
#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1> MDB_CAN;
FlexCAN_T4<CAN2> ACC_CAN;

CAN_message_t mdb_msg;
CAN_message_t acc_msg;

void init_can() {
  MDB_CAN.begin();
  MDB_CAN.setBaudRate(500000);
  ACC_CAN.begin();
  ACC_CAN.setBaudRate(500000);

  mdb_msg.len = 8;
  mdb_msg.flags = {0, 0, 0, 0};

  acc_msg.len = 8;
  acc_msg.flags = {0, 0, 0, 0};
}

void write_acc_can(can_message out_bound) {
  acc_msg.id = out_bound.id;
  acc_msg.len = out_bound.len;
  memcpy(acc_msg.buf, out_bound.buf, out_bound.len);

  ACC_CAN.write(acc_msg);
}

bool mdb_has_message() {
  if (MDB_CAN.read(mdb_msg)) {
    return true;
  } else {
    return false;
  }
}

can_message read_mdb_can() {
  can_message out;

  out.id = mdb_msg.id;
  out.buf = mdb_msg.buf;
  out.len = mdb_msg.len;

  return out;
}

bool acc_has_message() {
  if (ACC_CAN.read(acc_msg)) {
    return true;
  } else {
    return false;
  }
}
#endif
