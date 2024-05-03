#ifdef TM_MEM_HACK_TEST

#include <Arduino.h>
#include <test_module_hardware.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <SD.h>




void setup()
{
   Serial.begin(9600);
   while(!Serial);
   delay(500);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CS)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
   

}

void loop()
{

}

#endif