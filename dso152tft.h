#pragma once

#define Adafruit_I2CDevice_h
#define Adafruit_SPIDevice_h
#include "Adafruit_GFX.h"

#define LCD_BACKLIGHT PC9
#define LCD_RST PA8
#define LCD_CS PA15
#define LCD_RS PC13
#define LCD_WR PC14
#define LCD_RD PC15

#define LCD_RS_NUM 13
#define LCD_WR_NUM 14
#define LCD_RD_NUM 15

#define ST_CMD_DELAY 0x80  // special signifier for command lists

#define ST77XX_NOP 0x00
#define ST77XX_SWRESET 0x01
#define ST77XX_RDDID 0x04
#define ST77XX_RDDST 0x09

#define ST77XX_SLPIN 0x10
#define ST77XX_SLPOUT 0x11
#define ST77XX_PTLON 0x12
#define ST77XX_NORON 0x13

#define ST77XX_INVOFF 0x20
#define ST77XX_INVON 0x21
#define ST77XX_DISPOFF 0x28
#define ST77XX_DISPON 0x29
#define ST77XX_CASET 0x2A
#define ST77XX_RASET 0x2B
#define ST77XX_RAMWR 0x2C
#define ST77XX_RAMRD 0x2E

#define ST77XX_PTLAR 0x30
#define ST77XX_TEOFF 0x34
#define ST77XX_TEON 0x35
#define ST77XX_MADCTL 0x36
#define ST77XX_COLMOD 0x3A

#define ST77XX_MADCTL_MY 0x80
#define ST77XX_MADCTL_MX 0x40
#define ST77XX_MADCTL_MV 0x20
#define ST77XX_MADCTL_ML 0x10
#define ST77XX_MADCTL_RGB 0x00

#define ST77XX_RDID1 0xDA
#define ST77XX_RDID2 0xDB
#define ST77XX_RDID3 0xDC
#define ST77XX_RDID4 0xDD

// Some ready-made 16-bit ('565') color settings:
#define ST77XX_BLACK 0x0000
#define ST77XX_WHITE 0xFFFF
#define ST77XX_RED 0xF800
#define ST77XX_GREEN 0x07E0
#define ST77XX_BLUE 0x001F
#define ST77XX_CYAN 0x07FF
#define ST77XX_MAGENTA 0xF81F
#define ST77XX_YELLOW 0xFFE0
#define ST77XX_ORANGE 0xFC00

static const uint8_t
  generic_st7789[] = {                // Init commands for 7789 screens
    9,                                //  9 commands in list:
    ST77XX_SWRESET, ST_CMD_DELAY,     //  1: Software reset, no args, w/delay
    150,                              //     ~150 ms delay
    ST77XX_SLPOUT, ST_CMD_DELAY,      //  2: Out of sleep mode, no args, w/delay
    10,                               //      10 ms delay
    ST77XX_COLMOD, 1 + ST_CMD_DELAY,  //  3: Set color mode, 1 arg + delay:
    0x55,                             //     16-bit color
    10,                               //     10 ms delay
    ST77XX_MADCTL, 1,                 //  4: Mem access ctrl (directions), 1 arg:
    0x08,                             //     Row/col addr, bottom-top refresh
    ST77XX_CASET, 4,                  //  5: Column addr set, 4 args, no delay:
    0x00,
    0,  //     XSTART = 0
    0,
    240,              //     XEND = 240
    ST77XX_RASET, 4,  //  6: Row addr set, 4 args, no delay:
    0x00,
    0,  //     YSTART = 0
    320 >> 8,
    320 & 0xFF,                  //     YEND = 320
    ST77XX_INVON, ST_CMD_DELAY,  //  7: hack
    10,
    ST77XX_NORON, ST_CMD_DELAY,   //  8: Normal display on, no args, w/delay
    10,                           //     10 ms delay
    ST77XX_DISPON, ST_CMD_DELAY,  //  9: Main screen turn on, no args, delay
    10
  };  //    10 ms delay

class DSO152TFT : public Adafruit_GFX {
public:
  DSO152TFT()
    : Adafruit_GFX(240, 320) {}

public:
  void begin() {
    /* Configure interface. */

    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    pinMode(LCD_CS, OUTPUT);
    digitalWrite(LCD_CS, LOW);
    pinMode(LCD_RS, OUTPUT);
    digitalWrite(LCD_RS, HIGH);
    pinMode(LCD_WR, OUTPUT);
    digitalWrite(LCD_WR, HIGH);
    pinMode(LCD_RD, OUTPUT);
    digitalWrite(LCD_RD, HIGH);
    pinMode(LCD_RST, OUTPUT);
    digitalWrite(LCD_RST, HIGH);

    displayInit(generic_st7789);
    fillRect(0, 0, 240, 320, 0);
    
    /* Backlight on. */

    pinMode(LCD_BACKLIGHT, OUTPUT);
    digitalWrite(LCD_BACKLIGHT, LOW);

  }

