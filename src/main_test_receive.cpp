#ifdef TEST_RECEIVE

#include <Arduino.h>
#include <RH_ASK.h>

RH_ASK driver(1000, 3, 11, 0, false);


void setup()
{
    Serial.begin(9600);
    bool success = driver.init();
    if(!success)
    {
        Serial.println("init fail");
        delay(500);
        while(true);
    }
    //pinMode(3, INPUT);


}


void loop()
{
    uint8_t len;
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
   // driver.printBuffer("buffer:", buf, 4);
    // d
    if(driver.recv(buf, &len))
    {
        Serial.println(len);
        Serial.print("start[");
        for(int i=0;i<RH_ASK_MAX_MESSAGE_LEN;i++)
        {
            Serial.print((char)buf[i]);
        }
        Serial.println("]end");
        memset(buf, 0, RH_ASK_MAX_MESSAGE_LEN);

    }
    delay(100);
}

#endif