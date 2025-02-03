#pragma once // Make sure the file is only included once

#define DEBUG

#ifdef ARDUINO
#include <Arduino.h>
#include <Metro.h>
#include <SPI.h>
#endif

#include "CAN_Defs.hpp"
#include "CAN_Handle.hpp"
#include "Pin_Defs.hpp"
#include "adc.hpp"
#include "git.hpp"
#include "imd.hpp"
#include "led.hpp"
#include "tssi.hpp"
