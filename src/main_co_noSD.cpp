#ifdef CENTRALE_OPSLAG

#include <hu_protocol.h>
#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>


#define functiem_opvraag 0b11011111 //functiecode opvragen status ontvangen
#define functiem_opvraag_response 0b11011100 //functiecode opvragen status antwoorden
#define functiem_overdragen_ask 0b11010111 //functiecode overdragen meetwaardes ready?
#define functiem_overdragen_waardes 0b11010100 // functiecode overdragen meetwaardes nu gestuurd
#define functiem_acknowledge 0b11101111 //functiecode voor acknowledge ontvangen
#define functiem_acknowledge_trans 0b11101100 //functiecode voor acknowledge sturen
#define functiem_retrans_trans 0b11000000 //functiecode voor retransmit sturen
#define functiem_retrans_rec 0b11000011 //functiecode voor retransmit ontvangen
#define functiem_reset 0b11011011 //functiecode voor reset ontvangen


#define statusbyte_namont 0b11100000
#define statusbyte_amont 0b11100111
#define statusbyte_namopg 0b11101000
#define statusbyte_amopg 0b11101111

#define eindStudentNR 0b01011101

#define ButtonUp 4
#define ButtonDown 5
#define RedLed 8
#define GreenLed 9
#define YellowLed 3

uint8_t statusbyte_send = statusbyte_namont;



struct dataReadings {// selectie floats in een bepaalde orde 
    float avgTemp;
    float avgLDR;
    float avgPot;
    float maxTemp;
    float maxLDR;
    float maxPot;
    float minTemp;
    float minLDR;
    float minPot;
    uint8_t MM_adress;
};
static dataReadings reading1, reading2, reading3, reading4, reading5, reading6, reading7, reading8;




LiquidCrystal_I2C lcd(0x27,  16, 2); 
RH_ASK rh_ask(500, 10, 11, 0, false); // Bitrate, receive pin, transmit pin, select pin(unused), select inverse(unused)


void AskData(int fileReadNR1, char fileDataPartNR1);
void SDtoLCD2(String voorwoord, float ltr);
void saveData(const dataReadings* readings);
void HUontvangen(void);
void sendrf(uint8_t functie, uint8_t destinatie, uint8_t dataS);
void meetwaardes_recv(uint8_t *data, uint8_t dataLenght);
uint8_t status_encode();
void blinkLed();
String Address_decode(uint8_t adressMM);
void SDtoLCD_MM(String voorwoord2, String Adress_MM_Decoded);

void resetArduino();

int filewriteNR = 0;
char fileDataPartNR = 0;
int fileReadNR = 0;
char lrc_err_marijn = 0;

String dataString;

void setup()
{
    hu_protocol_set_address(hu_protocol_encode_address("2AG3CO"));
    Serial.begin(9600);
    rh_ask.init();
    lcd.init();
    lcd.backlight();
    pinMode(ButtonUp, INPUT_PULLUP);
    pinMode(ButtonDown, INPUT_PULLUP);
    pinMode(6, INPUT_PULLUP);
    pinMode(RedLed, OUTPUT);
    pinMode(GreenLed, OUTPUT);
    pinMode(YellowLed, OUTPUT);

    analogWrite(RedLed, 300);
}

void loop()
{
    HUontvangen();
    if(digitalRead(4) == LOW)
    {
        fileDataPartNR +=1;
        if(fileReadNR <= 0) fileReadNR = 1;
        if(fileDataPartNR >= 11)
        {
            fileDataPartNR = 1;
            fileReadNR +=1;
        }
        Serial.println(fileReadNR, DEC);
        Serial.println(fileDataPartNR, DEC);
        AskData(fileReadNR, fileDataPartNR);
        delay(500);
    }

    if(digitalRead(5) == LOW)
    {
        fileDataPartNR -=1;
        if(fileDataPartNR <=0)
        {
            fileDataPartNR = 1;
            fileReadNR -=1;
        }
        if(fileReadNR <= 0) fileReadNR = 1;   
        Serial.println(fileReadNR, DEC);
        Serial.println(fileDataPartNR, DEC); 
        AskData(fileReadNR, fileDataPartNR);
        delay(500);
    } 
    if(digitalRead(6) == LOW)
    {
        uint8_t dataTT[] = {0x0C, 0xCD, 0xCC, 0xCC, 0x40, 0x66, 0xA6, 0xA4, 0x43, 0x9A, 0x19, 0xA2, 0x43, 0xB8, 0x1E, 0x15, 0x41, 0x00, 0x80, 0xB7, 0x43, 0x00, 0x00, 0xAA, 0x43, 0x3D, 0x0A, 0x87, 0x40, 0x00, 0x00, 0x99, 0x43, 0x00, 0x80, 0x98, 0x43};
        meetwaardes_recv(dataTT, 37);
        //sendrf(functiem_acknowledge_trans, 0, 0x06);
        delay(1000);
    }
}

