#ifdef CENTRALE_OPSLAG

#include <RadioHead.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27,  16, 2);

char message1[16];
char message2[16];

void setup()
{
    lcd.init();
    lcd.backlight();
    sprintf(message1, "TEST HELP");
    sprintf(message2, "NOG EEN TEST");
}

void loop()
{
 delay(1000);
 lcd.setCursor(0,0);
 lcd.print(message1);
 lcd.setCursor(0,1);
 lcd.print(message2);

}


#endif