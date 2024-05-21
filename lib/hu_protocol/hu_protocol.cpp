#include <hu_protocol.h>
#include <biterrors.h>

uint8_t hu_protocol_address=0;

uint8_t hu_protocol_buffer[HU_PROTOCOL_BUFFER_SIZE];
char hu_protocol_tag_receive[]="PROTOCOL RECEIVE";

void hu_protocol_set_address( uint8_t addr )
{
    hu_protocol_address=addr;    
}


uint8_t hu_protocol_get_address()
{
    return hu_protocol_address;
}

// Written by Marijn Boumans & Kevin Witteveen
int hu_protocol_transmit(RH_ASK* driver, hu_packet_t* packet)
{
    // Prepare transmit buffer //

    // Move start byte further away for preambles for testing only
    int offset=0;
    int i=0;

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
    if(packet->destination!=hu_protocol_address)
    {
        return HU_PROT_RECEIVE_IGNORE;
    }

    // Do some checks before getting the data
    // Length is too long
    if(packet->length>HU_PROTOCOL_MAX_DATA_SIZE+HU_PROTOCOL_LENGTH_NON_DATA)
    {
        Serial.print(F("receive packet error: packet too long. Length="));
        Serial.print(packet->length);
        Serial.println("");
        return HU_PROT_RECEIVE_TOO_LONG;
    }
    // Length is too short
    if(packet->length<HU_PROTOCOL_LENGTH_NON_DATA)
    {
        Serial.print(F("receive packet error: packet too short. Length="));
        Serial.print(packet->length);
        Serial.println("");
        return HU_PROT_RECEIVE_TOO_LONG;
    }


    if(packet->function>HU_PROTOCOL_FUNCTION_RANGE)
    {
        Serial.print(F("receive packet error: unknown function ["));
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
        Serial.print(F("receive packet error: incorrect packet end ["));
        Serial.print(packet->end);
        Serial.println("]");       
        return HU_PROT_RECEIVE_INCORRECT_END;
    }

    // Calculate and check whether the LRC is correct or not
    uint8_t LRC = get_LRC(hu_protocol_buffer+offset, HU_PROTOCOL_START_LENGTH+packet->length); 
    if(LRC!=packet->LRC)
    {
        Serial.print(F("receive packet error: Received LRC ["));
        Serial.print(LRC);
        Serial.print(F("] but we calculated ["));   
        Serial.print(packet->LRC);
        Serial.println(F("]"));      
        return HU_PROT_RECEIVE_INCORRECT_LRC;     
    }

    return HU_PROT_RECEIVE_RECEIVED;    
}

// Written by Kevin Witteveen
void hu_protocol_decode_raw(hu_packet_t* packet, uint8_t* buffer )
{

    // Find the starting position of the packet by using the start byte and apply the offset if needed.
    //int i=hu_protocol_find_start_pos();
    //int offset=i;
    int i=0;

    // Construct packet
    packet->start=buffer[i++];
    packet->length=buffer[i++];
    packet->function=buffer[i++];
    packet->source=buffer[i++];
    packet->destination=buffer[i++];


    // Do some checks before getting the data
    // Length is too long
    int safelen=packet->length;
    if(packet->length>HU_PROTOCOL_MAX_DATA_SIZE+HU_PROTOCOL_LENGTH_NON_DATA)
    {
        safelen=HU_PROTOCOL_MAX_DATA_SIZE;
    }
    // Length is too short
    if(packet->length<HU_PROTOCOL_LENGTH_NON_DATA)
    {
        safelen=HU_PROTOCOL_LENGTH_NON_DATA;
    }


    // Transfer the data. This transfers nothing when length is the minimum packet length.
    int data_length = safelen-HU_PROTOCOL_LENGTH_NON_DATA;
    if(data_length>0)
    {
        memcpy(packet->data, buffer+i, data_length);
        i=i+data_length;
    }


    // Get the last bytes
    packet->end=buffer[i++];
    packet->LRC=buffer[i++];

}

void hu_protocol_print_packet( hu_packet_t* packet )
{
    Serial.println(F("----------[HU_Protocol packet]----------"));

    Serial.print(F("  Start byte: "));
    Serial.println(packet->start);

    Serial.print(F("  Length: "));
    Serial.println(packet->length);

    Serial.print(F("  Function: "));
    Serial.println(packet->function);

    Serial.print(F("  Source: "));
    Serial.println(packet->source);

    Serial.print(F("  Destination: "));
    Serial.println(packet->destination);
 
    int data_length = packet->length-HU_PROTOCOL_LENGTH_NON_DATA;
    Serial.print(F("  Data: ["));
    for(int i=0;i<data_length;i++)
    {
        Serial.print(packet->data[i]);
        Serial.print(F(","));
    }
    Serial.println(F("]"));

    Serial.print(F("  end byte: "));
    Serial.println(packet->end);

    Serial.print(F("  LRC: "));
    Serial.println(packet->LRC);

    Serial.println(F("-----------------------------------------\n"));
  
}


