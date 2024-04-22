#include <CI_processing.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



unsigned char get_next_argument_iterative(int* iterator, const char* str, char* arg, int max_arg_len, int* new_arg_len )
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