  void drawPixel(int16_t x, int16_t y, uint16_t color) override {
    fillRect(x, y, 1, 1, color);
  }

  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override {
    fillRect(x, y, w, 1, color);
  }

  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override {
    fillRect(x, y, 1, h, color);
  }

  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override {
    // digitalWrite(LCD_CS, LOW);
    color = ~color;
    setAddrWindow(x, y, w, h);
    uint32_t pixelCount = w*h;
    while (pixelCount--)
      rawWriteData(color);
    // digitalWrite(LCD_CS, HIGH);
  }

  void setAddrWindow(uint16_t x, uint16_t y, uint16_t w,
                     uint16_t h) {
    rawWriteCommand(ST77XX_CASET);  // Column addr set
    rawWriteData88(x);
    rawWriteData88(x + w - 1);

    rawWriteCommand(ST77XX_RASET);  // Column addr set
    rawWriteData88(y);
    rawWriteData88(y + h - 1);

    rawWriteCommand(ST77XX_RAMWR);  // write to RAM
  }

private:
  static inline void setWriteDir() {
    GPIOB->CRL = GPIOB->CRH = 0x33333333;
  }


  static inline void setReadDir() {
    GPIOB->CRL = GPIOB->CRH = 0x88888888;
  }

  #define PORT_BITS_HIGH(port, mask) \
    do { \
      port->BSRR = mask; \
    } while(0)

  #define PORT_BITS_LOW(port, mask) \
    do { \
      port->BRR = mask; \
    } while(0)

  void rawWriteData(uint16_t data, uint32_t count = 1) {
    setWriteDir();
    PORT_BITS_HIGH(GPIOC, (1<<LCD_RS_NUM) | (1<<LCD_RD_NUM) | (1<<LCD_WR_NUM));
    GPIOB->ODR = data;
    while (count--)
    {
      PORT_BITS_LOW(GPIOC, 1<<LCD_WR_NUM);
      PORT_BITS_HIGH(GPIOC, 1<<LCD_WR_NUM);
    }
  }

  void rawWriteData88(uint16_t data) {
    rawWriteData(data >> 8);
    rawWriteData(data);
  }

  void rawWriteCommand(uint16_t command) {
    setWriteDir();
    PORT_BITS_LOW(GPIOC, 1<<LCD_RS_NUM);
    PORT_BITS_HIGH(GPIOC, (1<<LCD_RD_NUM) | (1<<LCD_WR_NUM));
    GPIOB->ODR = command;
    PORT_BITS_LOW(GPIOC, 1<<LCD_WR_NUM);
    PORT_BITS_HIGH(GPIOC, 1<<LCD_WR_NUM);
  }

  uint16_t rawReadData() {
    uint16_t result;
    setReadDir();
    PORT_BITS_HIGH(GPIOC, 1<<LCD_RS_NUM);
    //digitalWrite(LCD_RS, HIGH);
    digitalWrite(LCD_RD, HIGH);
    digitalWrite(LCD_WR, HIGH);

    digitalWrite(LCD_RD, LOW);
    result = GPIOB->IDR;
    digitalWrite(LCD_RD, HIGH);

    return result;
  }

  void execute(uint8_t cmd, const uint8_t* args, int numArgs) {
    // digitalWrite(LCD_CS, LOW);
    rawWriteCommand(cmd);
    while (numArgs--)
      rawWriteData(*args++);
    // digitalWrite(LCD_CS, HIGH);
  }

  void execute(uint8_t cmd) {
    execute(cmd, nullptr, 0);
  }

  void displayInit(const uint8_t* addr) {
    uint8_t numCommands = *addr++;
    while (numCommands--) {
      uint8_t cmd = *addr++;
      uint8_t numArgs = *addr++;
      uint16_t ms = numArgs & ST_CMD_DELAY;  // If hibit set, delay follows args
      numArgs &= ~ST_CMD_DELAY;              // Mask out delay bit
      execute(cmd, addr, numArgs);
      addr += numArgs;

      if (ms) {
        ms = *addr++;
        if (ms == 255)
          ms = 500;  // If 255, delay for 500 ms
        delay(ms);
      }
    }
  }
};
