#pragma once

#if defined(TEST_MODULE) || defined(TM_MEM_HACK_TEST)

/****************
     RADIO
****************/
// Pins
#define RA_BITRATE          1000
#define RA_RX               3
#define RA_TX               5
#define RA_TR               2
/****************
    TFT screen
****************/
// Pins
#define TFT_RST 9
#define TFT_RS  8
#define TFT_CS  10  // SS
#define TFT_SDI 11  // MOSI
#define TFT_CLK 13  // SCK
#define TFT_LED 7   // Backlight
// Settings
#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight

/****************
    RAM
****************/
#define RAM_CS  6 
#define RAM_MOSI 11 
#define RAM_MISO 12
#define RAM_CLK 13
#define RAM_SPEED 8000000
#endif