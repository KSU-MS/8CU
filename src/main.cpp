#include "main.hpp"
#include "core_pins.h"

void setup() {
  digitalWrite(LED_BUILTIN, HIGH);

  pinMode(SSI_RED, OUTPUT);
  pinMode(SSI_GREEN, OUTPUT);

  safety_lights.update_status(true, true, false, false);
  safety_lights.update_lights();

  // pixels.setup();

  // Uhh, the IMD just kinda takes a sec to give a signal, so we have to wait
  delay(2000);

  init_imd();
}

void loop() {
  digitalToggle(LED_BUILTIN); // Flip for SOL

  safety_lights.update_lights();

  if (send_shutdown_2hz.check()) {
    //
    //// SDC updates
    imd_relay.update();
    bms_relay.update();
    imd_gpio.update();
    bms_gpio.update();

    int imd_hz = get_imd_hz();
    int imd_duty = get_imd_duty();

#ifdef DEBUG
    Serial.printf("IMD Relay: %i\t", imd_relay.value.in);
    Serial.printf("BMS Relay: %i\t", bms_relay.value.in);
    Serial.printf("IMD gpio: %i\t", imd_gpio.value.in);
    Serial.printf("BMS gpio: %i\t", bms_gpio.value.in);
    Serial.printf("IMD Freq: %i\t", imd_hz);
    Serial.printf("IMD duty cycle: %i\r\n", imd_duty);
#endif

    if (imd_relay.value.in < 50)
      imd_relay_state = true;
    else
      imd_relay_state = false;

    if (bms_relay.value.in < 50)
      bms_relay_state = true;
    else
      bms_relay_state = false;

    if (imd_gpio.value.in > 500)
      imd_is_chillin = true;
    else
      imd_is_chillin = false;

    if (bms_gpio.value.in > 500)
      bms_is_chillin = true;
    else
      bms_is_chillin = false;

    safety_lights.update_status(imd_is_chillin, bms_is_chillin, imd_relay_state,
                                bms_relay_state);

    acc_can.send_controller_message(
        pack_shutdown_message(imd_relay_state, bms_relay_state, imd_is_chillin,
                              bms_is_chillin, imd_hz, imd_duty));
  }

  if (send_status_10s.check()) {
    uint16_t time = millis() / 1000;
    // temp.update();
    // humidity.update();

    // VDD=3.3V 15.1 mV/°C, MCP does 12bit reads, but value shouldn't read
    // above 100 so cast to uint8_t and ball. So get our shit into volts
    // 2^12 - 1 = 4095 -> 3.3/4095 = 0.000805860805861
    // Get our volts into degrees
    // 1/(15.1 * 10^-3) = 66.225
    // Multiply the two and now we have a factor for uint16 -> degrees
    // uint8_t temp_value = uint8_t(temp.value.in * 0.0533682652888);
    uint8_t temp_value = 0;

    // VDD=3.3V 26.4 mV/%RH, same logic as above
    // uint8_t humidity_value = uint8_t(humidity.value.in * 0.030525030525);
    uint8_t humid_value = 0;

#ifdef DEBUG
    Serial.printf("On time: %d\t", time);
    Serial.printf("Temp: %d\t", temp_value);
    Serial.printf("Humidity: %d\r\n", humid_value);
#endif

    acc_can.send_controller_message(
        pack_status_message(time, temp_value, humid_value));

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
    // acu_readings.buf[0] = map(vSDC.value.in, 0, 4095, 0, 255);
    // acu_readings.buf[1] = map(iSDC.value.in, 0, 4095, 0, 255);
    // acu_readings.buf[2] = map(v12v.value.in, 0, 4095, 0, 255);
    // acu_readings.buf[3] = map(i12v.value.in, 0, 4095, 0, 255);
    // acu_readings.buf[4] = map(v5v.value.in, 0, 4095, 0, 255);
    // acu_readings.buf[5] = map(v3v.value.in, 0, 4095, 0, 255);
  }

  // Scrape out the highest and lowest voltage readings, convert them to
  // tempatures here for the highest and lowest temp message for the orion
  // BMS, get new temps after 5 messages (we only have 5 MDBs, so we can assume
  // after 5 messages everyone has talked) so that its a rolling number and not
  // just the peak high and low, then foward all the other messages we are
  // expecting, filter the rest for figuring out what is where it shouldn't be
  if (mdb_can.check_controller_message()) {
    can_message inbound_msg = mdb_can.get_controller_message();

    if (inbound_msg.id == CAN_ID_MODULE1_TEMPS || CAN_ID_MODULE2_TEMPS ||
        CAN_ID_MODULE3_TEMPS || CAN_ID_MODULE4_TEMPS || CAN_ID_MODULE5_TEMPS) {
      for (int i = 0; i < inbound_msg.length; i++) {
        if ((inbound_msg.buf.byte[i] > highest_temp) || message_count > 5) {
          highest_temp = inbound_msg.buf.byte[i] * VOLTAGE_TO_TEMP;
          message_count++;
        } else if (inbound_msg.buf.byte[i] < lowest_temp || message_count > 5) {
          lowest_temp = inbound_msg.buf.byte[i] * VOLTAGE_TO_TEMP;
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

      acc_can.send_controller_message(inbound_msg);
    } else if (inbound_msg.id == CAN_ID_MODULE1_STATUS ||
               CAN_ID_MODULE2_STATUS || CAN_ID_MODULE3_STATUS ||
               CAN_ID_MODULE4_STATUS || CAN_ID_MODULE5_STATUS) {
      acc_can.send_controller_message(inbound_msg);
    } else {
      Serial.print("Got unexpected CAN message on MDB bus with ID: ");
      Serial.println(inbound_msg.id);
    }
  }
}
