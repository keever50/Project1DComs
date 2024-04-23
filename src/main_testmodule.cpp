#ifdef TEST_MODULE
#include <Arduino.h>
#include <hu_protocol.h>
#include <test_module_hardware.h>
#include <biterrors.h>

#include "SPI.h"
#include <Wire.h>

// TFT
#include "TFT_22_ILI9225.h"
#include <../fonts/FreeSans12pt7b.h>
#include <../fonts/FreeSans24pt7b.h>
#include <../fonts/TomThumb.h>
//TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);

// RADIO
#include <RadioHead.h>
RH_ASK rh_ask(RA_BITRATE, RA_RX, RA_TX, 0, false);

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
  // packet.function=0x01;
  // packet.length=HU_PROTOCOL_MIN_PACKET_LEN;
  // packet.start=HU_PROTOCOL_START_BYTE;
  // packet.end=HU_PROTOCOL_END_BYTE;
  

  // Serial.println("Transmit");
  // int err = hu_protocol_transmit( &rh_ask, &packet );
  // if(err)
  // {
  //   Serial.print("Transmit error ");
  //   Serial.println(err);
  // }

  // Receive
  hu_prot_receive_err_t err = hu_protocol_receive( &rh_ask, &packet );
  Serial.print("Return: ");
  Serial.println(err);
  if(err != HU_PROT_RECEIVE_IGNORE && err != HU_PROT_RECEIVE_LISTENING)
  {
    hu_protocol_print_packet(&packet);
  }

  delay(100);
}


#endif