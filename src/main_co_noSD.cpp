#ifdef CENTRALE_OPSLAG


//Libraries includeren
#include <hu_protocol.h>
#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>

//LCD en RF instellen
LiquidCrystal_I2C lcd(0x27,  16, 2); 
RH_ASK rh_ask(2000, 10, 11, 0, false); // Bitrate, receive pin, transmit pin, select pin(niet gebruikt), select inverse(niet gebruikt)


//Bepaalde waardes definiëren
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

#define AckData 0x06
#define eindStudentNR 0b01011101

#define ButtonUp 4
#define ButtonDown 5
#define RedLed 8
#define GreenLed 9
#define YellowLed 3


//Struct waar de meetwaardes in kunnen aanmaken
struct dataReadings {
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



//Alle functies declareren
void AskData(int fileReadNR1, char fileDataPartNR1);
void SDtoLCD2(String voorwoord, float ltr);
void saveData(const dataReadings* readings);
void HUontvangen(void);
void sendrf(uint8_t functie, uint8_t destinatie, uint8_t dataS);
void meetwaardes_recv(uint8_t *data, uint8_t dataLenght);
uint8_t status_encode();
void blinkLed();
void SDtoLCD_MM(String voorwoord2, String Adress_MM_Decoded);
void resetArduino();

//Globale variabelen aanmaken
uint8_t statusbyte_send = statusbyte_namont;
int filewriteNR = 0;
char fileDataPartNR = 0;
int fileReadNR = 0;
char lrc_err_marijn = 0;



void setup()
{
    Serial.begin(9600);
    //Source adres instellen
    hu_protocol_set_address(hu_protocol_encode_address("2AG3CO"));
    Serial.println(F("Welkom bij de CO"));

    //drivers initialiseren
    rh_ask.init();
    lcd.init();
    lcd.backlight();

    //Welkom bericht op LCD
    lcd.print("Welkom");

    //Inputs & outputs instellen
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

    if(digitalRead(ButtonUp) == LOW) //Als de "omhoog" knop wordt ingedrukt
    {
        //Verander de te lezen waarde met +1
        fileDataPartNR +=1;
        if(fileReadNR <= 0) 
        { 
            fileReadNR = 1;
        }
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

    if(digitalRead(ButtonDown) == LOW) //Als de "omlaag" knop wordt ingedrukt
    {
        //Verander de te lezen waarde met -1
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
}

void HUontvangen(void)
{
    //Serial.println("Receiving...");

    hu_packet_t packet; //Pakket aanmaken dat later wordt ingevuld
    
    //Library aanroepen om pakket te ontvangen
    hu_prot_receive_err_t err = hu_protocol_receive( &rh_ask, &packet );

    if(err != HU_PROT_RECEIVE_IGNORE && err != HU_PROT_RECEIVE_LISTENING) //Geen fouten in pakket
    {
        //Volledig pakket printen
        hu_protocol_print_packet(&packet);
        
    }    
    else if(err == HU_PROT_RECEIVE_INCORRECT_LRC) //Controleren of er een LRC error is
    {
        lrc_err_marijn +=1;
        sendrf(functiem_retrans_trans, packet.source, 0); //Vraag om een retransmit
        blinkLed(); 
    }
    uint8_t* data = packet.data; //Data uit pakket halen
    uint8_t dataLengthStart = packet.length; //Lengte van het pakket

    if(err==HU_PROT_RECEIVE_RECEIVED)
    {
        analogWrite(YellowLed, HIGH);
    }
    if(err==HU_PROT_RECEIVE_RECEIVED)
    {   
      
        switch(packet.function) //Switch die kijkt naar de functiecode van het ontvangen pakket
        {
        case functiem_opvraag: //De testmodule vraagt de status op
            Serial.println(F("status opvraag"));
            delay(500);
            sendrf(functiem_opvraag_response, packet.source, status_encode()); //Stuur de status
            break;
        case functiem_overdragen_ask: //De CM vraagt of de CO klaar is om meetwaardes te ontvangen
            Serial.println(F("klaar om data te ontvangen"));
            delay(500);
            sendrf(functiem_acknowledge_trans, packet.source, AckData); //Stuur acknowledge
            break;
        case functiem_overdragen_waardes: //De CM stuurt de meetwaardes op
            lcd.clear();
            lcd.print(F("Data ontvangen"));
            statusbyte_send = statusbyte_amont; 
            meetwaardes_recv(data, dataLengthStart-4); //Meetwaardes lokaal opslaan
            sendrf(functiem_acknowledge_trans, packet.source, AckData); //Stuur acknowledge
            break;
        case functiem_retrans_rec: //Module vraagt om retransmit
            Serial.println(F("retransmit"));
            delay(500);
            sendrf(functiem_acknowledge_trans, packet.source, AckData); //Stuur acknowledge
            break;
        case functiem_reset: //De testmodule laat de CO resetten
            Serial.println(F("resetting...."));
            sendrf(functiem_acknowledge_trans, packet.source, AckData); //Stuur acknowledge
            resetArduino(); 
            break;
        default:
            Serial.println(F("No function code"));
            break;
        }
    }
    analogWrite(GreenLed, 0);
    memset(&packet, 0, sizeof(packet)); //Maak packet leeg
}

void meetwaardes_recv(uint8_t *dataR, uint8_t dataLength) //Meetwaardes opslaan in een tijdelijke struct
{
    filewriteNR +=1;
    byte dataArray[37];
    //char dataLength = sizeof(dataR);
    Serial.println(F("sizeof dataR"));
    Serial.println(dataLength, DEC);
    char j = 0;
    Serial.println(F("Ontvangen data:"));
    for(char i=0; i <= dataLength; i++) //Elke byte van ontvangen data overzetten naar dataArray
    {
        dataArray[i] = dataR[j];
        if(dataR[i] < 0x10) 
        {
            Serial.print(F("0"));
        }
        Serial.print(dataR[i], HEX);
        j++;
    }

    dataReadings reading;
    ////////////////////////////////////////////////////////////////////Hulp van Hayan Rafee
    byte* ptr = (byte*)&reading;
    for (uint8_t i = 1; i < sizeof(dataReadings); i++) //Struct reading vullen met de ontvangen floats
    {
        *ptr++ = dataArray[i];
    }
    /////////////////////////////////////////////////////////////////////////////////
    reading.MM_adress = dataArray[0];

    //Print de opgeslagen waardes    
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

void saveData(const dataReadings* readings) //Sla de waardes uit de tijdelijke struct op in een semi-permanente struct
{
    Serial.println(F("saving data"));
    
    switch(filewriteNR) //Switch die de waardes van elke meetmodule opslaat in zijn eigen struct
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

    Serial.println(F("data has been saved"));
    statusbyte_send = statusbyte_amopg;

}



void AskData(int fileReadNR1, char fileDataPartNR1) //Data opvragen met interfaceknoppen
{
    dataReadings* currentReading;
    switch(fileReadNR1) //Switch die de waardes van de juiste meetmodule kiest
    {
        case 1:
            currentReading = &reading1;
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

    switch (fileDataPartNR1) //Switch die van de gekozen meetmodule de juiste waarde selecteert
    {
        case 1:
            SDtoLCD_MM("Add meetmodule: ", hu_protocol_decode_address(currentReading->MM_adress));
            break;
        case 2:
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


void SDtoLCD2(String voorwoord, float ltr) //Print de waarde op het lcd scherm
{
    String dataString="";
    dataString =String(ltr, DEC);
    const int str_len = dataString.length() +1;
    char lcdText[str_len];
    dataString.toCharArray(lcdText, str_len); //Inhoud float omzetten naar een charArray voor het LCD
    Serial.println(F("display data:"));
    Serial.println(lcdText);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(voorwoord);
    lcd.setCursor(0,1);
    lcd.print(dataString);
}   

void SDtoLCD_MM(String voorwoord2, String Adress_MM_Decoded) //Adres meetmodule printen op LCD
{
    Serial.println(voorwoord2 + Adress_MM_Decoded);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(voorwoord2);
    lcd.setCursor(0,1);
    lcd.print(Adress_MM_Decoded);
}

void sendrf(uint8_t functie, uint8_t destinatie, uint8_t dataS) //Zendfunctie die de functiecode, bestemming en data vraagt
{
    analogWrite(YellowLed, 300);
    hu_packet_t packet; //Maak pakket aan

    //Geef het pakket de volgende waardes
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

uint8_t status_encode() //Statusbyte in elkaar zetten
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

void blinkLed() //Rode LED laten knipperen wanneer nodig
{
    for(char ll=0; ll <=10; ll++)
    {
        analogWrite(RedLed, 0);
        delay(200);
        analogWrite(RedLed, 300);
        delay(200);   
    }
}

void resetArduino() //Arduino resetten
{
    wdt_enable(WDTO_15MS);
    while(1);
}


#endif
