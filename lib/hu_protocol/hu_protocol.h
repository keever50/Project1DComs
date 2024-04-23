// Written by Kevin Witteveen

#pragma once
#include <Arduino.h>
#include <RH_ASK.h>


#define HU_PROTOCOL_START_BYTE      0x01
#define HU_PROTOCOL_END_BYTE        0x04
#define HU_PROTOCOL_MAX_DATA_SIZE   36
#define HU_PROTOCOL_FUNCTION_RANGE  13
#define HU_PROTOCOL_MIN_PACKET_LEN  7
#define HU_PROTOCOL_START_LENGTH    5
#define HU_PROTOCOL_END_LENGTH      2
#define HU_PROTOCOL_MAX_PACKET_SIZE (HU_PROTOCOL_MIN_PACKET_LEN+HU_PROTOCOL_MAX_DATA_SIZE)


#define HU_PROTOCOL_TIMEOUT 500
#define HU_PROTOCOL_BUFFER_SIZE RH_ASK_MAX_MESSAGE_LEN


typedef enum hu_prot_receive_err_e
{
    HU_PROT_RECEIVE_RECEIVED=0,             // We received a packet
    HU_PROT_RECEIVE_LISTENING=1,            // We received nothing yet 
    HU_PROT_RECEIVE_IGNORE=3,               // The packet was probably not for us (Destination or start byte did not match)
    HU_PROT_RECEIVE_TOO_LONG=4,             // The packet length byte was too long
    HU_PROT_RECEIVE_UNKNOWN_FUNCTION=5,     // The function does not exist
    HU_PROT_RECEIVE_INCORRECT_END=6,        // This packet ends with the wrong byte
    HU_PROT_RECEIVE_INCORRECT_LRC=7         // The packet did not pass the LRC error check. It is corrupted.
}hu_prot_receive_err_t;

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

// Receives and decodes a packet. Returns hu_prot_receive_err_t. Check this enum for errors.
hu_prot_receive_err_t hu_protocol_receive(RH_ASK* driver, hu_packet_t* packet);
// Decodes a packet. Returns hu_prot_receive_err_t. Check this enum for errors.
hu_prot_receive_err_t hu_protocol_decode(hu_packet_t* packet);

int hu_protocol_transmit(RH_ASK* driver, hu_packet_t* packet);

void hu_protocol_print_packet( hu_packet_t* packet );
