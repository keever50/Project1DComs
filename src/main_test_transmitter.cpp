#ifdef TRANSMIT_TEST

#include <RH_ASK.h>

RH_ASK driver(2000, 3, 4, 0, false);


void setup()
{
    driver.init();
}

void loop()
{
    driver.send("test",4);
    delay(100);
}


#endif