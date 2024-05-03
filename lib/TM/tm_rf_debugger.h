#ifdef TEST_MODULE
#pragma once

#include <cli_processing.h>
#include <avr/wdt.h>
#include <tm_display.h>
#include <RH_ASK_mod.h>

extern TFT_22_ILI9225 tft;
extern RH_ASK rh_ask;

int rf_debug_open(cli_terminal_t* term, const char* full_command);
int rf_debug_toggle_receive(cli_terminal_t* term, const char* full_command);
int rf_debug_generate_packet(cli_terminal_t* term, const char* full_command);
int rf_debug_send_packet(cli_terminal_t* term, const char* full_command);
int rf_debug_ident(cli_terminal_t* term, const char* full_command);
#endif