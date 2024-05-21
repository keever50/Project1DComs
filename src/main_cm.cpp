#ifdef TEST_CM
#include <Arduino.h>
#define CM_Groep3_Adres 0x2C

RH_ASK rh_ask(500, 3, 11, 0, false); // Bitrate, receive pin, transmit pin, select pin(unused), select inverse(unused)


void setup()
{
Serial.begin(9600);
// Serial.print("Test");
rh_ask.init();
}

void loop()
{
    Serial.print("Test_1");
    hu_packet_t packet;

    hu_prot_receive_err_t err = hu_protocol_receive( &rh_ask, &packet );
    if(err != HU_PROT_RECEIVE_IGNORE && err != HU_PROT_RECEIVE_LISTENING)
    {
        hu_protocol_print_packet(&packet);
    } 
    uint8_t Source = 0x14;   //packet.function;
    uint8_t Functie = 0x01;  //packet.function;
    Serial.print(Source, Functie);
    Functiecode_Receive(&Functie);
}
/*
void Functiecode_Receive(uint8_t* FunctieCode)
{
    // serial.print("Test: ");
    // serial.println(FunctieCode);
    switch (FunctieCode);
    {
    case 0xC0:
    break;

    case 0xC3:
    Retransmit();
    break;

    case 0xD0:
    Opvragen_Meetwaardes_MM();
    break;

    case 0xD3:
    break;
    
    case 0xD4:
    case 0xD7:
    Error(1);
    break;

    case 0xD8:
    break;

    case 0xDB:
    break;

    case 0xDC:
    break;

    case 0xDF:
    break;

//  FC10
    case 0xE4:  
    Error(1);
    break;
    case 0xE7:  
    Meetmodule_Gevonden();
    break;
    
    case 0xE8:
    case 0xEB:
    Error(1);
    break;

    case 0xEC:
    break;
    
    case 0xEF:
    break;
    
    case 0xF0:
    break;

    case 0xF3:
    break;
    
    default:
    Error(0);
    break;
    }
}

void Sturen()
{
    hu_packet_t packet;
    packet.start=HU_PROTOCOL_START_BYTE;
    packet.end=HU_PROTOCOL_END_BYTE;
    packet.function= ;
    packet.length= HU_PROTOCOL_MIN_PACKET_LEN+(4*sizeof(float));
    packet.destination = ;
    packet.source = CM_Groep3_Adres;

    hu_protocol_transmit( &rh_ask, &packet );
}

void Retransmit()
{

}
void Opvragen_Meetwaardes_MM()
{

}
void Reset_module()
{

}
void Opvragen_Status()
{

}
void Meetmodule_Gevonden()  //  Aanvraag_Meetwaardes_MM
{

}
void Acknowledge()  // weet nog niet hoe Acknowledge werkt als niemand het kan versturen
{

}
void Centrale_Opslag_Gevonden()
{

}

void Error(uint8_t* Error_nummer)
{
    Serial.print("Error:");
    switch (Error_nummer)
    {
    case 0:
    Serial.print("geen goede functiecode");
    break;
    case 1:
    Serial.print("functiecode is onlogisch voor comunicatie module")
    break;
    }
}
*/
#endif