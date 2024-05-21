/*
    I/O interface
    Written by Kevin Witteveen
*/


#pragma once
#include <Arduino.h>
#include <string.h>
#include <ExtRAM.h>
#include <TFT_22_ILI9225.h>
#include <cli_processing.h>
#include <tm_display.h>
#include <Wire.h>
#include <avr/wdt.h>
#include <tm_sys.h>
class Tm_io
{
public:
    void init();
    void print( String msg );
    void put( char c );
    void set_color( uint8_t raw );
    String input( bool echo );
    char get_char( bool blocking );
    void set_auto_flush( bool on ) {auto_flush=on;};
    void flush();
    void redraw();
private:
    bool auto_flush=true;
};

extern Tm_io tm_io;