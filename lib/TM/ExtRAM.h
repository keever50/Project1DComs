/*
    External RAM 23A512/23LC512 driver
    Written by Kevin Witteveen
*/

#ifdef TEST_MODULE

#pragma once
#include <stdint.h>
#include <Arduino.h>
#include <test_module_hardware.h>
#include <SPI.h>

#define EXTRAM_REG_READ   0x03 // Read data from memory array beginning at selected address
#define EXTRAM_REG_WRITE  0x02 // Write data to memory array beginning at selected address
#define EXTRAM_REG_EDIO   0x3B // Enter Dual I/O access
#define EXTRAM_REG_EQIO   0x38 // Enter Quad I/O access
#define EXTRAM_REG_RSTIO  0xFF // Reset Dual and Quad I/O access
#define EXTRAM_REG_RDMR   0x05 // Read Mode Register
#define EXTRAM_REG_WRMR   0x01 // Write Mode Register

#define EXTRAM_MODE_BYTE  0b00000000
#define EXTRAM_MODE_PAGE  0b10000000
#define EXTRAM_MODE_SEQ   0b01000000
#define EXTRAM_MODE_RES   0b11000000

class ExtRAM
{
public:
    // Sets the mode
    int set_mode(uint8_t mode); 
    // Writes a single byte to a 16-bit address
    void write_byte(uint16_t address, uint8_t byte);
    // Reads a single byte to a 16-bit address
    uint8_t read_byte(uint16_t address);
    // Writes multiple bytes starting from a 16-bit address
    void write_bytes(uint16_t address, const uint8_t* source, uint16_t count);
    // Reads multiple bytes starting from a 16-bit address
    void read_bytes(uint16_t address, uint8_t* destination, uint16_t count);

    void set_bytes(uint16_t start, uint8_t value, uint16_t count );

    void reset();

private:
    void write(uint8_t byte); 
    uint8_t read(); 
    // Selects the RAM
    void set_cs(bool on){ digitalWrite(RAM_CS, !on); }; 
    // Writes to a register
    void write_reg(uint8_t reg, uint8_t byte);
    // Sets the writing address
    void write_address(uint16_t address);
    // Sets the reading address
    void read_address(uint16_t address);
    // Reads a register
    uint8_t read_reg(uint8_t reg);

};

extern ExtRAM extram;

#endif