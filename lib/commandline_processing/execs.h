#pragma once

#include <Arduino.h>


typedef struct 
{
    const char* name;
    int(*function)(String full_input);
    const char* description;
}
execs_function_t;

int execs_run(String full_input, execs_function_t* functions);
String execs_get_arg(String full_input, unsigned int arg);