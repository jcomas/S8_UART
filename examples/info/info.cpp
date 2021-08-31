/************************************
   Get information about CO2 sensor 
 ************************************/

#include <Arduino.h>
#include "s8_uart.h"


/* BEGIN CONFIGURATION */
#define DEBUG_BAUDRATE 115200

#if (defined USE_SOFTWARE_SERIAL || defined ARDUINO_ARCH_RP2040)
  #define S8_RX_PIN 5         // Rx pin which the S8 Tx pin is attached to (change if it is needed)
  #define S8_TX_PIN 4         // Tx pin which the S8 Rx pin is attached to (change if it is needed)
#else
  #define S8_UART_PORT  1     // Change UART port if it is needed
#endif
/* END CONFIGURATION */


#ifdef USE_SOFTWARE_SERIAL
  SoftwareSerial S8_serial(S8_RX_PIN, S8_TX_PIN);
#else
  #if defined(ARDUINO_ARCH_RP2040)
    REDIRECT_STDOUT_TO(Serial)    // to use printf (Serial.printf not supported)
    UART S8_serial(S8_TX_PIN, S8_RX_PIN, NC, NC);
  #else
    HardwareSerial S8_serial(S8_UART_PORT);   
  #endif
#endif


S8_UART *sensor_S8;
S8_sensor sensor;


void setup() {

  // Configure serial port, we need it for debug
  Serial.begin(DEBUG_BAUDRATE);

  // Wait port is open or timeout
  int i = 0;
  while (!Serial && i < 50) {
    delay(10);
    i++;
  }
  
  // First message, we are alive
  Serial.println("");
  Serial.println("Init");

  // Initialize S8 sensor
  S8_serial.begin(S8_BAUDRATE);
  sensor_S8 = new S8_UART(S8_serial);

  // Check if S8 is available
  sensor_S8->get_firmware_version(sensor.firm_version);
  int len = strlen(sensor.firm_version);
  if (len == 0) {
      Serial.println("SenseAir S8 CO2 sensor not found!");
      while (1) { delay(1); };
  }

  // Show S8 sensor info
  Serial.println(">>> SenseAir S8 NDIR CO2 sensor <<<");

  Serial.print("Firmware version: "); Serial.println(sensor.firm_version);

  sensor.sensor_type_id = sensor_S8->get_sensor_type_ID();
  Serial.print("Sensor type: 0x"); printIntToHex(sensor.sensor_type_id, 3); Serial.println("");

  sensor.sensor_id = sensor_S8->get_sensor_ID();
  Serial.print("Sensor ID: 0x"); printIntToHex(sensor.sensor_id, 4); Serial.println("");

  sensor.map_version = sensor_S8->get_memory_map_version();
  Serial.print("Memory map version: "); Serial.println(sensor.map_version);

  sensor.abc_period = sensor_S8->get_ABC_period();
  
  if (sensor.abc_period > 0) {
    Serial.print("ABC (automatic background calibration) period: ");
    Serial.print(sensor.abc_period); Serial.println(" hours");
  } else {
    Serial.println("ABC (automatic calibration) is disabled");
  }
  
}


void loop() {

  delay(10);

}
