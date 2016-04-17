WatchdogLog
==========

[Arduino](http://arduino.cc) library that logs the program address where a watchdog timeout occurred to EEPROM before the microcontroller resets.

*Who watches the watchdog?* This library helps to find the cause of watchdog timeouts by saving the program address where the timeout occurred. This program address can be used with a disassembly of the compiled code to identify the location of the problem in your sketch. Another use for the library is to identify the cause of a reset.

Based on: http://www.megunolink.com/how-to-detect-lockups-using-the-arduino-watchdog/


#### The Watchdog Timer
The main purpose of the watchdog timer is to automatically reset the Arduino if it locks up. The timeout duration of watchdog timer is set using `wdt_enable()`. If the timer is not reset using `wdt_reset()` before timing out it will trigger a reset of the microcontroller. Often the cause of unintended watchdog timeouts is a bug in the code. This library helps you to identify the location of the bug.


<a id="installation"></a>
#### Installation
- Download the most recent version of WatchdogLog here: https://github.com/per1234/WatchdogLog/archive/master.zip
- Using Arduino IDE 1.0.x:
  - **Sketch > Import Library... > Add Library... >** select the downloaded file **> Open**
- Using Arduino IDE 1.5+:
  - **Sketch > Include Library > Add ZIP Library... >** select the downloaded file **> Open**


<a id="usage"></a>
#### Usage
See the example sketches in **File > Examples > WatchdogLog** for demonstration of usage.

##### `WatchdogLog.begin(baseEEPROMaddress)`
Put the watchdog into interrupt+reset mode and set the base EEPROM address.
- Parameter: **baseEEPROMaddress** - The base EEPROM address for storage of the program address where the watchdog timeout ocurred to EEPROM. The saved program address will take 4 bytes of EEPROM starting with this address.
  - Type: unsigned int
- Returns: **true** = success, **false** = invalid EEPROM base address. The base address must allow for the 4 byte program address value to fit within the capacity of the EEPROM.
  - Type: boolean


<a id="disassembly"></a>
#### Disassembly
When you upload a sketch to your Arduino the IDE compiles your program to assembly code. The program address logged by WatchdogLog relates to this code so you must produce a disassembly of the compiled code. This is accomplished by using the avr-objdump program included with the Arduino IDE in the **Arduino\hardware\tools\avr\bin\** folder:
```
avr-objdump -I[sketch path] -d -S -j .text [.elf file path] > disassembly.txt
```
The location of the compiled .elf file can be found by turning verbose ouput on in **File > Preferences > Show verbose output during: compilation**(check). If you are using Arduino IDE versions 1.0.2 up to and including 1.6.5-r5 then avr-objdump needs to know the location of your sketch so that it can include the source code with the assembly code.

I have written a Windows batch file that automates the process of creating a disassembly dump of the last compiled sketch: http://github.com/per1234/ArduinoDisassembly

Serial output from WatchdogLogExample.ino:
```
Logged program address: 224
```

Relevant section of the disassembly dump:
```
  Serial.println("Waiting for watchdog timeout reset...");
 20a:  0f b6         in  r0, 0x3f  ; 63
 20c: f8 94         cli
 20e: a8 95         wdr
 210: 80 93 60 00   sts 0x0060, r24
 214: 0f be         out 0x3f, r0  ; 63
  wdt_enable(WDTO_500MS);  //enable the watchdog timer with a 500MS second timeout duration
 216: 20 93 60 00   sts 0x0060, r18
 21a: 60 e0         ldi r22, 0x00 ; 0
 21c: 70 e0         ldi r23, 0x00 ; 0
 21e: 88 e8         ldi r24, 0x88 ; 136
 220: 92 e0         ldi r25, 0x02 ; 2
 222: 02 d0         rcall .+4       ; 0x228 <_ZN16WatchdogLogClass5beginEj>
 224: ff cf         rjmp  .-2       ; 0x224 <setup+0xe0>

00000226 <loop>:
 226: 08 95         ret
 ```
In this simple example it is fairly easy to decipher the disassembly but in many cases the address will point to the code of a function from a library that was running when the reset occurred. So you will need to find where in your code that function was called but it can still provide a valuable clue to track down an elusive bug.

