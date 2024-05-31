#ifdef CENTRALE_OPSLAG

#include <hu_protocol.h>
#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SD.h> // cs = 4, clk = 13, mosi = 11, miso = 12.

LiquidCrystal_I2C lcd(0x27,  16, 2); 
RH_ASK rh_ask(500, 3, 9, 0, false); // Bitrate, receive pin, transmit pin, select pin(unused), select inverse(unused)

File dataFile;
File extraFile;
String dataString;

void AskData(int fileReadNR1);
void SDtoLCD2(char ltr);
void saveData(int fileWriteNR1, String writeString1);
void HUontvangen(void);

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
    HUontvangen();
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

void HUontvangen(void)
{
    //Serial.println("Receiving...");

    //The function hu_protocol_receive will fill this packet in.
    hu_packet_t packet;

    /*Check if we received data, otherwise it returns HU_PROT_RECEIVE_LISTENING.
    It can also return HU_PROT_RECEIVE_IGNORE in case the packet was not for us.*/
    hu_prot_receive_err_t err = hu_protocol_receive( &rh_ask, &packet );
    if(err != HU_PROT_RECEIVE_IGNORE && err != HU_PROT_RECEIVE_LISTENING)
    {
        // Print out the packet on a pretty way
        hu_protocol_print_packet(&packet);
    }    

    // You can extract variables this way
    uint8_t function = packet.function;

    // We can extract data and calculate how much data we have by subtracting the non-data bytes from the packet length we received.
    uint8_t* data = packet.data;
    for(int ss=0; ss<strlen((char*)data); ss++)
    {
        dataString +=(char)data[ss];
    }
    if(err==HU_PROT_RECEIVE_RECEIVED)
    {   
        fileWriteNR +=1;
        Serial.println(dataString);
        saveData(fileWriteNR, dataString);
    }
    delay(500);
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

#endif