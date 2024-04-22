#include <hu_protocol.h>
#include <biterrors.h>

uint8_t hu_protocol_buffer[HU_PROTOCOL_BUFFER_SIZE];

int hu_protocol_transmit(RH_ASK* driver, hu_packet_t* packet)
{

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

    return 0;
}

// Returns the start position of the packet by finding start byte. Otherwise return -1 if no start is found.
int hu_protocol_find_start_pos()
{
    // Search for start byte
    for(int pos=0;pos<HU_PROTOCOL_BUFFER_SIZE-HU_PROTOCOL_MAX_PACKET_SIZE;pos++)
    {
        if(hu_protocol_buffer[pos]==HU_PROTOCOL_START_BYTE)
        {
            return pos;
        }
    }
    return -1;
}

// Written by Kevin Witteveen
hu_prot_receive_err_t hu_protocol_receive(RH_ASK* driver, hu_packet_t* packet)
{
    uint8_t len;
    // Look if we received a packet. Otherwise return as listening
    if(!driver->recv(hu_protocol_buffer, &len)) return HU_PROT_RECEIVE_LISTENING;
    
    return hu_protocol_decode( packet );

}

hu_prot_receive_err_t hu_protocol_decode(hu_packet_t* packet)
{

    int i=hu_protocol_find_start_pos();
    // When no start position is found, ignore this packet.
    if(i<0){return HU_PROT_RECEIVE_IGNORE;}

    // Construct packet
    packet->start=hu_protocol_buffer[i++];
    packet->length=hu_protocol_buffer[i++];
    packet->function=hu_protocol_buffer[i++];
    packet->source=hu_protocol_buffer[i++];
    packet->destination=hu_protocol_buffer[i++];

    // Check whether the destination is us. Otherwise ignore this packet.
    // OUR ADDRESS FUNCTION HERE

    // Do some checks before getting the data
    if(packet->length>HU_PROTOCOL_MAX_PACKET_SIZE)
    {
        Serial.println("receive packet error: packet too long. Length=");
        Serial.print(packet->length);
        Serial.println("");
        return HU_PROT_RECEIVE_TOO_LONG;
    }

    if(packet->function>HU_PROTOCOL_FUNCTION_RANGE)
    {
        Serial.print("receive packet error: unknown function [");
        Serial.print(packet->function);
        Serial.println("]");
        return HU_PROT_RECEIVE_UNKNOWN_FUNCTION;
    }

    // Transfer the data. This transfers nothing when length is the minimum packet length.
    int data_length = packet->length-HU_PROTOCOL_MIN_PACKET_LEN;
    if(data_length>0)
    {
        memcpy(packet->data, hu_protocol_buffer+i, data_length);
        i=i+data_length;
    }


    // Get the last bytes
    packet->end=hu_protocol_buffer[i++];
    packet->LRC=hu_protocol_buffer[i++];

    // Last checks
    if(packet->end==HU_PROTOCOL_END_BYTE)
    {
        Serial.print("receive packet error: incorrect packet end [");
        Serial.print(packet->end);
        Serial.println("]");       
        return HU_PROT_RECEIVE_INCORRECT_END;
    }

    // Calculate and check whether the LRC is correct or not
    uint8_t LRC = get_LRC((uint8_t*)packet, packet->length-1);
    if(LRC!=packet->LRC)
    {
        Serial.print("receive packet error: incorrect LRC [");
        Serial.print(LRC);
        Serial.print("] should be [");   
        Serial.print(packet->LRC);
        Serial.println("]");      
        return HU_PROT_RECEIVE_INCORRECT_LRC;     
    }

    return HU_PROT_RECEIVE_RECEIVED;    
}