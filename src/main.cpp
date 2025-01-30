#include "main.hpp"

// Critical GPIO for SDC
adc imd_relay;
adc bms_relay;
adc imd_gpio;
adc bms_gpio;

// Non-critical GPIO for rules
adc vSDC;
adc iSDC;
adc v12v;
adc i12v;
adc v5v;
adc v3v;
adc temp;
adc humidity;

debug pixels;

Metro send_status_10s = Metro(10000);
Metro send_data_2hz = Metro(500);
can_message acu_status;
can_message acu_shutdown;
can_message acu_readings;

uint8_t highest_temp;
uint8_t lowest_temp;
uint8_t message_count = 0;

bool imd_is_chillin;
bool bms_is_chillin;
bool imd_gpio_high;
bool bms_gpio_high;

void setup() {
  imd_relay.setup(avr, IMD_RELAY);
  bms_relay.setup(avr, BMS_RELAY);
  imd_gpio.setup(avr, IMD_GPIO);
  bms_gpio.setup(avr, BMS_GPIO);

  vSDC.setup(mcp, SDC_SENSE);
  iSDC.setup(mcp, SDC_ISENSE);
  v12v.setup(mcp, SENSE_12V);
  i12v.setup(mcp, ISENSE_12V);
  v5v.setup(mcp, SENSE_5V);
  v3v.setup(mcp, SENSE_3V);
  temp.setup(mcp, TEMP);
  humidity.setup(mcp, HUMIDITY);

  pixels.setup();

  init_imd();

  init_can();

  acu_status.id = ID_ACU_STATUS;
  acu_status.len = sizeof(board_status);

  acu_shutdown.id = ID_ACU_SHUTDOWN_DATA;
  acu_shutdown.len = 6;

  acu_readings.id = ID_ACU_READINGS;
  acu_readings.len = 6;
}

void loop() {
  if (send_data_2hz.check()) {
    //
    //// SDC updates
    imd_relay.update();
    bms_relay.update();
    imd_gpio.update();
    bms_gpio.update();

#ifdef DEBUG
    Serial.printf("IMD Relay: %d\t", imd_relay.value.in);
    Serial.printf("BMS Relay: %d\t", bms_relay.value.in);
    Serial.printf("IMD gpio: %d\t", imd_gpio.value.in);
    Serial.printf("BMS gpio: %d\t", bms_gpio.value.in);
    Serial.printf("IMD Freq: %d\t", get_imd_hz());
    Serial.printf("IMD duty cycle: ", get_imd_duty());
    Serial.println();
#endif

    if (imd_relay.value.in < 50)
      imd_is_chillin = true;
    else
      imd_is_chillin = false;

    if (bms_relay.value.in < 50)
      bms_is_chillin = true;
    else
      bms_is_chillin = false;

    if (imd_gpio.value.in > 500)
      imd_gpio_high = true;
    else
      imd_gpio_high = false;

    if (bms_gpio.value.in > 500)
      bms_gpio_high = true;
    else
      bms_gpio_high = false;

    acu_shutdown.buf[0] = imd_is_chillin;
    acu_shutdown.buf[1] = bms_is_chillin;
    acu_shutdown.buf[2] = imd_gpio_high;
    acu_shutdown.buf[3] = bms_gpio_high;
    acu_shutdown.buf[4] = get_imd_hz();
    acu_shutdown.buf[5] = get_imd_duty();

    write_acc_can(acu_shutdown);

    //
    //// Gizmo updates
    vSDC.update();
    iSDC.update();
    v12v.update();
    i12v.update();
    v5v.update();
    v3v.update();

#ifdef DEBUG
    Serial.printf("SDC Voltage: %d", vSDC.value.in);
    Serial.printf("SDC Current: %d\t", iSDC.value.in);
    Serial.printf("12v Voltage: %d\t", v12v.value.in);
    Serial.printf("12v Current: %d\t", i12v.value.in);
    Serial.printf("5v  Voltage: %d\t", v5v.value.in);
    Serial.printf("3v3 Voltage: %d\t", v3v.value.in);
    Serial.println();
#endif

    // This 4095 number comes from the ADCs 12bit readings, we convert to
    // voltage at the DBC to save on message space
    acu_readings.buf[0] = map(vSDC.value.in, 0, 4095, 0, 255);
    acu_readings.buf[1] = map(iSDC.value.in, 0, 4095, 0, 255);
    acu_readings.buf[2] = map(v12v.value.in, 0, 4095, 0, 255);
    acu_readings.buf[3] = map(i12v.value.in, 0, 4095, 0, 255);
    acu_readings.buf[4] = map(v5v.value.in, 0, 4095, 0, 255);
    acu_readings.buf[5] = map(v3v.value.in, 0, 4095, 0, 255);
  }

  if (send_status_10s.check()) {
    set_time();
    temp.update();
    humidity.update();

    // VDD=3.3V 15.1 mV/°C, MCP does 12bit reads, but value shouldn't read
    // above 100 so cast to uint8_t and ball. So get our shit into volts
    // 2^12 - 1 = 4095 -> 3.3/4095 = 0.000805860805861
    // Get our volts into degrees
    // 1/(15.1 * 10^-3) = 66.225
    // Multiply the two and now we have a factor for uint16 -> degrees
    uint8_t temp_value = uint8_t(temp.value.in * 0.0533682652888);
    set_temp(temp_value);

    // VDD=3.3V 26.4 mV/%RH, same logic as above
    uint8_t humidity_value = uint8_t(humidity.value.in * 0.030525030525);
    set_humidity(humidity_value);

#ifdef DEBUG
    Serial.printf("On time: %d\t", (millis() / 1000));
    Serial.printf("Temp: %d\t", temp.value.in);
    Serial.printf("Humidity: %d", humidity.value.in);
    Serial.println();
#endif

    write_acc_can(acu_status);
  }

  // Scrape out the highest and lowest voltage readings, convert them to
  // tempatures here for the highest and lowest temp message for the orion
  // BMS, get new temps after 5 messages so that its a rolling number and not
  // just the peak high and low, then foward all the other messages we are
  // expecting, filter the rest for figuring out what is where it shouldn't be
  if (mdb_has_message()) {
    can_message inbound_msg = read_mdb_can();

    if (inbound_msg.id == MODULE_1_A || MODULE_2_A || MODULE_3_A ||
        MODULE_4_A || MODULE_5_A) {
      for (int i = 0; i < inbound_msg.len; i++) {
        if ((inbound_msg.buf[i] > highest_temp) || message_count > 5) {
          highest_temp = inbound_msg.buf[i] * VOLTAGE_TO_TEMP;
          message_count++;
        } else if (inbound_msg.buf[i] < lowest_temp || message_count > 5) {
          lowest_temp = inbound_msg.buf[i] * VOLTAGE_TO_TEMP;
          message_count++;
        } else {
          message_count = 0;
        }
      }

#ifdef DEBUG
      Serial.printf("Highest Temp: %d", highest_temp);
      Serial.printf("\tLowest Temp: %d", lowest_temp);
      Serial.println();
#endif

      write_acc_can(inbound_msg);
    } else if (inbound_msg.id == MODULE_1_B || MODULE_2_B || MODULE_3_B ||
               MODULE_4_B || MODULE_5_B) {
      write_acc_can(inbound_msg);
    } else {
      Serial.print("Got unexpected CAN message on MDB bus with ID: ");
      Serial.println(inbound_msg.id);
    }
  }
}
