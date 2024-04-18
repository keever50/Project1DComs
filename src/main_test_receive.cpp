#ifdef TEST_RECEIVE

#include <Arduino.h>
#include <RH_ASK.h>

RH_ASK driver(2000, 3, 4, 0);

void setup()
{
    Serial.begin(19200);
    driver.init();
}

void loop()
{
    uint8_t len;
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    if(driver.recv(buf, &len))
    {
        Serial.println(len);
        Serial.print("start[");
        for(int i=0;i<len;i++)
        {
            Serial.print(buf[i]);
        }
        Serial.println("]end");
    }
}

#endif