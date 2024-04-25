#ifndef CI_PROC_H
#define CI_PROC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
    uint16_t width;
    uint16_t height;
    uint16_t buffersize;
    char current_color;
    char* buffer;
    char* colors;
} cli_terminal_t;

void cli_put( cli_terminal_t* term, uint16_t x, uint16_t y, char c );
void cli_shift_up( cli_terminal_t* term );


/*
This function can be repeated to find all the arguments in a string.

int* iterator takes a pointer to an iterator. The iterator is a counter that shows the next argument position. So we can run this function in repeat and advance to the next argument automatically.
const char* str is the string where to get arguments from.
char* arg is where the new argument will be written to.
int max_arg_len is the maximum size of the argument.
int* new_arg_len is where the size of the new argument we found will be written to.

Returns 0 when we reached the last argument, otherwise 1.
*/
unsigned char cli_get_next_argument_iterative(int* iterator, const char* str, char* arg, int max_arg_len, int* new_arg_len );

#ifdef __cplusplus
}
#endif

#endif