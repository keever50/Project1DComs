#include <hu_protocol.h>

int hu_protocol_transmit(RH_ASK* driver, hu_packet_t* packet)
{
    // Fix this. Use waitPacketSent() before sending more data.
    if(!driver->send(&packet->start,1)) return 1;
    if(!driver->waitPacketSent()) return 11;
    if(!driver->send(&packet->length,1)) return 2;
    if(!driver->waitPacketSent()) return 12;
    if(!driver->send(&packet->function,1)) return 3;
    if(!driver->waitPacketSent()) return 13;
    if(!driver->send(&packet->source,1)) return 4;
    if(!driver->waitPacketSent()) return 14;
    if(!driver->send(&packet->destination,1)) return 5;
    if(!driver->waitPacketSent()) return 15;
    if(!driver->send(packet->data, packet->length)) return 6;
    if(!driver->waitPacketSent()) return 16;
    if(!driver->send(&packet->end,1)) return 7;
    if(!driver->waitPacketSent()) return 17;
    if(!driver->send(&packet->LRC,1)) return 8;
    if(!driver->waitPacketSent()) return 18;
}

int hu_protocol_receive(RH_ASK* driver, hu_packet_t* packet, int buff_length)
{
    uint8_t len;
    if(!driver->recv(hu_protocol_buffer, &len)) return 1;
    
    // Search for the start byte
    

}

int hu_protocol_calculate_LRC(hu_packet_t* packet)
{
    return 0;
}