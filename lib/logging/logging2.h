// Written by Kevin Witteveen

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define LOG2_LOG_LEVEL  0

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

/*Called by log2_log_output, but can also be used to directly output messages without formatting. Also ignoring level.*/
void log2_log_raw( const char* msg );
void log2_log(int level, const char* tag, const char* str, ...);
void log2_log_raw_printf(const char* str, ...);

#ifdef __cplusplus
}
#endif