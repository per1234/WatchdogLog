// Demonstrates usage of the WatchdogLog library
// WARNING!!! Use of watchdog as done in this sketch may brick certain Arduino boards due to a bug in their bootloader. See https://github.com/per1234/WatchdogLog/#warning for more information.

#include <WatchdogLog.h>
#include <EEPROM.h>  //If you are using an Arduino IDE version prior to 1.6.2 then you will need to install the EEPROM v2.0 library from here: https://github.com/arduino/Arduino/tree/ide-1.5.x/hardware/arduino/avr/libraries/EEPROM. If you are using Arduino IDE version 1.6.2 or greater then the library is already installed.

const unsigned int EEPROMaddress = 0;  //EEPROM address to store the program address where the watchdog timeout reset occurred. The saved program address will take 4 bytes of EEPROM starting with this address.

void setup() {
  Serial.begin(9600);
  while (!Serial) {};  //for Leonardo et al. Wait for the Serial Monitor to be opened before proceeding with the sketch.
  Serial.println("\nProgram start");
  unsigned long programAddress;
  EEPROM.get(EEPROMaddress, programAddress);
  if (programAddress > 0) {
    Serial.print("Logged program address: ");
    Serial.println(programAddress, HEX);
    programAddress = 0;
    EEPROM.put(EEPROMaddress, programAddress);  //reset the EEPROM
  }
  else {
    Serial.println("No program address logged");
  }
  Serial.println("Waiting for watchdog timeout reset...");
  wdt_enable(WDTO_500MS);  //enable the watchdog timer with a 500 millisecond timeout duration
  WatchdogLog.begin(EEPROMaddress);  //initialize WatchdogLog
  while (true) {};  //perpetual loop until the watchdog times out
}

void loop() {
}
