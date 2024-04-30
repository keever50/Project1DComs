#include <hu_protocol.h>
#include <Arduino.h>
#include <RH_ASK.h>

RH_ASK rh_ask(500, 3, 11, 0, false); // Bitrate, receive pin, transmit pin, select pin(unused), select inverse(unused)

void setup()
{
    Serial.begin(9600);
    rh_ask.init();
}

void loop()
{
    Serial.println("Receiving...");

    // The function hu_protocol_receive will fill this packet in.
    hu_packet_t packet;

    /*Check if we received data, otherwise it returns HU_PROT_RECEIVE_LISTENING.
    It can also return HU_PROT_RECEIVE_IGNORE in case the packet was not for us.*/
    hu_prot_receive_err_t err = hu_protocol_receive( &rh_ask, &packet );
    if(err != HU_PROT_RECEIVE_IGNORE && err != HU_PROT_RECEIVE_LISTENING)
    {
        // Print out the packet on a pretty way
        hu_protocol_print_packet(&packet);
    }    

    // You can extract variables this way
    uint8_t function = packet.function;
    /*Ofcourse arrays as well. Make sure you dont read more than "packet.length-HU_PROTOCOL_MIN_PACKET_LEN"
    This is the total package length we received and we subtract the minimum size of the packet as if there is no data. This way you know the data length.*/
    uint8_t data_length = packet.length-HU_PROTOCOL_MIN_PACKET_LEN;
    uint8_t* data = packet.data;


    // Wait a little
    delay(500);
}
