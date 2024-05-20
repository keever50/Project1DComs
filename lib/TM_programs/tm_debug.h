#pragma once

#include <Arduino.h>
#include <tm_io.h>
#include <tm_rf.h>
int tm_exec_receiver_open( String full_command );
int tm_exec_scrambler_open( String full_command );
int tm_exec_packet_generator_open( String full_command );