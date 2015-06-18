// WatchdogLog - logs program address to EEPROM before watchdog timeout MCU reset https://github.com/per1234/WatchdogLog
#ifndef WatchdogLog_h
#define WatchdogLog_h
#include <Arduino.h>
#include <avr/wdt.h>


#if defined(__AVR_ATmega2560__)
#define PROGRAM_COUNTER_SIZE 3 /* bytes*/
#else
#define PROGRAM_COUNTER_SIZE 2 /* bytes*/
#endif


struct CWatchdogLogHeader {
  // The number of reports saved in the EEPROM.
  uint8_t m_uSavedReports;
  // the location for the next report to be saved
  uint8_t m_uNextReport;
} __attribute__((__packed__));


struct CCrashReport {
  // Address of code executing when watchdog interrupt fired.
  // On the 328 & 644 this is just a word pointer. For the mega,
  // we need 3 bytes. We can use an array to make it easy.
  uint8_t m_auAddress[PROGRAM_COUNTER_SIZE];
  // User data.
  uint32_t m_uData;
} __attribute__((__packed__));


class WatchdogLogClass {
    CCrashReport m_CrashReport;
  public:
    WatchdogLogClass();
    unsigned long getLoggedAddress();
    void begin(int nBaseAddress);

    void WatchdogInterruptHandler(uint8_t *puProgramAddress);

  private:
    void SaveHeader(const CWatchdogLogHeader &rReportHeader) const;
    void LoadHeader(CWatchdogLogHeader &rReportHeader) const;

    void SaveCurrentReport(int nReportSlot) const;
    void LoadReport(int nReport, CCrashReport &rState) const;
    int GetAddressForReport(int nReport) const;

    void ReadBlock(int nBaseAddress, void *pData, uint8_t uSize) const;
    void WriteBlock(int nBaseAddress, const void *pData, uint8_t uSize) const;

    int c_nMaxEntries;
    int c_nBaseAddress;
};

extern WatchdogLogClass WatchdogLog;

#endif

