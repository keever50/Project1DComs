#ifdef TEST_RECEIVE

#include <Arduino.h>
#include <RH_ASK.h>
#include <hu_protocol.h>

/****************
     RADIO
****************/
// Pins
#define RA_BITRATE          500
#define RA_RX               3
#define RA_TX               5
#define RA_TR               2

RH_ASK driver(RA_BITRATE, RA_RX, RA_TX, RA_TR, false);


void setup()
{
    Serial.begin(9600);
    bool success = driver.init();
    if(!success)
    {
        Serial.println("init fail");
        delay(500);
        while(true);
    }
    //pinMode(3, INPUT);


}


void loop()
{
    hu_packet_t packet;
    hu_prot_receive_err_t err = hu_protocol_receive( &driver, &packet );
    if(err == HU_PROT_RECEIVE_RECEIVED)
    {
        hu_protocol_print_packet(&packet);
    }

    delay(100);
}

#endif