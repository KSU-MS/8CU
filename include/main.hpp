#pragma once // Make sure the file is only included once
#define DEBUG

#include <adc.hpp>
#include <can_tools.hpp>

#ifdef ARDUINO
#include <Arduino.h>
#include <Metro.h>
#include <SPI.h>
#endif

// These have to get refed before the ksu-fw-common guys for proper define order
#include "Pin_Defs.hpp"
#include "can_handle.hpp"
#include "imd.hpp"
// #include "led.hpp"
#include "tssi.hpp"

// Critical GPIO for SDC
adc imd_relay(avr, IMD_RELAY);
adc bms_relay(avr, BMS_RELAY);
adc imd_gpio(avr, IMD_GPIO);
adc bms_gpio(avr, BMS_GPIO);

canMan mdb_can(TEENSY_CAN1, MDB_CAN_BAUD_RATE);
canMan acc_can(TEENSY_CAN2, ACCUMULATOR_CAN_BAUD_RATE);

// tssi safety_lights;

// Non-critical GPIO for rules
// adc vSDC(mcp, SDC_SENSE);
// adc iSDC(mcp, SDC_ISENSE);
// adc v12v(mcp, SENSE_12V);
// adc i12v(mcp, ISENSE_12V);
// adc v5v(mcp, SENSE_5V);
// adc v3v(mcp, SENSE_3V);
// adc temp(mcp, TEMP);
// adc humidity(mcp, HUMIDITY);

// debug pixels;

Metro send_status_10s = Metro(10000);
Metro send_shutdown_2hz = Metro(500);

uint8_t highest_temp;
uint8_t lowest_temp;
uint8_t message_count = 0;

bool imd_is_chillin = true;
bool bms_is_chillin = true;
bool imd_relay_state = false;
bool bms_relay_state = false;
