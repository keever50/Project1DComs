/*
    Command line interface library.
    Written by Kevin Witteveen
*/

#include <cli_processing.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Standard memcpy replacement
void cli_memcopy( char* dest, const char* src, int amount )
{
    for(int i=0;i<amount;i++)
    {
        dest[i]=src[i];
    }
}

inline int cli_xy_to_addr( cli_terminal_t* term, uint16_t x, uint16_t y)
{
    return y*term->width+x;
}

void cli_set( cli_terminal_t* term, uint16_t x, uint16_t y, char c )
{
    uint16_t addr = cli_xy_to_addr(term, x, y);
    if(addr>=term->buffersize) return;
    term->buffer[addr]=c;
    term->colors[addr]=term->current_color;
}

void cli_set_addr( cli_terminal_t* term, int addr, char c )
{
    if(addr>=term->buffersize) return;
    term->buffer[addr]=c;
    term->colors[addr]=term->current_color;
}


void cli_shift_up( cli_terminal_t* term )
{
    // For each line copy next line up. Except the first line
    for(uint16_t addr=term->width ; addr<term->buffersize ; addr=addr+term->width)
    {
        cli_memcopy(term->buffer+(addr-term->width), term->buffer+addr, term->width);
        cli_memcopy(term->colors+(addr-term->width), term->colors+addr, term->width);
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
    for((*iterator)=(*iterator);(*iterator)<len; (*iterator)++)
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

// Creates a newline
void cli_newline(cli_terminal_t* term)
{
    if(term->current_line < term->height-1)
    {
        term->current_line++;
    }else{
        cli_shift_up(term);
    }

    term->current_colunn=0;
}

void cli_put(cli_terminal_t* term, char c)
{
    if(c=='\n' || c=='\r')
    {
        cli_newline(term);
        return;
    }
    cli_set( term, term->current_colunn++, term->current_line, c);
    if(term->current_colunn>=term->width) cli_newline(term);
}

void cli_print(cli_terminal_t* term, const char* str )
{
    
    for(int i=0;i<CLI_MAX_STR_LEN;i++)
    {
        char c = str[i];
        if(c==0) break;
        cli_put(term, c);
    }  
}

void cli_clear(cli_terminal_t* term)
{
    memset(term->buffer, 0, term->buffersize);
    memset(term->colors, 0, term->buffersize);
}