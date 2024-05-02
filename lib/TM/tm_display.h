/*
    Test module display helper functions.
    Written by Kevin Witteveen
*/
#ifdef TEST_MODULE

#pragma once
#include <cli_processing.h>
#include <TFT_22_ILI9225.h>

#define CLI_FONT_HEIGHT 8
#define CLI_FONT_WIDTH  6
#define CLI_HEIGHT  6
#define CLI_WIDTH   20
#define SCR_WIDTH   220
#define SCR_HEIGHT  176

#define TM_RED  0b11100000
#define TM_GREEN 0b00011000
#define TM_WHITE 0b11111111
#define TM_BLACK 0
#define TM_BLUE 0b00000111


void tm_test_cli(TFT_22_ILI9225& tft, cli_terminal_t& cli);
void tm_draw_cli(TFT_22_ILI9225& tft, cli_terminal_t& cli);
// Returns 0 when user is done typing
int tm_entry(TFT_22_ILI9225& tft, char* buff, int *buff_pos, int buff_len);

#endif