// Written by Marijn Boumans
uint8_t hu_protocol_encode_address(const char* str) {
    uint8_t classBits, groupBits, moduleBits;

    // Class
    if (strncmp(str, "2A", 2) == 0) classBits = 0b00;
    else if (strncmp(str, "2B", 2) == 0) classBits = 0b01;
    else if (strncmp(str, "2C", 2) == 0) classBits = 0b10;
    else if (strncmp(str, "2D", 2) == 0) classBits = 0b11;
    else return 0;  // Invalid class

    // Group
    if (strncmp(str + 2, "G1", 2) == 0) groupBits = 0b001;
    else if (strncmp(str + 2, "G2", 2) == 0) groupBits = 0b010;
    else if (strncmp(str + 2, "G3", 2) == 0) groupBits = 0b011;
    else if (strncmp(str + 2, "G4", 2) == 0) groupBits = 0b100;
    else if (strncmp(str + 2, "G5", 2) == 0) groupBits = 0b101;
    else if (strncmp(str + 2, "G6", 2) == 0) groupBits = 0b110;
    else if (strncmp(str + 2, "G7", 2) == 0) groupBits = 0b111;
    else return 0;  // Invalid group
    
    // Module
    if (strncmp(str + 4, "MM", 2) == 0) moduleBits = 0b000;
    else if (strncmp(str + 4, "CM", 2) == 0) moduleBits = 0b001;
    else if (strncmp(str + 4, "NM", 2) == 0) moduleBits = 0b010;
    else if (strncmp(str + 4, "CO", 2) == 0) moduleBits = 0b011;
    else if (strncmp(str + 4, "TM", 2) == 0) moduleBits = 0b100;
    else return 0;  // Invalid module

    return classBits | (groupBits << 2) | (moduleBits << 5);
    
}

// Written by Hayan Rafee
void hu_protocol_MM_structToByteArray(const hu_protocol_SensorReadings &readings, byte* byteArray) 
{
  byte* ptr = (byte*)&readings;               // Convert float values to byte arrays
  for (uint8_t i = 0; i < sizeof(hu_protocol_SensorReadings); i++) 
  {
    byteArray[i] = *ptr++;
  }
  Serial.println("Array of bytes:"); // Print the array of bytes
  for (uint8_t i = 0; i < sizeof(hu_protocol_SensorReadings); i++) 
  {
    Serial.println(byteArray[i], BIN);//binair
  }
  Serial.println();
  Serial.println();
}

void hu_protocol_MM_checkByteArray(byte* byteArray) {     //check function of het omzetten goed is gegaan
  // Retrieve the bytes from the byteArray    // de bytes array is pakket - (SoS + sours + dest + PL + LRC + FC)      
  hu_protocol_SensorReadings readings;
  byte* ptr = (byte*)&readings;
  for (uint8_t i = 0; i < sizeof(hu_protocol_SensorReadings); i++) {
    *ptr++ = byteArray[i];
  }
  // Print the converted floats
  Serial.println("Converted floats from bytes:");
  Serial.print("Max Temp: ");
  Serial.println(readings.maxTemp);
  Serial.print("Min Temp: ");
  Serial.println(readings.minTemp);
  Serial.print("Avg Temp: ");
  Serial.println(readings.avgTemp);
  Serial.print("Max LDR: ");
  Serial.println(readings.maxLDR);
  Serial.print("Min LDR: ");
  Serial.println(readings.minLDR);
  Serial.print("Avg LDR: ");
  Serial.println(readings.avgLDR);
  Serial.print("Max Pot: ");
  Serial.println(readings.maxPot);
  Serial.print("Min Pot: ");
  Serial.println(readings.minPot);
  Serial.print("Avg Pot: ");
  Serial.println(readings.avgPot);
}  //check of je de exact values krijgt met 4 signifcante cijfers.



//Written by Hayan: union coverter function

//make the variables if not already done
union SensorUnion {
struct SensorReadings 
{
  float avgTemp; 
  float avgLDR; 
  float avgPot ;
  float maxTemp ;
  float maxLDR ;
  float maxPot ;
  float minTemp ; 
  float minLDR ;
  float minPot;
}sensorReadings;
byte byteArray[sizeof(sensorReadings)];  //the array of the converted bytes
};

void printUnionBytes(SensorUnion &sensorUnion) 
{
  int structSize = sizeof(sensorUnion.byteArray);
  
  Serial.print("Bytes: ");
  for (int i = 0; i < structSize; ++i) 
  {
    if (sensorUnion.byteArray[i] < 16) 
    { // Ensures two-digit format
      Serial.print("0");//debug
    }
    Serial.print(sensorUnion.byteArray[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
} //gain access to the bytes using byteArray


void setup00() {
  Serial.begin(9600);
  SensorUnion sensorUnion = 
  {
    .sensorReadings = 
    {
      6.40, 329.30, 324.20,
      9.32, 367.00, 340.00,
      4.22, 306.00, 305.00
    } //test values//
  };
  printUnionBytes(sensorUnion); //call in loop with an empty struct
}



