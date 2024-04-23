#pragma once

#include <stdio.h>
#include <stdlib.h>

// Returns 0 on even, 1 on odd
unsigned char get_parity(int value, int bits);

// Calculates an 8-bit LRC
unsigned char get_LRC(unsigned char* values, int len);

// Calculates an 8-bit checksum
unsigned char get_checksum(unsigned char* values, int len);
