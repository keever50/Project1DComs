#ifdef TEST_MODULE
#include <Arduino.h>
#include <hu_protocol.h>
#include <test_module_hardware.h>

#include "SPI.h"
#include <Wire.h>

// TFT
#include "TFT_22_ILI9225.h"
#include <../fonts/FreeSans12pt7b.h>
#include <../fonts/FreeSans24pt7b.h>
#include <../fonts/TomThumb.h>
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);

// RADIO
#include <RadioHead.h>
RH_ASK rh_ask(RA_BITRATE, RA_RX, RA_TX, 0, false);




void keys(int bytes)
{
  Serial.println(Wire.read());
}

// Setup
void setup() {
  Serial.begin(9600);
  Wire.begin();
}

// Loop
void loop() {
    uint8_t bytes = Wire.requestFrom(0x5F,1);
    uint8_t byte = Wire.read();
    if(byte)
    {
        Serial.print((char)byte);
    }
    
    hu_packet_t packet;
    hu_protocol_receive(&rh_ask, &packet);
    
    delay(50);
}


#endif