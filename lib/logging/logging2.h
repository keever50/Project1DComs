// Written by Kevin Witteveen

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*You can select the logging level by defining LOG2_LOG_LEVEL_OVERRIDE and then select your level. Such as LOG2_xxx (example LOG2_DEBUG)*/
#ifndef LOG2_LOG_LEVEL_OVERRIDE
#define LOG2_LOG_LEVEL  0
#endif

#define LOG2_MAX_CHARS_PER_LOG  128

/**Pokes code with stick* Does it hurt when I do this?*/
#define LOG2_DEBUG      0
/*Just FYI*/
#define LOG2_INFO       1
/*Could be a problem if you do that, maybe not, worth noting*/
#define LOG2_WARNING    2
/*Able to recover, but problematic*/
#define LOG2_ERROR      3
/*Unrecoverable. Might blow up later idk*/
#define LOG2_CRITICAL   4

/*
    This function can be customized to output logging.
    Called by log2_log_output, but can also be used to directly output messages without formatting. Also ignoring error level.
    
    const char* msg contains the logging message, which can include a newline in case of normal logging.
*/
void log2_log_raw( const char* msg ) __attribute__((weak));

/*
    This function takes:
    int level: can be one of the error level defines LOG2_XXX. As example LOG2_DEBUG.
    const char* tag: This is the string printed before printing the message. Can be used to show where the message comes from.
    const char* str: the actual message. Takes printf modifiers such as %d %f %c etc
    Anything after can be variables, like in printf.
    Example log2_log(LOG2_ERROR, program_name_string, "It's over %d!", 9000);
*/
void log2_log(int level, const char* tag, const char* str, ...);

/*
    This function works like printf and bypasses all the extra logging features
*/
void log2_log_raw_printf(const char* str, ...);

#ifdef __cplusplus
}
#endif