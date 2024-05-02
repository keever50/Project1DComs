#ifdef TRANSMIT_TEST

#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#include <hu_protocol.h>

#define RA_BITRATE          500
#define RA_RX               3
#define RA_TX               5
#define RA_TR               2
RH_ASK driver(RA_BITRATE,RA_RX,RA_TX,RA_TR,false);

void setup()
{
    randomSeed(A1);
    Serial.begin(9600);	  // Debugging only
    driver.init();
}

void loop()
{
    hu_packet_t packet;
    packet.start=HU_PROTOCOL_START_BYTE;
    packet.function=0x04;
    packet.source=0x01;
    packet.destination=0x02;
    packet.length=HU_PROTOCOL_LENGTH_NON_DATA+2; // Do not include LRC in length
    packet.data[0]=0xAA;
    packet.data[1]=0xBB;
    packet.end=HU_PROTOCOL_END_BYTE;
    hu_protocol_transmit(&driver, &packet);
    delay(8000);
}

#endif