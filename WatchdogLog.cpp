// WatchdogLog - logs program address to EEPROM before watchdog timeout MCU reset https://github.com/per1234/WatchdogLog
#include "WatchdogLog.h"


/*
Function called when the watchdog interrupt fires. The function is naked so that
we don't get program stated pushed onto the stack. Consequently the top two
values on the stack will be the program counter when the interrupt fired. We're
going to save that in the eeprom then let the second watchdog event reset the
micro. We never return from this function.
*/
ISR(WDT_vect, ISR_NAKED) {
  register uint8_t *upStack;  //Setup a pointer to the program counter. It goes in a register so we don't mess up the stack.
  upStack = (uint8_t*)SP;

  // The stack pointer on the AVR micro points to the next available location
  // so we want to go back one location to get the first byte of the address
  // pushed onto the stack when the interrupt was triggered. There will be
  // PROGRAM_COUNTER_SIZE bytes there.
  ++upStack;
  WatchdogLog.WatchdogInterruptHandler(upStack);
}


/*
Initialize the WatchdogLog. There should only be a single instance
of the application monitor in the whole program.
nBaseAddress: The address in the EEPROM where crash data should be stored.
nMaxEntries: The maximum number of crash entries that should be stored in the
EEPROM. Storage of EEPROM data will take up sizeof(CWatchdogLogHeader) +
nMaxEntries * sizeof(CCrashReport) bytes in the EEPROM.
*/
WatchdogLogClass::WatchdogLogClass() {
  m_CrashReport.m_uData = 0;
  c_nMaxEntries = 1;  //maximum number of addresses to store
}


/*
set the watchdog timer to trigger an interrupt(WDT_vect) before
resetting the micro. When the interrupt fires, we save the program counter
to the EEPROM.
*/
void WatchdogLogClass::begin(int nBaseAddress) {
  c_nBaseAddress = nBaseAddress;
  WDTCSR |= _BV(WDIE);
}


unsigned long WatchdogLogClass::getLoggedAddress() {
  CWatchdogLogHeader Header;
  CCrashReport Report;
  uint8_t uReport = 0;
  uint32_t uAddress;

  LoadHeader(Header);
  LoadReport(uReport, Report);

  //rDestination.print(uReport);
  uAddress = 0;
  memcpy(&uAddress, Report.m_auAddress, PROGRAM_COUNTER_SIZE);
  return uAddress * 2;
}


void WatchdogLogClass::WatchdogInterruptHandler(uint8_t *puProgramAddress) {
  wdt_enable(WDTO_120MS);
  CWatchdogLogHeader Header;

  LoadHeader(Header);
  memcpy(m_CrashReport.m_auAddress, puProgramAddress, PROGRAM_COUNTER_SIZE);
  SaveCurrentReport(Header.m_uNextReport);

  // Update header for next time.
  ++Header.m_uNextReport;
  if (Header.m_uNextReport >= c_nMaxEntries)
    Header.m_uNextReport = 0;
  else
    ++Header.m_uSavedReports;
  SaveHeader(Header);
  // Wait for next watchdog time out to reset system.
  // If the watch dog timeout is too short, it doesn't
  // give the program much time to reset it before the
  // next timeout. So we can be a bit generous here.
  while (true);
}


void WatchdogLogClass::LoadHeader(CWatchdogLogHeader &rReportHeader) const {
  ReadBlock(c_nBaseAddress, &rReportHeader, sizeof(rReportHeader));

  // Ensure the report structure is valid.
  if (rReportHeader.m_uSavedReports == 0xff) // EEPROM is 0xff when uninitialized
    rReportHeader.m_uSavedReports = 0;
  else if (rReportHeader.m_uSavedReports > c_nMaxEntries)
    rReportHeader.m_uSavedReports = c_nMaxEntries;

  if (rReportHeader.m_uNextReport >= c_nMaxEntries)
    rReportHeader.m_uNextReport = 0;
}


void WatchdogLogClass::SaveHeader(const CWatchdogLogHeader &rReportHeader) const {
  WriteBlock(c_nBaseAddress, &rReportHeader, sizeof(rReportHeader));
}


void WatchdogLogClass::SaveCurrentReport(int nReportSlot) const {
  WriteBlock(GetAddressForReport(nReportSlot), &m_CrashReport, sizeof(m_CrashReport));
}


void WatchdogLogClass::LoadReport(int nReport, CCrashReport &rState) const {
  ReadBlock(GetAddressForReport(nReport), &rState, sizeof(rState));

  // The return address is reversed when we read it off the stack. Correct that.
  // by reversing the byte order. Assuming PROGRAM_COUNTER_SIZE is 2 or 3.
  uint8_t uTemp;
  uTemp = rState.m_auAddress[0];
  rState.m_auAddress[0] = rState.m_auAddress[PROGRAM_COUNTER_SIZE - 1];
  rState.m_auAddress[PROGRAM_COUNTER_SIZE - 1] = uTemp;
}


int WatchdogLogClass::GetAddressForReport(int nReport) const {
  int nAddress;

  nAddress = c_nBaseAddress + sizeof(CWatchdogLogHeader);
  if (nReport < c_nMaxEntries)
    nAddress += nReport * sizeof(m_CrashReport);
  return nAddress;
}


void WatchdogLogClass::ReadBlock(int nBaseAddress, void *pData, uint8_t uSize) const {
  uint8_t *puData = (uint8_t *)pData;
  while (uSize --)
    *puData++ = eeprom_read_byte((const uint8_t *)nBaseAddress++);
}


void WatchdogLogClass::WriteBlock(int nBaseAddress, const void *pData, uint8_t uSize) const {
  const uint8_t *puData = (const uint8_t *)pData;
  while (uSize --)
    eeprom_write_byte((uint8_t *)nBaseAddress++, *puData++);
}

WatchdogLogClass WatchdogLog;

