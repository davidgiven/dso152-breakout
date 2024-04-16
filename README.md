# dso152-breakout
Breakout for the Fnirsi DSO152

## What?

This is a very crude port of Enrique Alberto's Arduino Breakout game for the Fnirsi DSO152 portable oscilloscope. This is a CH32F103-based cheap oscilloscope which is essentially a Blue Pill with some peripherals attached.

The Breakout game itself has some very basic modifications; the interesting code (such as it is) is in dso152tft.h, which is the LCD driver, based on Adafruit_GFX. The screen is a ST7789 connected via a 16-bit parallel interface on port B (the IM pins are strapped to ground so the interface protocol can't be changed). There is a software-serial logging output hooked up to the calibration terminal on the outside of the scope at 9600 baud, but this isn't used for anything right now.

Use the device in portrait orientation with the left-most and right-most buttons.

In addition, the enormous `annotated-pcb-scan.xcf` file is an annotated double-sided high-resolution image of the PCB with the known pins marked.

## How?

It's an Arduino sketch. Build it using the Generic ST32F103 profile in the STM32Duino core.

To upload to the device, attach a standard SWD connector to the unpopulated header in the bottom-left corner of the PCB, and then use your favourite flashing tool to disable flash read protection. **This will trash the stock firmware.** Currently I don't have a copy of this so once you do this, **the stock functionality will be unrecoverable**.

Now you can treat it like any other STM32F103-based device.

## Who?

The Breakout game is by Enrique Albertos: https://www.hackster.io/javagoza/arduino-touch-breakout-game-eda113

The hacking was by me, David Given: http://cowlark.com

## License?

The Breakout game is public domain. My code is CC0-licensed.
