WatchdogLog
==========

[Arduino](http://arduino.cc) library that logs the program address where a watchdog timeout occurred to EEPROM before the microcontroller resets.

Based on: http://www.megunolink.com/how-to-detect-lockups-using-the-arduino-watchdog/


#### The Watchdog Timer
The main purpose of the watchdog timer is to automatically reset the Arduino if it locks up. The timeout duration of watchdog timer is set using `wdt_enable()`. If the timer is not reset using `wdt_reset()` before timing out it will trigger a reset of the microcontroller. Often the cause of the timeout will be a bug in the code. This library helps you to identify the location of the bug.


<a id="installation"></a>
#### Installation
- Download the most recent version of WatchdogLog here: https://github.com/per1234/WatchdogLog/archive/master.zip
- Using Arduino IDE 1.0.x:
  - Sketch > Import Library... > Add Library... > select the downloaded file > Open
- Using Arduino IDE 1.5+:
  - Sketch > Include Library > Add ZIP Library... > select the downloaded file > Open


<a id="usage"></a>
#### Usage
See the example sketch **WatchdogLog.ino** for demonstration of usage.

`WatchdogLog.begin(baseEEPROMaddress)` - Put the watchdog into interrupt+reset mode and set the base eeprom address.
- Parameter: **baseEEPROMaddress** - The base EEPROM address for storage of the program address where the watchdog timeout ocurred.
- Returns: none

`WatchdogLog.getLoggedAddress()`
- Returns: The last program address where a watchdog timeout ocurred.
  - Type: unsigned long


<a id="disassembly"></a>
#### Disassembly
When you upload a sketch to your Arduino the IDE compiles your program to assembly code. The program address logged by WatchdogLog relates to this code so you must produce a disassembly of the compiled code. This is accomplished by using the avr-objdump program included with the Arduino IDE in the Arduino\hardware\tools\avr\bin\ folder:

`avr-objdump -I[sketch path] -d -S -j .text [.elf file path] > disassembly.txt`

avr-objdump needs to know the location of your sketch so that it can include the source code with the assembly code. It also needs to know the location of the compiled .elf file. This can be found by turning verbose ouput on in File > Preferences > Show verbose output during: compilation(check).

I have written a Windows batch file that automates the process of creating a disassembly dump of the last compiled sketch: http://github.com/per1234/ArduinoDisassembly

Serial output from WatchdogLogExample.ino:
`Logged program address: 20E`

Relevant section of the disassembly dump:
```avrasm
  Serial.print("Waiting for watchdog timeout reset");
  WatchdogLog.begin(EEPROMaddress);  //initialize WatchdogLog
 1f4:  0f be         out 0x3f, r0  ; 63
 1f6: 20 93 60 00   sts 0x0060, r18
 1fa: 60 e0         ldi r22, 0x00 ; 0
 1fc: 70 e0         ldi r23, 0x00 ; 0
 1fe: 86 e8         ldi r24, 0x86 ; 134
 200: 92 e0         ldi r25, 0x02 ; 2
 202: 14 d0         rcall .+40      ; 0x22c <_ZN8Watchdog12CWatchdogLog5beginEi>
 204: 61 e4         ldi r22, 0x41 ; 65
 206: 72 e0         ldi r23, 0x02 ; 2
 208: 81 e9         ldi r24, 0x91 ; 145
  wdt_enable(WDTO_2S);  //enable the watchdog timer with a 2 second timeout duration
 20a: 92 e0         ldi r25, 0x02 ; 2
 20c: 94 d3         rcall .+1832    ; 0x936 <_ZN5Print5printEPKc>
 20e: ff cf         rjmp  .-2       ; 0x20e <setup+0xc8>

00000210 <loop>:
 210: 08 95         ret
 ```
In this simple example it is fairly easy to decipher the disassembly but in many cases the address will point to the code of a function from a library that was running when the reset occurred. So you will still need to track down where in your code that function was called but it is still a valuable clue in tracking down elusive bugs.