void HUontvangen(void)
{
    //Serial.println("Receiving...");

    ///////////////////////////////////////////////////////////////////////////Written with Kevin Witteveen
    hu_packet_t packet;
    
    hu_prot_receive_err_t err = hu_protocol_receive( &rh_ask, &packet );
    if(err != HU_PROT_RECEIVE_IGNORE && err != HU_PROT_RECEIVE_LISTENING)
    {
        // Print out the packet 
        hu_protocol_print_packet(&packet);
        
    }    
    /////////////////////////////////////////////////////////////////////////////////////////////////
    else if(err == HU_PROT_RECEIVE_INCORRECT_LRC)
    {
        lrc_err_marijn +=1;
        sendrf(functiem_retrans_trans, packet.source, 0);
        blinkLed();
    }
    uint8_t* data = packet.data;
    uint8_t dataLengthStart = packet.length;
    if(err==HU_PROT_RECEIVE_RECEIVED)
    {   
        analogWrite(GreenLed, 300);
        Serial.println(F("iets ontvangen"));
        switch(packet.function)
        {
        case functiem_opvraag:
            delay(500);
            sendrf(functiem_opvraag_response, packet.source, status_encode());
            break;
        case functiem_overdragen_ask:
        delay(500);
            sendrf(functiem_acknowledge_trans, packet.source, 0x06);
            break;
        case functiem_overdragen_waardes:
            statusbyte_send = statusbyte_amont;         
            meetwaardes_recv(data, dataLengthStart-4);
            sendrf(functiem_acknowledge_trans, packet.source, 0x06);
            break;
        case functiem_retrans_rec:
            delay(500);
            sendrf(functiem_acknowledge_trans, packet.source, 0x06);
            break;
        case functiem_reset:
            Serial.println(F("resetting...."));
            sendrf(functiem_acknowledge_trans, packet.source, 0x06);
            resetArduino();
            break;
        /*case functiem_acknowledge:
            sendrf(functiem_acknowledge_trans, packet.source, 0x06);
            break; */
        default:
            Serial.println(F("No function code"));
            break;
        }
    }
    analogWrite(GreenLed, 0);
    memset(&packet, 0, sizeof(packet));
}

void meetwaardes_recv(uint8_t *dataR, uint8_t dataLength)
{
    filewriteNR +=1;
    byte dataArray[37];
    //char dataLength = sizeof(dataR);
    Serial.println(F("sizeof dataR"));
    Serial.println(dataLength, DEC);
    char j = 0;
    Serial.println(F("Ontvangen data:"));
    for(char i=0; i <= dataLength; i++)
    {
        dataArray[i] = dataR[j];
        if(dataR[i] < 0x10) 
        {
            Serial.print('0');
        }
        Serial.println(dataR[i], HEX);
        j++;
    }

    dataReadings reading;
    ////////////////////////////////////////////////////////////////////Hulp van Hayan Rafee
    byte* ptr = (byte*)&reading;
    for (uint8_t i = 1; i < sizeof(dataReadings); i++) 
    {
        *ptr++ = dataArray[i];
    }
    /////////////////////////////////////////////////////////////////////////////////
    reading.MM_adress = dataArray[0];

    //dataReadings reading = {dataArray[0],(float)dataArray[1,2,3,4],dataArray[5,6,7,8],dataArray[9,10,11,12],dataArray[13,14,15,16],dataArray[17,18,19,20],dataArray[21,22,23,24],dataArray[25,26,27,28],dataArray[29,30,31,32],dataArray[33,34,35,36]};    
    Serial.println(F("De waardes opgeslagen vlak na receive, eerst dataArray en dan dataR"));
    Serial.println(reading.MM_adress);
    Serial.println(reading.avgTemp);
    Serial.println(reading.avgLDR);
    Serial.println(reading.avgPot);
    Serial.println(reading.maxTemp);
    Serial.println(reading.maxLDR);
    Serial.println(reading.maxPot);
    Serial.println(reading.minTemp);
    Serial.println(reading.minLDR);
    Serial.println(reading.minPot);
    Serial.println(F("Einde waardes is nu aangebroken ///////////////////"));
    statusbyte_send = statusbyte_namopg;
    saveData(&reading);
    
}

