#ifdef CENTRALE_OPSLAG

#include <RadioHead.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SD.h>

LiquidCrystal_I2C lcd(0x27,  16, 2);

char message1[16];
char message2[16];

File dataFile;

void setup()
{
    lcd.init();
    lcd.backlight();
    sprintf(message1, "TEST HELP");
    sprintf(message2, "NOG EEN TEST");

    if (!SD.begin(4)) 
    {
        Serial.println("initialization failed!");
        while (1);
    }
    dataFile = SD.open("test.txt", FILE_WRITE);

    if (dataFile) {
    Serial.print("Writing to test.txt...");
    dataFile.println("testing 1, 2, 3.");
    // close the file:
    dataFile.close();
    Serial.println("done.");
    } 
    else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
    }
}

void loop()
{
    dataFile = SD.open("test.txt");
    if(dataFile)
    {
        while (dataFile.available())
        {
            lcd.setCursor(0,0);
            lcd.print(dataFile.read());
        }
        lcd.setCursor(0,1);
        lcd.print(message2);
    }
 
 

}


#endif