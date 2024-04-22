#ifdef PROTOCOL_TEST

#include <Arduino.h>
#include <hu_protocol.h>
#include <RH_ASK.h>

#define PACKET_DATA_MAX_LENGTH  64

RH_ASK radio(1000, 3, 11, 0, false);
hu_packet_t packet;


void setup()
{
    Serial.begin(9600);


}

void loop()
{
    int status = hu_protocol_receive(&radio, &packet, PACKET_DATA_MAX_LENGTH);
    // status is a fail when greater than 1. 0 means we received something, 1 means we received nothing and else its an error
    if(status>1)
    {
        Serial.print("Radio receive error "); Serial.print(status); Serial.println("");
    }
    delay(100);
}

#endif