void saveData(const dataReadings* readings)
{
    Serial.println(F("saving data"));
    
    switch(filewriteNR)
    {
        case 1:
        reading1 = {readings->avgTemp,readings->avgLDR,readings->avgPot,readings->maxTemp,readings->maxLDR,readings->maxPot,readings->minTemp,readings->minLDR,readings->minPot, readings->MM_adress};
        break;
        case 2:
        reading2 = {readings->avgTemp,readings->avgLDR,readings->avgPot,readings->maxTemp,readings->maxLDR,readings->maxPot,readings->minTemp,readings->minLDR,readings->minPot, readings->MM_adress};
        break;
        case 3:
        reading3 = {readings->avgTemp,readings->avgLDR,readings->avgPot,readings->maxTemp,readings->maxLDR,readings->maxPot,readings->minTemp,readings->minLDR,readings->minPot, readings->MM_adress};
        break;
        case 4:
        reading4 = {readings->avgTemp,readings->avgLDR,readings->avgPot,readings->maxTemp,readings->maxLDR,readings->maxPot,readings->minTemp,readings->minLDR,readings->minPot, readings->MM_adress};
        break;
        case 5:
        reading5 = {readings->avgTemp,readings->avgLDR,readings->avgPot,readings->maxTemp,readings->maxLDR,readings->maxPot,readings->minTemp,readings->minLDR,readings->minPot, readings->MM_adress};
        break;
        case 6:
        reading6 = {readings->avgTemp,readings->avgLDR,readings->avgPot,readings->maxTemp,readings->maxLDR,readings->maxPot,readings->minTemp,readings->minLDR,readings->minPot, readings->MM_adress};
        break;
        case 7:
        reading7 = {readings->avgTemp,readings->avgLDR,readings->avgPot,readings->maxTemp,readings->maxLDR,readings->maxPot,readings->minTemp,readings->minLDR,readings->minPot, readings->MM_adress};
        break;
        case 8:
        reading8 = {readings->avgTemp,readings->avgLDR,readings->avgPot,readings->maxTemp,readings->maxLDR,readings->maxPot,readings->minTemp,readings->minLDR,readings->minPot, readings->MM_adress};
        break;
    }

    Serial.println(F("data has been saved, avgTemp:"));
    Serial.println(reading1.avgTemp);
    statusbyte_send = statusbyte_amopg;

}



void AskData(int fileReadNR1, char fileDataPartNR1)
{
    dataReadings* currentReading;
    //fileReadNR1 = 1; //verwijder straks
    //fileDataPartNR1 = 1; //verwijder straks
    switch(fileReadNR1)
    {
        case 1:
            currentReading = &reading1;
            Serial.println(F("currentreading is nu reading 1, avgTemp:"));
            Serial.println(currentReading->avgTemp);
            break;
        case 2:
            currentReading = &reading2;
            break;
        case 3:
            currentReading = &reading3;
            break;
        case 4:
            currentReading = &reading4;
            break;
        case 5:
            currentReading = &reading5;
            break;
        case 6:
            currentReading = &reading6;
            break;
        case 7:
            currentReading = &reading7;
            break;
        case 8:
            currentReading = &reading8;
            break;
    }

    switch (fileDataPartNR1)
    {
        case 1:
            
            Serial.println(F("sdnaarLCD krijgt als mm adres:"));
            Serial.println(currentReading->MM_adress, DEC);
            //SDtoLCD2("Add meetmodule: ", currentReading->MM_adress);
            SDtoLCD_MM("Add meetmodule: ", Address_decode(currentReading->MM_adress));
            break;
        case 2:
            Serial.println(F("sdnaarLCD krijgt als gem temp:"));
            Serial.println(currentReading->avgTemp, DEC);
            SDtoLCD2("Gemiddeld Temp: ",currentReading->avgTemp);
            break;
        case 3:
            SDtoLCD2("Gemiddeld licht:",currentReading->avgLDR);
            break;
        case 4:
            SDtoLCD2("Gemiddeld potm: ",currentReading->avgPot);
            break;
        case 5:
            SDtoLCD2("Maximaal Temp:  ",currentReading->maxTemp);
            break;
        case 6:
            SDtoLCD2("Maximaal licht: ",currentReading->maxLDR);
            break;
        case 7:
            SDtoLCD2("Maximaal potm:  ",currentReading->maxPot);
            break;
        case 8:
            SDtoLCD2("Minimaal temp:  ",currentReading->minTemp);
            break;
        case 9:
            SDtoLCD2("Minimaal licht: ",currentReading->minLDR);
            break;
        case 10:
            SDtoLCD2("Minimaal potm:  ",currentReading->minPot);
            break;
            default:
            break;
        }
}


