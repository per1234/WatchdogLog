// WatchdogLog - logs program address to EEPROM before watchdog timeout MCU reset https://github.com/per1234/WatchdogLog
#include "WatchdogLog.h"


#if defined(__AVR_ATmega2560__)
const byte programCounterSize = 3;  //bytes
#else
const byte programCounterSize = 2;  //bytes
#endif

/*
Function called when the watchdog interrupt fires. The function is naked so that
we don't get program stated pushed onto the stack. Consequently the top two
values on the stack will be the program counter when the interrupt fired. We're
going to save that in the eeprom then let the second watchdog event reset the
micro. We never return from this function.
*/
ISR(WDT_vect, ISR_NAKED) {
  register uint8_t *programCounter;  //Setup a pointer to the program counter. It goes in a register so we don't mess up the stack.
  programCounter = (uint8_t*)SP;

  // The stack pointer on the AVR micro points to the next available location
  // so we want to go back one location to get the first byte of the address
  // pushed onto the stack when the interrupt was triggered. There will be
  // programCounterSize bytes there.
  programCounter += programCounterSize;

  WatchdogLog.writeLog(programCounter);
}


WatchdogLogClass::WatchdogLogClass() {
}


boolean WatchdogLogClass::begin(unsigned int EEPROMbaseAddressInput) {
  if (EEPROMbaseAddressInput > E2END - sizeof(unsigned int) + 1) {
    return false;
  }
  EEPROMbaseAddress = EEPROMbaseAddressInput;  //set the base EEPROM address for logging the program address
  WDTCSR |= _BV(WDIE);  //set the watchdog timer to trigger an interrupt(WDT_vect) before resetting the micro
  return true;
}


void WatchdogLogClass::writeLog(uint8_t *programCounter) {
  wdt_disable();  //disable the watchdog until the log is written

  uint8_t programAddressArray[sizeof(unsigned long)];
  //the stack has the bytes of the address in reverse order
  for (byte counter = 0; counter < programCounterSize; counter++) {
    programAddressArray[counter] = *programCounter;
    programCounter--;
  }

  //convert the array into unsigned long
  unsigned long programAddress;
  memcpy(&programAddress, programAddressArray, sizeof(unsigned long));
  programAddress = programAddress * 2;  //convert from word to byte

  eeprom_update_dword((unsigned long*)EEPROMbaseAddress, programAddress);  //write the program address to the EEPROM

  //reset the MCU
  wdt_enable(WDTO_15MS);
  while (true) {};
}


WatchdogLogClass WatchdogLog;

