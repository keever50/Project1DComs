/*
    Command line interface library.
    Written by Kevin Witteveen
*/

#ifndef CI_PROC_H
#define CI_PROC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define CLI_MAX_STR_LEN     64
#define CLI_MAX_WIDTH   40

typedef struct
{
    uint16_t width;
    uint16_t height;
    uint16_t buffersize; // The size of a single buffer.
    char current_color;
    char* buffer;
    char* colors;
    uint16_t current_line;
    uint16_t current_colunn;
} cli_terminal_t;

typedef int(*cli_function)(cli_terminal_t* term, const char* full_command);

typedef struct
{
    cli_function* executable_functions; // Pointer to array containing .number_of_executable functions
    const char** executable_names; // Pointer to array containing .number_of_executable null-terminated strings
    int number_of_executables; // The amount of executables
} cli_executables_t;

// To be implemented by user
void cli_draw( cli_terminal_t* term ) __attribute__((weak));

/*
    Sets a character on a specific x, y coordinate.
*/
void cli_set( cli_terminal_t* term, uint16_t x, uint16_t y, char c );
/*
    Shifts the entire terminal up.
    This is useful in newlines.
*/
void cli_shift_up( cli_terminal_t* term );
/*
    Prints a string to the terminal.
    This does not create newlines.
*/
void cli_print(cli_terminal_t* term, const char* str );
/*
    Puts a character into the terminal and advances.
*/
void cli_put(cli_terminal_t* term, char c);
/*
    Clears the terminal buffer.
*/
void cli_clear(cli_terminal_t* term);

/*
    Weak functions for custom memory access. For example, an arduino has too little ram to render a big console. External RAM might be used.
*/
void cli_mem_write(cli_terminal_t* term, int address, uint8_t byte) __attribute__((weak));
uint8_t cli_mem_read(cli_terminal_t* term, int address) __attribute__((weak));

void cli_mem_writes(cli_terminal_t* term, uint8_t* source, int address, int len) __attribute__((weak));
void cli_mem_reads(cli_terminal_t* term, uint8_t* destination, int address, int len) __attribute__((weak));

void cli_demo(cli_terminal_t* term);

/*
This helper function can be repeated to find all the arguments in a string.

int* iterator takes a pointer to an iterator. The iterator is a counter that shows the next argument position. So we can run this function in repeat and advance to the next argument automatically.
const char* str is the string where to get arguments from.
char* arg is where the new argument will be written to.
int max_arg_len is the maximum size of the argument.
int* new_arg_len is where the size of the new argument we found will be written to.

Returns 0 when we reached the last argument, otherwise 1.
*/
unsigned char cli_get_next_argument_iterative(int* iterator, const char* str, char* arg, int max_arg_len, int* new_arg_len );

/*
    Takes a const char* command, where the first argument is used to find the right executable found in cli_executables_t* executables.
    Returns 0 on success, -1 on command not found. The executable can also return anything.
*/
unsigned char cli_execute( cli_terminal_t* term, const char* command, cli_executables_t* executables );


#ifdef __cplusplus
}
#endif

#endif