void SDtoLCD2(String voorwoord, float ltr)
{
    Serial.println(F("ltr is geworden:"));
    Serial.println(ltr, DEC);
    dataString =String(ltr, DEC);
    const int str_len = dataString.length() +1;
    char lcdText[str_len];
    dataString.toCharArray(lcdText, str_len);
    Serial.println(F("char array is:"));
    Serial.println(lcdText);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(voorwoord);
    lcd.setCursor(0,1);
    lcd.print(dataString);
}   

void SDtoLCD_MM(String voorwoord2, String Adress_MM_Decoded)
{
    Serial.println(Adress_MM_Decoded);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(voorwoord2);
    lcd.setCursor(0,1);
    lcd.print(Adress_MM_Decoded);
}

void sendrf(uint8_t functie, uint8_t destinatie, uint8_t dataS)
{
    analogWrite(YellowLed, 300);
    hu_packet_t packet;
    packet.start=HU_PROTOCOL_START_BYTE;
    packet.end=HU_PROTOCOL_END_BYTE;
    packet.function= functie;
    packet.data[HU_PROTOCOL_MAX_DATA_SIZE] = dataS;
    packet.length= HU_PROTOCOL_MIN_PACKET_LEN+(sizeof(dataS));
    packet.destination = destinatie;
    packet.source = hu_protocol_encode_address("2AG3CO");
    Serial.println(F("Packet made"));
    hu_protocol_transmit( &rh_ask, &packet );
    Serial.println(F("Packet sent"));
    analogWrite(YellowLed, 0);
}

uint8_t status_encode()
{
    byte Ma = statusbyte_send;
    byte Mb = (byte)(millis()/60000UL);
    byte Mc = lrc_err_marijn;
    byte Md = eindStudentNR;
    //uint32_t status_calcd = ((uint32_t)Ma << 24) | ((uint32_t)Mb << 16) | ((uint32_t)Mc << 8) | Md;
    uint8_t status_calcd2[4];
    status_calcd2[0]= Ma;
    status_calcd2[1]=Mb;
    status_calcd2[2]=Mc;
    status_calcd2[3]=Md;

    return status_calcd2[0,1,2,3];
}

void blinkLed()
{
    for(char ll=0; ll <=10; ll++)
    {
        analogWrite(RedLed, 0);
        delay(200);
        analogWrite(RedLed, 300);
        delay(200);   
    }
}

void resetArduino()
{
    wdt_enable(WDTO_15MS);
    while(1);
}

String Address_decode(uint8_t addressMM)
{
    String addressName="";
    switch((addressMM >> 5) & 0b111)
    {
        case 0b000:
            addressName += "MM ";
            break;
        case 0b001:
            addressName += "CM ";
            break;
        case 0b010:
            addressName += "NM ";
            break;
        case 0b011:
            addressName += "CO ";
            break;
        case 0b100:
            addressName += "TM ";
            break;
    }
	

    switch(addressMM & 0b11)
    {
        case 0b00:
            addressName +="EV2A ";
            break;
        case 0b01:
            addressName +="EV2B ";
            break;
        case 0b10:
            addressName +="EV2C ";
            break;
        case 0b11:
            addressName +="EV2D ";
            break; 
        default:
            break;
    }

    switch((addressMM >> 2) & 0b111)
    {
        case 0b001:
            addressName +="Groep 1";
            break;
        case 0b010:
            addressName +="Groep 2";
            break;
        case 0b011:
            addressName +="Groep 3";
            break;
        case 0b100:
            addressName +="Groep 4";
            break; 
        case 0b101:
            addressName +="Groep 5";
            break;
        case 0b110:
            addressName +="Groep 6";
            break;
        case 0b111:
            addressName +="Groep 7";
            break; 
        default:
            break;
    }

    if(addressMM == 0x00) addressName = "Geen Waardes";

    return addressName;
}

#endif
