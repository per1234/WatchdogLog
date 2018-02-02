// WatchdogLog - logs program address to EEPROM before watchdog timeout MCU reset https://github.com/per1234/WatchdogLog
#ifndef WatchdogLog_h
#define WatchdogLog_h
#include <Arduino.h>
#include <avr/wdt.h>


class WatchdogLogClass {
  public:
    WatchdogLogClass();
    boolean begin(unsigned int EEPROMbaseAddressInput);
    void writeLog(uint8_t *programCounter);
  private:
    unsigned int EEPROMbaseAddress;
};

extern WatchdogLogClass WatchdogLog;

#endif  //WatchdogLog_h
