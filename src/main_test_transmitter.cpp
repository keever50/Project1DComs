#ifdef TRANSMIT_TEST

#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver(1000,3,11,0,false);

void setup()
{
    randomSeed(A1);
    Serial.begin(9600);	  // Debugging only
    driver.init();
}

void loop()
{
    char *msg = "TEam_3!_is_het_beste";
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    delay(1000+random(100,2000));
}

#endif