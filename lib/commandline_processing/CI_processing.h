#ifndef CI_PROC_H
#define CI_PROC_H

/*
This function can be repeated to find all the arguments in a string.

int* iterator takes a pointer to an iterator. The iterator is a counter that shows the next argument position. So we can run this function in repeat and advance to the next argument automatically.
const char* str is the string where to get arguments from.
char* arg is where the new argument will be written to.
int max_arg_len is the maximum size of the argument.
int* new_arg_len is where the size of the new argument we found will be written to.

Returns 0 when we reached the last argument, otherwise 1.
*/
unsigned char get_next_argument_iterative(int* iterator, const char* str, char* arg, int max_arg_len, int* new_arg_len );

#endif