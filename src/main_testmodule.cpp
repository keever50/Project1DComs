#ifdef TEST_MODULE
#include <Arduino.h>
#include <hu_protocol.h>
#include <test_module_hardware.h>
#include <biterrors.h>
#include <logging2.h>
#include "SPI.h"
#include <Wire.h>

// TFT
#include "TFT_22_ILI9225.h"
#include <../fonts/FreeSans12pt7b.h>
#include <../fonts/FreeSans24pt7b.h>
#include <../fonts/TomThumb.h>
//TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);

// RADIO
RH_ASK rh_ask(RA_BITRATE, RA_RX, RA_TX, RA_TR, false);

// override the weak attribute of the logging2 library raw printing output
void log2_log_raw( const char* msg )
{
  Serial.print(msg);
}


// Setup
void setup() 
{
  Serial.begin(9600);
  rh_ask.init();
  
  
}

// Loop
void loop() 
{
  hu_packet_t packet;

  static unsigned long next=0;
  if(millis()-next>7000)
  {
    packet.function=0x02;
    packet.length=HU_PROTOCOL_MIN_PACKET_LEN+HU_PROTOCOL_MAX_DATA_SIZE;
    for(int i=0;i<HU_PROTOCOL_MAX_DATA_SIZE;i++)
    {
      packet.data[i]=random(0x00,0xFF);
    }
    packet.start=HU_PROTOCOL_START_BYTE;
    packet.end=HU_PROTOCOL_END_BYTE;


    Serial.println("Transmit");
    hu_protocol_transmit( &rh_ask, &packet );
    next=millis();
  }

  // Receive
  hu_prot_receive_err_t err = hu_protocol_receive( &rh_ask, &packet );
  Serial.print("Receive: ");
  Serial.println(err);
  if(err != HU_PROT_RECEIVE_IGNORE && err != HU_PROT_RECEIVE_LISTENING)
  {
    hu_protocol_print_packet(&packet);
  }

  //log2_log(LOG2_INFO, "TEST", "hello %d",5);



  delay(500);
}


#endif