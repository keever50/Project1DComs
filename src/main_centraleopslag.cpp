

#include <hu_protocol.h>
#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SD.h> // cs = 4, clk = 13, mosi = 11, miso = 12.

LiquidCrystal_I2C lcd(0x27,  16, 2); 
RH_ASK rh_ask(500, 3, 11, 0, false); // Bitrate, receive pin, transmit pin, select pin(unused), select inverse(unused)

File dataFile;
File extraFile;
String dataString;

void AskData(void);
void SDtoLCD2(char ltr);

int fileReadNR = 0;
int fileWriteNR = 0;

void setup()
{
    hu_protocol_set_address(hu_protocol_encode_address("2AG3CO"));
    Serial.begin(9600);
    rh_ask.init();
    lcd.init();
    lcd.backlight();
    if (!SD.begin(10))
    {
        Serial.println(F("initialization failed!"));
        while (1);
    }
    pinMode(4, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    pinMode(6, INPUT_PULLUP);
  
}

void loop()
{
    if(digitalRead(4) == LOW)
    {
        fileReadNR +=1;   
        AskData(fileReadNR);
        delay(500);
    }

    if(digitalRead(5) == LOW)
    {
        fileReadNR -=1;
        if(fileReadNR <= 0) fileReadNR = 0;    
        AskData(fileReadNR);
        delay(500);
    } 

    if(digitalRead(6)==LOW)
    {
        fileWriteNR +=1; 
        String writeString = "AvgTemp=19,5";
        saveData(fileWriteNR, writeString);
        delay(500);
        
    }

}

void saveData(int fileWriteNR1, String writeString1)
{
    String fileNameW = "data" + String(fileWriteNR1) + ".txt";
    extraFile=SD.open(fileNameW, FILE_WRITE);
    if(extraFile)
    {
        lcd.clear();
        lcd.print("Saving Rec Data:");
        extraFile.println(writeString1);
        extraFile.close();
        Serial.println(writeString1);
    }
}



void AskData(int fileReadNR1)
{
    String fileNameR = "data" + String(fileReadNR1) + ".txt";
    dataFile = SD.open(fileNameR);
    if (dataFile)
    {
        Serial.println(F("data1.txt:"));
        while (dataFile.available()) 
        {
            SDtoLCD2(dataFile.read());
        }
        dataFile.close();
        dataString = "";
    } 
    else 
    {
    Serial.println(F("error opening test.txt"));
    lcd.print("Error met SD");
    }

}

void SDtoLCD2(char ltr)
{
    dataString += ltr;
    const int str_len = dataString.length() +1;
    char lcdText[str_len];
    dataString.toCharArray(lcdText, str_len);
    Serial.println(lcdText);
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print(lcdText);
}
