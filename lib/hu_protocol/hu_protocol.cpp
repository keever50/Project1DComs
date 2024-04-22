#include <hu_protocol.h>

int hu_protocol_transmit(RH_ASK* driver, hu_packet_t* packet)
{
    // Fix this. Use waitPacketSent() before sending more data.
    if(!driver->send(&packet->start,1)) return 1;
    if(!driver->send(&packet->length,1)) return 2;
    if(!driver->send(&packet->function,1)) return 3;
    if(!driver->send(&packet->source,1)) return 4;
    if(!driver->send(&packet->destination,1)) return 5;
    if(!driver->send(packet->data, packet->length)) return 6;
    if(!driver->send(&packet->end,1)) return 7;
    if(!driver->send(&packet->LRC,1)) return 8;
    
}

int hu_protocol_receive(RH_ASK* driver, hu_packet_t* packet, int buff_length)
{
    uint8_t len;
    if(!driver->recv(hu_protocol_buffer, &len)) return 1;
    
    int i=0;
    // Search for start byte
    for(int pos=0;pos<HU_PROTOCOL_BUFFER_SIZE;pos++)
    {

    }

    // Construct packet
    packet->start=hu_protocol_buffer[i++];
    packet->length=hu_protocol_buffer[i++];
    packet->function=hu_protocol_buffer[i++];
    packet->source=hu_protocol_buffer[i++];
    packet->destination=hu_protocol_buffer[i++];

    // Do some checks before getting the data


}

int hu_protocol_calculate_LRC(hu_packet_t* packet)
{
    return 0;
}