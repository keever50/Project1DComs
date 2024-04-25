#include <CI_processing.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Standard memcpy replacement
void memcopy( char* dest, const char* src, int amount )
{
    for(int i=0;i<amount;i++)
    {
        dest[i]=src[i];
    }
}

void cli_put( cli_terminal_t* term, uint16_t x, uint16_t y, char c )
{
    uint16_t addr = y*term->width+x;
    if(addr>=term->buffersize) return;
    term->buffer[addr]=c;
    term->colors[addr]=term->current_color;
}

void cli_shift_up( cli_terminal_t* term )
{
    // For each line copy next line up. Except the first line
    for(uint16_t addr=term->width ; addr<term->buffersize ; addr=addr+term->width)
    {
        memcopy(term->buffer+(addr-term->width), term->buffer+addr, term->width);
        memcopy(term->colors+(addr-term->width), term->colors+addr, term->width);
    }

    // Clean the new line appeared on the bottom
    uint16_t top = term->buffersize-term->width;
    memset(term->buffer+top, 0, term->width);
    memset(term->colors+top, 0, term->width);
}

unsigned char cli_get_next_argument_iterative(int* iterator, const char* str, char* arg, int max_arg_len, int* new_arg_len )
{
    //Clear argument
    memset(arg, 0, max_arg_len);

    // Include the null terminator to the string length
    int len = strlen(str)+1;

    unsigned char quote=0;
    (*new_arg_len)=0;
    // For each character(c) in string(str)
    for((*iterator);(*iterator)<len; (*iterator)++)
    {
        char c=str[*iterator];
        // Toggle quote to ignore whitespaces when c is a quote (" or ') and continue
        if(c==0x27 || c==0x22)
        {
            quote=!quote;
            continue;
        }

        // Stop when we found a whitespace or null, except when we are in a quote
        if(!quote && (c==' ' || c==0))
        {
            // Also count iterator up by one more so we dont start with a whitespace next time we run this function.
            (*iterator)++;
            // We also got a null terminator
            (*new_arg_len)++;
            return 1;
        }

        // When everything passes, add characters to argument. Except dont when argument is too long (So we just cut it off). Also preserve null terminator
        if((*new_arg_len)<max_arg_len-1) arg[(*new_arg_len)++]=c;
        

    }

    return 0;
}