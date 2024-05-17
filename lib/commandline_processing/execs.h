#pragma once

#include <Arduino.h>


typedef struct 
{
    String name;
    int(*function)(String full_input);
    String description;
}
execs_function_t;

int execs_run(String full_input, execs_function_t* functions);
String execs_get_arg(String full_input, unsigned int arg);