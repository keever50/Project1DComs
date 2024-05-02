#include <hu_protocol.h>
#include <biterrors.h>

uint8_t hu_protocol_buffer[HU_PROTOCOL_BUFFER_SIZE];
char hu_protocol_tag_receive[]="PROTOCOL RECEIVE";


// Written by Marijn Boumans & Kevin Witteveen
int hu_protocol_transmit(RH_ASK* driver, hu_packet_t* packet)
{
    // Prepare transmit buffer //

    // Move start byte further away for preambles for testing only
    int offset=0;
    int i=offset;

    // Start
    hu_protocol_buffer[i++]=packet->start;
    hu_protocol_buffer[i++]=packet->length;
    hu_protocol_buffer[i++]=packet->function;
    hu_protocol_buffer[i++]=packet->source;
    hu_protocol_buffer[i++]=packet->destination;

    // Data
    int data_len = packet->length-HU_PROTOCOL_LENGTH_NON_DATA;
    for(int p=0;p<data_len;p++)
    {
        hu_protocol_buffer[i++]=packet->data[p];
    }
    
    // End
    hu_protocol_buffer[i++]=packet->end;
    
    // LRC
    packet->LRC = get_LRC(hu_protocol_buffer+offset, HU_PROTOCOL_START_LENGTH+packet->length);
    hu_protocol_buffer[i++]=packet->LRC;


    // Transmit //
    if(!driver->send(hu_protocol_buffer,HU_PROTOCOL_START_LENGTH+packet->length+1+offset)) return 1;  // Include LRC
    if(!driver->waitPacketSent(HU_PROTOCOL_TIMEOUT)) return 2;


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
    uint8_t len=HU_PROTOCOL_BUFFER_SIZE;
    
    // Look if we received a packet. Otherwise return as listening
    if(!driver->recv(hu_protocol_buffer, &len)) return HU_PROT_RECEIVE_LISTENING;
    return hu_protocol_decode( packet );

}

// Written by Kevin Witteveen
hu_prot_receive_err_t hu_protocol_decode(hu_packet_t* packet)
{

    // Find the starting position of the packet by using the start byte and apply the offset if needed.
    int i=hu_protocol_find_start_pos();
    int offset=i;
    
    // When no start byte is found, ignore this packet.
    if(i<0)
    {
        return HU_PROT_RECEIVE_IGNORE;
    }

    // Construct packet
    packet->start=hu_protocol_buffer[i++];
    packet->length=hu_protocol_buffer[i++];
    packet->function=hu_protocol_buffer[i++];
    packet->source=hu_protocol_buffer[i++];
    packet->destination=hu_protocol_buffer[i++];

    // Check whether the destination is us. Otherwise ignore this packet.
    // OUR ADDRESS FUNCTION HERE

    // Do some checks before getting the data
    // Length is too long
    if(packet->length>HU_PROTOCOL_MAX_DATA_SIZE+HU_PROTOCOL_LENGTH_NON_DATA)
    {
        Serial.print("receive packet error: packet too long. Length=");
        Serial.print(packet->length);
        Serial.println("");
        return HU_PROT_RECEIVE_TOO_LONG;
    }
    // Length is too short
    if(packet->length<HU_PROTOCOL_LENGTH_NON_DATA)
    {
        Serial.print("receive packet error: packet too short. Length=");
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
    int data_length = packet->length-HU_PROTOCOL_LENGTH_NON_DATA;
    if(data_length>0)
    {
        memcpy(packet->data, hu_protocol_buffer+i, data_length);
        i=i+data_length;
    }


    // Get the last bytes
    packet->end=hu_protocol_buffer[i++];
    packet->LRC=hu_protocol_buffer[i++];

    // Last checks
    if(packet->end!=HU_PROTOCOL_END_BYTE)
    {
        Serial.print("receive packet error: incorrect packet end [");
        Serial.print(packet->end);
        Serial.println("]");       
        return HU_PROT_RECEIVE_INCORRECT_END;
    }

    // Calculate and check whether the LRC is correct or not
    uint8_t LRC = get_LRC(hu_protocol_buffer+offset, HU_PROTOCOL_START_LENGTH+packet->length); 
    if(LRC!=packet->LRC)
    {
        Serial.print("receive packet error: Received LRC [");
        Serial.print(LRC);
        Serial.print("] but we calculated [");   
        Serial.print(packet->LRC);
        Serial.println("]");      
        return HU_PROT_RECEIVE_INCORRECT_LRC;     
    }

    return HU_PROT_RECEIVE_RECEIVED;    
}

void hu_protocol_print_packet( hu_packet_t* packet )
{
    Serial.println("----------[HU_Protocol packet]----------");

    Serial.print("  Start byte: ");
    Serial.println(packet->start);

    Serial.print("  Length: ");
    Serial.println(packet->length);

    Serial.print("  Function: ");
    Serial.println(packet->function);

    Serial.print("  Source: ");
    Serial.println(packet->source);

    Serial.print("  Destination: ");
    Serial.println(packet->destination);
 
    int data_length = packet->length-HU_PROTOCOL_LENGTH_NON_DATA;
    Serial.print("  Data: [");
    for(int i=0;i<data_length;i++)
    {
        Serial.print(packet->data[i]);
        Serial.print(",");
    }
    Serial.println("]");

    Serial.print("  end byte: ");
    Serial.println(packet->end);

    Serial.print("  LRC: ");
    Serial.println(packet->LRC);

    Serial.println("-----------------------------------------\n");
  
}


// Written by Marijn Boumans
uint8_t hu_protocol_encode_address(const char* str) {
    uint8_t classBits, groupBits, moduleBits;

    // Class
    if (strncmp(str, "EV2A", 4) == 0) classBits = 0b00;
    else if (strncmp(str, "EV2B", 4) == 0) classBits = 0b01;
    else if (strncmp(str, "EV2C", 4) == 0) classBits = 0b10;
    else if (strncmp(str, "EV2D", 4) == 0) classBits = 0b11;
    else return 0;  // Invalid class

    // Group
    if (strncmp(str + 4, "GROEP1", 6) == 0) groupBits = 0b001;
    else if (strncmp(str + 4, "GROEP2", 6) == 0) groupBits = 0b010;
    else if (strncmp(str + 4, "GROEP3", 6) == 0) groupBits = 0b011;
    else if (strncmp(str + 4, "GROEP4", 6) == 0) groupBits = 0b100;
    else if (strncmp(str + 4, "GROEP5", 6) == 0) groupBits = 0b101;
    else if (strncmp(str + 4, "GROEP6", 6) == 0) groupBits = 0b110;
    else if (strncmp(str + 4, "GROEP7", 6) == 0) groupBits = 0b111;
    else return 0;  // Invalid group

    // Module
    if (strncmp(str + 10, "MM", 8) == 0) moduleBits = 0b000;
    else if (strncmp(str + 10, "CM", 8) == 0) moduleBits = 0b001;
    else if (strncmp(str + 10, "NM", 8) == 0) moduleBits = 0b010;
    else if (strncmp(str + 10, "CO", 8) == 0) moduleBits = 0b011;
    else if (strncmp(str + 10, "TM", 8) == 0) moduleBits = 0b100;
    else return 0;  // Invalid module

    return classBits | (groupBits << 2) | (moduleBits << 5);
    
}

