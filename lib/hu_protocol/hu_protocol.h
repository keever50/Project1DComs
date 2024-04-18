#pragma once
#include <Arduino.h>
#include <RH_ASK.h>

typedef struct hu_packet_s
{
    uint8_t start;
    uint8_t length;
    uint8_t function;
    uint8_t source;
    uint8_t destination;
    uint8_t* data;
    uint8_t end;
    uint8_t LRC;
}hu_packet_t;

int hu_protocol_receive(RH_ASK* driver, hu_packet_t* packet);
int hu_protocol_transmit(RH_ASK* driver, hu_packet_t* packet);

// Calculates the length of a packet and returns the value
int hu_protocol_calculate_length(hu_packet_t* packet);

// Checks the LRC of a packet and returns 1 on error
int hu_protocol_calculate_LRC(hu_packet_t* packet);
