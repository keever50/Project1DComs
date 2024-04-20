#include <CI_processing.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int main()
{
    char* msg = "DO 'hello there' 'this is a quote' 123 dasd, fsdk. @@$* aaa";
    char arg[64];
    char* mem_arguments[64];
    int iter=0;
    int arglen=0;
    int number_of_arguments=0;

    // Store all arguments into memory
    while(get_next_argument_iterative(&iter, msg, arg, sizeof(arg), &arglen))
    {
        mem_arguments[number_of_arguments]=malloc(arglen);
        strcpy(mem_arguments[number_of_arguments], arg);
        number_of_arguments++;
    }

    for(int i=0;i<number_of_arguments;i++)
    {
        printf("[%s]\n", mem_arguments[i]);
        free(mem_arguments[i]);
    }
    
    
    

    return 0;
}