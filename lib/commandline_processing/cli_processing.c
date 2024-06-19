/*
    Command line interface library.
    Written by Kevin Witteveen
*/

#include <cli_processing.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
uint8_t cli_line_buffer[CLI_MAX_WIDTH];
void cli_memcopy(cli_terminal_t *term, int dest, int source, int len)
{

    cli_mem_reads(term, cli_line_buffer, source, len);
    cli_mem_writes(term, cli_line_buffer, dest, len);
}

inline int cli_xy_to_addr(cli_terminal_t *term, uint16_t x, uint16_t y)
{
    return y * term->width + x;
}

void cli_set(cli_terminal_t *term, uint16_t x, uint16_t y, char c)
{
    uint16_t addr = cli_xy_to_addr(term, x, y);
    if (addr >= term->buffersize)
        return;
    cli_mem_write(term, addr, c);
    cli_mem_write(term, addr + term->buffersize, term->current_color);
}

void cli_set_addr(cli_terminal_t *term, int addr, char c)
{
    if (addr >= term->buffersize)
        return;
    cli_mem_write(term, addr, c);
    cli_mem_write(term, addr + term->buffersize, term->current_color);
}

void cli_shift_up(cli_terminal_t *term)
{
    uint16_t w = term->width;
    uint16_t top = term->buffersize;

    // Shift every line up
    for (int addr = w; addr < top; addr = addr + w)
    {
        int source = addr;
        int dest = addr - w;
        cli_memcopy(term, dest, source, w);
        cli_memcopy(term, term->buffersize + dest, term->buffersize + source, w);
    }

    // Clear last line
    memset(cli_line_buffer, ' ', sizeof(cli_line_buffer));
    cli_mem_writes(term, cli_line_buffer, top - w, w);

    // for(int line=1; line<term->height; line++)
    // {
    //     int source = line*term->width;
    //     int dest = (line-1)*term->width;
    //     cli_memcopy(term, dest, source, term->width);
    //     cli_memcopy(term, term->buffersize+dest, term->buffersize+source, term->width);
    // }
}

unsigned char cli_get_next_argument_iterative(int *iterator, const char *str, char *arg, int max_arg_len, int *new_arg_len)
{
    // Clear argument
    memset(arg, 0, max_arg_len);

    // Include the null terminator to the string length
    int len = strlen(str) + 1;

    unsigned char quote = 0;
    (*new_arg_len) = 0;
    // For each character(c) in string(str)
    for ((*iterator) = (*iterator); (*iterator) < len; (*iterator)++)
    {
        char c = str[*iterator];
        // Toggle quote to ignore whitespaces when c is a quote (" or ') and continue
        if (c == 0x27 || c == 0x22)
        {
            quote = !quote;
            continue;
        }

        // Stop when we found a whitespace or null, except when we are in a quote
        if (!quote && (c == ' ' || c == 0))
        {
            // Also count iterator up by one more so we dont start with a whitespace next time we run this function.
            (*iterator)++;
            // We also got a null terminator
            (*new_arg_len)++;
            return 1;
        }

        // When everything passes, add characters to argument. Except dont when argument is too long (So we just cut it off). Also preserve null terminator
        if ((*new_arg_len) < max_arg_len - 1)
            arg[(*new_arg_len)++] = c;
    }

    return 0;
}

// Creates a newline
void cli_newline(cli_terminal_t *term)
{
    if (term->current_line < term->height - 1)
    {
        term->current_line++;
    }
    else
    {
        cli_shift_up(term);
    }

    term->current_colunn = 0;
}

void cli_put(cli_terminal_t *term, char c)
{
    if (c == '\n' || c == '\r')
    {
        cli_newline(term);
        return;
    }
    cli_set(term, term->current_colunn++, term->current_line, c);
    if (term->current_colunn >= term->width)
        cli_newline(term);
}

void cli_print(cli_terminal_t *term, const char *str)
{

    for (int i = 0; i < CLI_MAX_STR_LEN; i++)
    {
        char c = str[i];
        if (c == 0)
            break;
        cli_put(term, c);
    }
}

void cli_clear(cli_terminal_t *term)
{
    term->current_colunn = 0;
    term->current_line = 0;
    term->current_color = 0b11111111;
    // memset(term->buffer, 0, term->buffersize);
    // memset(term->colors, 0, term->buffersize);
}

unsigned char cli_execute(cli_terminal_t *term, const char *command, cli_executables_t *executables)
{
    const uint8_t maxarglen = 32;
    int amount = executables->number_of_executables;

    int iter = 0, arglen = 0;
    // char* arg = (char*)malloc(maxarglen);
    char arg[32];
    memset(arg, 0, sizeof(arg));
    // cli_print(term, command);
    cli_get_next_argument_iterative(&iter, command, arg, maxarglen, &arglen);
    for (int i = 0; i < amount; i++)
    {
        const char *name = executables->executable_names[i];
        // cli_print(term, name);
        if (!strcmp(name, arg))
        {
            // cli_print(term, "yes\n");
            // free(arg);
            return executables->executable_functions[i](term, command);
        }
    }

    // free(arg);
    return -1;
}

void cli_demo(cli_terminal_t *term)
{
    for (uint16_t y = 0; y < term->height + 5; y++)
    {
        term->current_color = 0b11100111;
        sprintf((char *)cli_line_buffer, "DEMO %d\n", y);
        cli_print(term, (char *)cli_line_buffer);
        cli_draw(term);
    }

    cli_print(term, "BYE DEMO\n");
    for (uint16_t y = 0; y < term->height; y++)
    {
        cli_shift_up(term);
        cli_draw(term);
    }
}