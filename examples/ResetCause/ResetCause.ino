// Demonstrates usage of the WatchdogLog library to determine the cause of the last reset
#include <WatchdogLog.h>
#include <EEPROM.h>  //If you are using Arduino IDE version 1.0.x then you will need to install the EEPROM v2.0 library from here: https://github.com/arduino/Arduino/tree/ide-1.5.x/hardware/arduino/avr/libraries/EEPROM If you are using Arduino IDE version 1.6.2+ then the library is already installed.

const unsigned int EEPROMaddress = 0;  //EEPROM address to store the program address where the watchdog timeout reset occurred. The saved program address will take 4 bytes of EEPROM starting with this address.

void setup() {
  Serial.begin(9600);
  while (!Serial) {};  //for Leonardo et al. Wait for the serial monitor to be opened before proceeding with the sketch.
  Serial.print("\nThe last reset was a ");
  unsigned long programAddress;
  EEPROM.get(EEPROMaddress, programAddress);
  if (programAddress > 0) {
    Serial.println("watchdog reset.");
    programAddress = 0;
    EEPROM.put(EEPROMaddress, programAddress);  //reset the EEPROM
  }
  else {
    Serial.println("hard reset.");
  }
  Serial.println("Watchdog timeout reset will occur in 3 seconds, press reset button for hard reset...");
  delay(2500);
  wdt_enable(WDTO_500MS);  //enable the watchdog timer with a 500MS second timeout duration
  WatchdogLog.begin(EEPROMaddress);  //initialize WatchdogLog
  while (true) {};  //perpetual loop until the watchdog times out
}

void loop() {
}

