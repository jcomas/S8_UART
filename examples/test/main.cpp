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
    UART S8_serial(S8_TX_PIN, S8_RX_PIN, 0, 0);
  #else
    HardwareSerial S8_serial(S8_UART_PORT);   
  #endif
#endif


S8_UART *sensor_S8;
S8_sensor sensor;


void setup() {

  // Pico lcd 1.44 off
  pinMode(13, OUTPUT);
  digitalWrite(13, 0);

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
  Serial.println("Init test PlatformIO IDE");

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
  

/*
  Serial.println("Disable ABC period");
  sensor_S8->set_ABC_period(0);
  delay(1000);
  printf("ABC period (0 = disabled): %d hours\n", sensor_S8->get_ABC_period());

  Serial.println("ABC period set to 4900 hours");
  sensor_S8->set_ABC_period(4900);
  delay(1000);
  printf("ABC period (0 = disabled): %d hours\n", sensor_S8->get_ABC_period());

  while (1) { delay(1);}

  Serial.println("ABC period set to 180 hours");
  sensor_S8->set_ABC_period(180);
  delay(1000);
  printf("ABC period (0 = disabled): %d hours\n", sensor_S8->get_ABC_period());
*/

  sensor.ack = sensor_S8->get_acknowledgement();
  Serial.print("Acknowledgement: b"); printBinary(sensor.ack); Serial.println("");

  sensor.meter_status = sensor_S8->get_meter_status();
  Serial.print("Meter status: b"); printBinary(sensor.meter_status); Serial.println("");

  sensor.alarm_status = sensor_S8->get_alarm_status();
  Serial.print("Alarm status: b"); printBinary(sensor.alarm_status); Serial.println("");

  sensor.output_status = sensor_S8->get_output_status();
  Serial.print("Output status: b"); printBinary(sensor.output_status); Serial.println("");
  
}


void loop() {
  
  //printf("Millis: %lu\n", millis());

  // Get CO2 measure
  sensor.co2 = sensor_S8->get_co2();
  printf("CO2 value = %d ppm\n", sensor.co2);

  // Compare with PWM output
  sensor.pwm_output = sensor_S8->get_PWM_output();
  printf("PWM output = %0.0f ppm\n", (sensor.pwm_output / 16383.0) * 2000.0);

  // Wait 5 second for next measure
  delay(5000);
}
