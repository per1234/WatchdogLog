// Demonstrates usage of the WatchdogLog library
#include "WatchdogLog.h"
#include <EEPROM.h>  //If you are using Arduino IDE version 1.0.x then you will need to install the EEPROM v2.0 library from here: https://github.com/arduino/Arduino/tree/ide-1.5.x/hardware/arduino/avr/libraries/EEPROM If you are using Arduino IDE version 1.6.2+ then the library is already installed.

const byte EEPROMaddress = 0;  //EEPROM address to store the program address where the watchdog timeout reset occurred

Watchdog::CWatchdogLog WatchdogLog;  //instantiate the class

void setup() {
  Serial.begin(9600);
  while (!Serial) {};  //for Leonardo et al. Wait for the serial monitor to be opened before proceeding with the sketch.
  Serial.println("\nProgram start");
  unsigned long programAddress = WatchdogLog.getLoggedAddress();
  if (programAddress > 0) {
    Serial.print("Logged program address: ");
    Serial.println(programAddress, HEX);
    programAddress = 0;
    EEPROM.put(EEPROMaddress + 3, programAddress);  //reset the EEPROM
  }
  else {
    Serial.println("No program address logged");
  }
  wdt_enable(WDTO_2S);  //enable the watchdog timer with a 2 second timeout duration
  WatchdogLog.begin(EEPROMaddress);  //initialize WatchdogLog
  Serial.print("Waiting for watchdog timeout reset...");
  while (true) {};  //perpetual loop until the watchdog times out
}

void loop() {
}

