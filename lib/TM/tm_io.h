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


class Tm_io
{
public:
    void init();
    void print( String msg );
    void set_color( uint8_t raw );
};

extern Tm_io tm_io;