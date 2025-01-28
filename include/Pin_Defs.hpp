#pragma once

//
////
////// CONTROLLER THINGS
// VOLTAGE TO TEMP CONSTANT
#define VOLTAGE_TO_TEMP 1

// PINS
#define CAN_CTX 0
#define CAN_CRX 1
#define NEOPIXEL_PIN 2
#define SSI_GREEN 3
#define SSI_RED 4
#define BMS_CS 9
#define ADC_CS 10
#define IMD_SENSE 17
#define IMD_RELAY 18
#define BMS_RELAY 19
#define IMD_GPIO 20
#define BMS_GPIO 21
#define ISO_CAN_CTX 22
#define ISO_CAN_CRX 23

// BAUD RATES
#define SERIAL_BAUD 9600
#define CAN_BAUD 500000
#define ISO_CAN_BAUD 500000
#define DEFAULT_SPI_SPEED 1000000

//
////
////// BOARD THINGS

// LEDS
#define NUM_NEOPIXEL 4

// ADC PINS
#define ISENSE_12V 0
#define SDC_ISENSE 1
#define SENSE_12V 2
#define SENSE_5V 3
#define SENSE_3V 4
#define SDC_SENSE 5
#define HUMIDITY 6
#define TEMP 7
