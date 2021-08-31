/******************************
   To do a manual calibration 
 ******************************/

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

#define COUNTDOWN (6*60)      // Time in seconds to wait outside before a manual calibration starts
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

  // Show basic S8 sensor info
  Serial.println(">>> SenseAir S8 NDIR CO2 sensor <<<");
  printf("Firmware version: %s\n", sensor.firm_version);
  sensor.sensor_id = sensor_S8->get_sensor_ID();
  Serial.print("Sensor ID: 0x"); printIntToHex(sensor.sensor_id, 4); Serial.println("");

  // Countdown waiting outside
  Serial.println("Now, you put the sensor outside and wait.");
  Serial.println("Countdown begins...");
  unsigned int seconds = COUNTDOWN;
  while (seconds > 0) {
    printf("Time remaining: %d minutes %d seconds\n", seconds / 60, seconds % 60);
    delay(1000);
    seconds--;
  }
  Serial.println("Time reamining: 0 minutes 0 seconds");

  // Start manual calibration
  Serial.println("Starting manual calibration...");
  if (!sensor_S8->manual_calibration()) {
    Serial.println("Error setting manual calibration!");
    while (1) { delay(10); }
  }

  
}


void loop() {

  static unsigned int elapsed = 0;

  // Wait 2 sec lamp cycle
  delay(2000);   
  elapsed += 2;

  // Check if background calibration is finished
  sensor.ack = sensor_S8->get_acknowledgement();
  if (sensor.ack & S8_MASK_CO2_BACKGROUND_CALIBRATION) {
    printf("Manual calibration is finished. Elapsed: %u seconds\n", elapsed);
    while (1) { delay(10); }
  } else {
    Serial.println("Doing manual calibration...");
  }


}
