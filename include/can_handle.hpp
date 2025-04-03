#pragma once

#include "can_tools.hpp"
#include "car.h"

can_obj_car_h_t kms_can;

can_message pack_shutdown_message(bool imd_relay_state, bool bms_relay_state,
                                  bool imd_ok_hs, bool bms_ok_hs,
                                  uint8_t imd_freq, uint8_t imd_cycle) {
  // Throw everything in
  encode_can_0x258_acu_imd_relay_state(&kms_can, imd_relay_state);
  encode_can_0x258_acu_bms_relay_state(&kms_can, bms_relay_state);
  encode_can_0x258_acu_imd_gpio_state(&kms_can, imd_ok_hs);
  encode_can_0x258_acu_bms_gpio_state(&kms_can, bms_ok_hs);
  encode_can_0x258_acu_imd_pwm_frequency(&kms_can, imd_freq);
  encode_can_0x258_acu_imd_pwm_duty(&kms_can, imd_cycle);

  // Setup and pack the message
  can_message out_msg;
  out_msg.id = CAN_ID_ACU_SHUTDOWN_STATUS;
  out_msg.length =
      pack_message(&kms_can, CAN_ID_ACU_SHUTDOWN_STATUS, &out_msg.buf.val);

  return out_msg;
}

can_message pack_readings_message() {
  // Setup and pack the message
  can_message out_msg;
  out_msg.id = CAN_ID_ACU_BOARD_VOLTAGE_READINGS;
  out_msg.length = pack_message(&kms_can, CAN_ID_ACU_BOARD_VOLTAGE_READINGS,
                                &out_msg.buf.val);

  return out_msg;
}

// These values are provided by the python script ran by the lib_dep
// https://github.com/KSU-MS/pio-git-hash-gen
#ifndef AUTO_VERSION
#warning "AUTO_VERSION was not defined by the generator!"
#define AUTO_VERSION 0xdeadbeef
#endif

#ifndef FW_PROJECT_IS_DIRTY
#warning "FW_PROJECT_IS_DIRTY was not defined by the generator!"
#define FW_PROJECT_IS_DIRTY 1
#endif

#ifndef FW_PROJECT_IS_MAIN_OR_MASTER
#warning "FW_PROJECT_IS_MAIN_OR_MASTER was not defined by the generator!"
#define FW_PROJECT_IS_MAIN_OR_MASTER 0
#endif

can_message pack_status_message(uint16_t time, uint8_t temp, uint8_t humidity) {
  encode_can_0x25a_acu_temperature(&kms_can, temp);
  encode_can_0x25a_acu_humidity(&kms_can, humidity);
  encode_can_0x25a_acu_on_time_seconds(&kms_can, time);
  encode_can_0x25a_acu_firmware_version(&kms_can, AUTO_VERSION);
  encode_can_0x25a_acu_firmware_is_dirty(&kms_can, FW_PROJECT_IS_DIRTY);
  encode_can_0x25a_acu_code_on_main(&kms_can, FW_PROJECT_IS_MAIN_OR_MASTER);

  can_message out_msg;
  out_msg.id = CAN_ID_ACU_FIRMWARE_VERSION;
  out_msg.length =
      pack_message(&kms_can, CAN_ID_ACU_FIRMWARE_VERSION, &out_msg.buf.val);

  return out_msg;
}
