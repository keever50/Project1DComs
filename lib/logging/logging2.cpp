// Written by Kevin Witteveen
#include <Arduino.h>
#include <logging2.h>
#include <stdio.h>
#include <stdlib.h>


/*
    This function can be customized to output logging.
    Called by log2_log_output, but can also be used to directly output messages without formatting. Also ignoring level
*/
void log2_log_raw( const char* msg )
{
    //char msg_case[64];
    //strlcpy(msg_case, msg, 64);
    //strupr(msg_case);

    Serial.print(msg);
    
}

void log2_log(int level, const char* tag, const char* str, ...)
{
    static char buff[LOG2_MAX_CHARS_PER_LOG];
    static char output_buff[LOG2_MAX_CHARS_PER_LOG];
    /*Do not log messages under log_level*/
    if(level<LOG2_LOG_LEVEL) return;

    /*Empty buffer*/
    for(int i=0;i<LOG2_MAX_CHARS_PER_LOG;i++)
    {
        buff[i]=0;
    }


    switch(level)
    {
        case LOG2_DEBUG:
            strlcat(buff, "D", LOG2_MAX_CHARS_PER_LOG);
            break;
        case LOG2_INFO:
            strlcat(buff, "I", LOG2_MAX_CHARS_PER_LOG);
            break;
        case LOG2_WARNING:
            strlcat(buff, "WARN", LOG2_MAX_CHARS_PER_LOG);
            break;
        case LOG2_ERROR:
            strlcat(buff, "ERRO", LOG2_MAX_CHARS_PER_LOG);
            break;
        case LOG2_CRITICAL:
            strlcat(buff, "CRIT", LOG2_MAX_CHARS_PER_LOG);
            break;  
        default:
            strlcat(buff, "????", LOG2_MAX_CHARS_PER_LOG);
            break;                                                           
    }
    strlcat(buff, "_", LOG2_MAX_CHARS_PER_LOG);

    /*Add tag*/
    strlcat(buff, tag, LOG2_MAX_CHARS_PER_LOG);
    /*Spacing*/
    strlcat(buff, ":", LOG2_MAX_CHARS_PER_LOG);
    /*Message*/
    strlcat(buff, str, LOG2_MAX_CHARS_PER_LOG);
    
    /*Format arguments*/
    va_list args;
    va_start(args, str);
    vsnprintf(output_buff, LOG2_MAX_CHARS_PER_LOG, buff, args);
    
    va_end(args);

    /*Newline*/
    strlcat(output_buff, "\n", LOG2_MAX_CHARS_PER_LOG);

    /*Output*/
    log2_log_raw( output_buff );
}


void log2_log_raw_printf(const char* str, ...)
{
    static char output_buff[LOG2_MAX_CHARS_PER_LOG];
    /*Empty buffer*/
    for(int i=0;i<LOG2_MAX_CHARS_PER_LOG;i++)
    {
        output_buff[i]=0;
    }

    /*Format arguments*/
    va_list args;
    va_start(args, str);
    vsnprintf(output_buff, LOG2_MAX_CHARS_PER_LOG, str, args);
    va_end(args);

    /*Output*/
    log2_log_raw( output_buff );
}