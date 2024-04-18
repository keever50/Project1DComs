#include <hu_protocol.h>

int hu_protocol_transmit(RH_ASK* driver, hu_packet_t* packet)
{

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