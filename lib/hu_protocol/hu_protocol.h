

#pragma once

#ifdef TEST_MODULE
#include <RH_ASK.h>
#else
#include <RH_ASK.h>
#endif

// Written by Kevin Witteveen
#define HU_PROTOCOL_START_BYTE      0x01 // Starting byte
#define HU_PROTOCOL_END_BYTE        0x04 // Ending byte
#define HU_PROTOCOL_MAX_DATA_SIZE   37 // Most data a packet can send
#define HU_PROTOCOL_FUNCTION_RANGE  13 // Highest function we can have
#define HU_PROTOCOL_MIN_PACKET_LEN  7 // Minimal amount of total bytes the smallest packet can have
#define HU_PROTOCOL_START_LENGTH    2 // Bytes before length
#define HU_PROTOCOL_END_LENGTH      2 // Remaining bytes after the data
#define HU_PROTOCOL_LENGTH_NON_DATA 4 // The bytes in length that are not data
#define HU_PROTOCOL_MAX_PACKET_SIZE (HU_PROTOCOL_MIN_PACKET_LEN+HU_PROTOCOL_MAX_DATA_SIZE) // Maximum total size of a packet
#define HU_PROTOCOL_BUFFER_SIZE RH_ASK_MAX_MESSAGE_LEN // Size of the working buffer. Do not change

#define HU_PROTOCOL_FUNCTION_OPVRAAG_MEETWAARDES    0x05
#define HU_PROTOCOL_FUNTION_OVERDRAGEN_MEETWAARDES    0x06
#define HU_PROTOCOL_TIMEOUT 500



// Written by Kevin Witteveen
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
    uint8_t length; // Aka remaining bytes after this until LRC. (Start, length and LRC are not included)
    uint8_t function;
    uint8_t source;
    uint8_t destination;
    uint8_t data[HU_PROTOCOL_MAX_DATA_SIZE];
    uint8_t end;
    uint8_t LRC;
}hu_packet_t;
//


// Written by Hayan Rafee
struct hu_protocol_SensorReadings {// selectie floats in een bepaalde orde 
  float avgTemp;
  float avgLDR;
  float avgPot;
  float maxTemp;
  float maxLDR;
  float maxPot;
  float minTemp;
  float minLDR;
  float minPot;
};
void hu_protocol_MM_structToByteArray(const hu_protocol_SensorReadings &readings, byte* byteArray);
void hu_protocol_MM_checkByteArray(byte* byteArray);
//


// Written by Kevin Witteveen
// Receives and decodes a packet. Returns hu_prot_receive_err_t. Check this enum for errors.
hu_prot_receive_err_t hu_protocol_receive(RH_ASK* driver, hu_packet_t* packet);
// Decodes a packet. Returns hu_prot_receive_err_t. Check this enum for errors.
hu_prot_receive_err_t hu_protocol_decode(hu_packet_t* packet);
void hu_protocol_decode_raw(hu_packet_t* packet, uint8_t* buffer );

// Returns 0 on success
int hu_protocol_transmit(RH_ASK* driver, hu_packet_t* packet);
// Prints the packet
void hu_protocol_print_packet( hu_packet_t* packet );

// Sets the global address
void hu_protocol_set_address( uint8_t addr );
// Gets the global address
uint8_t hu_protocol_get_address();

// Written by Marijn Boumans
uint8_t hu_protocol_encode_address(const char* str);


