#include <stdio.h>
#include <stdlib.h>

// Returns 0 on even, 1 on odd
unsigned char get_parity(int value, int bits)
{
    unsigned char par=0;
    for(int i=0;i<bits;i++) par = par^(value>>i&0x01);
    return par;
}

// Calculates an 8-bit LRC
unsigned char get_LRC(unsigned char* values, int len)
{
    int X=0;
    for(int i=0;i<len;i++) X = values[i]^X;   
    return X;
}

// Calculates an 8-bit checksum
unsigned char get_checksum(unsigned char* values, int len)
{
    unsigned char sum=0;
    for(int i=0;i<len;i++) sum=sum+values[i];
    return sum;
}
