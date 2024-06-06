#pragma once

#include <Arduino.h>
#include <tm_io.h>
#include <tm_rf.h>

#define TM_TRANSFLOW_RECEIVE_RETRIES        3
#define TM_TRANSFLOW_RECEIVE_TIMEOUT    2000
#define TM_TRANSFLOW_RECEIVE_TIMEOUT_ERR    1
#define TM_TRANSFLOW_RECEIVE_CORRUPT_ERR    2
// Public command line executable functions

int tm_exec_receiver_open( String full_command );
int tm_exec_scrambler_open( String full_command );
int tm_exec_packet_generator_open( String full_command );
int tm_exec_packet_send_open( String full_command );
int tm_exec_packet_send_full_open( String full_command );
int tm_exec_packet_data_open( String full_command );
int tm_exec_packet_getf( String full_command );
int tm_exec_packet_view( String full_command );