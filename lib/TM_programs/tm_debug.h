#pragma once

#include <Arduino.h>
#include <tm_io.h>
#include <tm_rf.h>

int tm_exec_receiver_open( String full_command );
int tm_exec_scrambler_open( String full_command );
int tm_exec_packet_generator_open( String full_command );
int tm_exec_packet_send_open( String full_command );
int tm_exec_packet_data_open( String full_command );
int tm_exec_packet_getf( String full_command );
int tm_exec_packet_view( String full_command );