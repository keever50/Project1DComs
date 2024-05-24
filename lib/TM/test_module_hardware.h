#pragma once

#if defined(TEST_MODULE) || defined(TM_MEM_HACK_TEST)

/****************
     RADIO
****************/
// Pins
#define RA_BITRATE          500
#define RA_RX               3
#define RA_TX               5
#define RA_TR               2
/****************
    TFT screen
****************/
#define SCR_WIDTH   220
#define SCR_HEIGHT  176

// Pins
#define TFT_RST 46
#define TFT_RS  48
#define TFT_CS  53  // SS
#define TFT_SDI 51  // MOSI
#define TFT_CLK 52  // SCK
#define TFT_LED 47   // Backlight
// Settings
#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight

/****************
    TERMINAL
****************/
#define CLI_FONT_HEIGHT 8
#define CLI_FONT_WIDTH  6
#define CLI_HEIGHT  22
#define CLI_WIDTH   36
#define CLI_SIZE    CLI_HEIGHT*CLI_WIDTH

/****************
    Extermal RAM
****************/
#define RAM_CS  49
#define RAM_MOSI TFT_SDI
#define RAM_MISO 50
#define RAM_CLK TFT_CLK
#define RAM_SPEED 8000000

// Layout
#define RAM_SECT_GRAPHICS_START 0
#define RAM_SECT_GRAPHICS_END (CLI_HEIGHT*CLI_WIDTH*2)

#define RAM_SECT_PERSISTANCE_START RAM_SECT_GRAPHICS_END
#define RAM_SECT_PERSISTANCE_END (RAM_SECT_PERSISTANCE_START+(CLI_HEIGHT*CLI_WIDTH*2))


#endif