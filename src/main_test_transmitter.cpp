#ifdef TRANSMIT_TEST

#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#include <hu_protocol.h>

#define RA_BITRATE          500
#define RA_RX               10
#define RA_TX               11
#define RA_TR               2
RH_ASK driver(RA_BITRATE,RA_RX,RA_TX,RA_TR,false);

void setup()
{
    randomSeed(A1);
    Serial.begin(9600);	  // Debugging only
    driver.init();
}

hu_packet_t packet;
void loop()
{
    
    packet.start=HU_PROTOCOL_START_BYTE;
    packet.function=0x04;
    packet.source=hu_protocol_encode_address("2AG2MM");
    packet.destination=hu_protocol_encode_address("2AG2TM");
    packet.length=HU_PROTOCOL_LENGTH_NON_DATA+HU_PROTOCOL_MAX_DATA_SIZE; // Do not include LRC in length
    for(int i=0;i<HU_PROTOCOL_MAX_DATA_SIZE;i++)
    {
        packet.data[i]=(uint8_t)rand();
    }

    packet.end=HU_PROTOCOL_END_BYTE;
    hu_protocol_transmit(&driver, &packet);
    delay(8000);
}

#endif