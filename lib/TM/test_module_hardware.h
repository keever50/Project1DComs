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
#define SCR_WIDTH   220
#define SCR_HEIGHT  176

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
#define RAM_CS  6 
#define RAM_MOSI 11 
#define RAM_MISO 12
#define RAM_CLK 13
#define RAM_SPEED 8000000

// Layout
#define RAM_SECT_GRAPHICS_START 0
#define RAM_SECT_GRAPHICS_END (CLI_HEIGHT*CLI_WIDTH*2)

#define RAM_SECT_PERSISTANCE_START RAM_SECT_GRAPHICS_END
#define RAM_SECT_PERSISTANCE_END (RAM_SECT_PERSISTANCE_START+(CLI_HEIGHT*CLI_WIDTH*2))


#endif