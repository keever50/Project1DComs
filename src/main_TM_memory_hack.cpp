#ifdef TM_MEM_HACK_TEST

#include <Arduino.h>
#include <test_module_hardware.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <stdint.h>
#include <ExtRAM.h>


void memorytest()
{
  const uint16_t linesize = 1000;
  const uint16_t lines=60000/linesize;
  uint8_t line[linesize];

  Serial.println("Generating");
  for(uint16_t i=0;i<linesize;i++)
  {
    line[i]=(uint8_t)i;
  }

  Serial.println("Writing");
  for(uint16_t a=0;a<60000;a+=linesize)
  {
    extram.write_bytes(a, line, linesize);
  }

  Serial.println("Reading and verify");
  for(uint16_t a=0;a<60000;a+=linesize)
  {
    extram.read_bytes(a, line, linesize);
    for(uint16_t i=0;i<linesize;i++)
    {
      if(line[i]!=(uint8_t)i)
      {
        Serial.println("Error " + String(line[i]));
      }
    }    
  }  

  Serial.println("Done");

}

void setup()
{
  Serial.begin(9600);
  while(!Serial);
  delay(500);

  pinMode(RAM_CS, OUTPUT);
  SPI.begin();
  int err = extram.set_mode(EXTRAM_MODE_SEQ);
  if(err)
  {
    Serial.println("Mode register did not match");
    while(true);
  }else{
    Serial.println("Mode set successfully");
  }


  memorytest();


}

void loop()
{

}

#endif