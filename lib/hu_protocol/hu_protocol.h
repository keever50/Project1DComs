#pragma once
#include <Arduino.h>
#include <RH_ASK.h>

#define HU_PROTOCOL_START_BYTE      0x01
#define HU_PROTOCOL_END_BYTE        0x04
#define HU_PROTOCOL_MAX_PACKET_SIZE 0x28
#define HU_PROTOCOL_MAX_DATA_SIZE   36
#define HU_PROTOCOL_FUNCTION_RANGE  13
#define HU_PROTOCOL_MIN_PACKET_LEN  7

#define HU_PROTOCOL_BUFFER_SIZE RH_ASK_MAX_MESSAGE_LEN
uint8_t hu_protocol_buffer[HU_PROTOCOL_BUFFER_SIZE];

typedef struct hu_packet_s
{
    uint8_t start;
    uint8_t length;
    uint8_t function;
    uint8_t source;
    uint8_t destination;
    uint8_t data[HU_PROTOCOL_MAX_DATA_SIZE];
    uint8_t end;
    uint8_t LRC;
}hu_packet_t;

/*
    Checks if we received a packet.
    Returns 0 on we received a packet correctly,
    returns 1 on we received nothing,
    returns 2 on we received a corrupted packet

*/
int hu_protocol_receive(RH_ASK* driver, hu_packet_t* packet, int buff_length );
int hu_protocol_transmit(RH_ASK* driver, hu_packet_t* packet);

// Calculates the length of a packet and returns the value
int hu_protocol_calculate_length(hu_packet_t* packet);

// Checks the LRC of a packet and returns 1 on error
int hu_protocol_calculate_LRC(hu_packet_t* packet);
