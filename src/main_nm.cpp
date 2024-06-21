#ifdef NAVIGATIE_TEST
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <LiquidCrystal_I2C.h>
#include <hu_protocol.h>
#include <avr/wdt.h>

#define PN532_IRQ   (2)
#define PN532_RESET (3)
#define red_pin (8)
#define yellow_pin (9)
#define green_pin (10)
#define BITRATE 2000

char MM = 0;
char flag = 0;
RH_ASK rh_ask(BITRATE, 3, 11, 0, false);

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
LiquidCrystal_I2C lcd(0x027, 16, 2);

void nm_route(char i);
void kaartbepaler();
void CO_route(char i);
void MM_gevonden(char i);
void CO_gevonden();
void RF_nm_receive();
void Error();
void ack_nm();
void reset();

//Dit zijn pijlen die later op het scherm worden weergeven
byte arrow_up[] = 
{
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

byte arrow_down[] = 
{
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B11111,
  B01110,
  B00100
};

void setup(void)
{
  Serial.begin(9600);
  nfc.begin();
  lcd.init();
  rh_ask.init();
  lcd.backlight();
  lcd.createChar(0, arrow_up);
  lcd.createChar(1, arrow_down);
  hu_protocol_set_address(0x4C);    //Zet eigen adres als 4C (hex)
  pinMode(red_pin, OUTPUT);
  pinMode(yellow_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  uint32_t status = nfc.getFirmwareVersion();
  if (!status) {
    while (1);
  }
  lcd.setCursor(0,0);
  lcd.print(F("Scan een"));
  lcd.setCursor(0, 1);
  lcd.print(F("kaart:"));
}

void loop(void)
{
  digitalWrite(red_pin, HIGH);
  kaartbepaler();
}

void kaartbepaler()
{
  //Alle mogelijke startlocaties staan hier gegeven
uint8_t startlocatie1[4] = {0x92, 0x83, 0x1B, 0xCC};
uint8_t startlocatie2[4] = {0x1C, 0x31, 0xFC, 0x42};
uint8_t startlocatie3[4] = {0x02, 0x34, 0x11, 0x7E};
uint8_t startlocatie4[4] = {0xD2, 0x13, 0x20, 0xF4};
uint8_t startlocatie5[4] = {0xC2, 0x42, 0x0F, 0xCC};
uint8_t startlocatie6[4] = {0x02, 0x19, 0x07, 0xCC};
uint8_t startlocatie7[4] = {0x52, 0xB2, 0x24, 0xCC};
uint8_t startlocatie8[4] = {0x72, 0x87, 0x1F, 0x7E};
uint8_t startlocatie9[4] = {0x32, 0x27, 0x18, 0xCC};
uint8_t startlocatie10[4] = {0x52, 0x16, 0x2F, 0xF4};
uint8_t startlocatie11[4] = {0xB2, 0xBF, 0x14, 0xF4};
uint8_t startlocatie12[4] = {0x72, 0xFD, 0x45, 0xCC};
uint8_t startlocatie13[4] = {0x12, 0x5B, 0xAB, 0xC2};
uint8_t startlocatie14[4] = {0xA2, 0xF1, 0x30, 0xF4};
uint8_t startlocatie15[4] = {0xE2, 0xF7, 0x14, 0xCC};
uint8_t startlocatie16[4] = {0xFC, 0x8F, 0x44, 0x0B};
uint8_t startlocatie17[4] = {0x32, 0x18, 0x71, 0xC2};
uint8_t startlocatie18[4] = {0xC2, 0x4A, 0x1B, 0xF4};
uint8_t startlocatie19[4] = {0x92, 0xE6, 0xA8, 0xC2};
uint8_t startlocatie20[4] = {0xF2, 0xED, 0x4B, 0xCC};
uint8_t startlocatie21[4] = {0x82, 0xF1, 0x45, 0xC2};
uint8_t startlocatie22[4] = {0x52, 0xA5, 0xB2, 0xCA};
uint8_t startlocatie23[4] = {0x32, 0xD7, 0x6C, 0xC2};
uint8_t testkaart1[4] = {0xF3, 0x75, 0xBB, 0xC2};
uint8_t testkaart2[4] = {0xA5, 0x3F, 0xFF, 0xBB};

  char i;

  uint8_t UID;
  uint8_t UID_tag[7]= { 0, 0, 0, 0, 0, 0, 0};
  uint8_t UIDLengte;
//De volgende functie scant de kaart en zet de gevonde UID in een arrayy, en bepaalt de lengte van deze array
  UID = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, UID_tag, &UIDLengte);

  if (UID)  //De functie gaat pas verder als de UID gevonden is
  {
    lcd.clear();
    lcd.print(F("Kaart gescand"));
    delay(1000);
    lcd.clear();

    switch (UID_tag[0])
    {
      //De gescand UID tag wordt hier vergeleken met alle mogelijke startlocaties
      case 0x92:
      if (memcmp(UID_tag, startlocatie1, 4)== 0)
      {
        i=1;
        nm_route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie19, 4) == 0)
      {
        i=19;
        MM_gevonden(i);
        RF_nm_receive();
        if (flag == 1)
        {
        MM++;
        ack_nm();
        if (MM == 5)
        {
          CO_route(i);
          break;
        }
        else
        nm_route(i);
        break;
        }
        else
        Error();
      }

      case 0x1C:
      if (memcmp(UID_tag, startlocatie2, 4)== 0)
      {
        i=2;
        MM_gevonden(i);
        RF_nm_receive();
        if (flag == 1)
        {
        MM++;
        ack_nm();
        if (MM == 5)
        {
          CO_route(i);
          break;
        }
        else
        nm_route(i);
        break;
        }
        else
        Error();
      }

      case 0x02:
      if (memcmp(UID_tag, startlocatie3, 4)== 0)
      {
        i=3;
        nm_route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie6, 4)== 0)
      {
        i=6;
        nm_route(i);
        break;
      }

      case 0xD2:
      if (memcmp(UID_tag, startlocatie4, 4)== 0)
      {
        i=4;
        nm_route(i);
        break;
      }

      case 0xC2:
      if (memcmp(UID_tag, startlocatie5, 4)== 0)
      {
        i=5;
        nm_route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie18, 4) == 0)
      {
        i=18;
        nm_route(i);
        break;
      }

      case 0x52:
      if (memcmp(UID_tag, startlocatie7, 4)== 0)
      {
        i=7;
        nm_route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie10, 4)== 0)
      {
        i=10;
        nm_route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie22, 4)== 0)
      {
        i=22;
        nm_route(i);
        break;
      }

      case 0x72:
      if (memcmp(UID_tag, startlocatie8, 4)== 0)
      {
        i=8;
        nm_route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie12, 4)== 0)
      {
        i=12;
        nm_route(i);
        break;
      }

      case 0x32:
      if (memcmp(UID_tag, startlocatie9, 4)== 0)
      {
        i=9;
        if (MM == 5)
        {
          //Als alle meetmodules zijn gescand, wordt er een bericht verstuurd naar de CM en wordt het programma opnieuw gestart
          CO_gevonden();
          lcd.clear();
          lcd.print(F("Eindpunt breikt"));
          delay(2000);
          lcd.clear();
          lcd.print(F("Terminating"));
          lcd.setCursor(0, 1);
          lcd.print(F("Programme...."));
          delay(2000);
          reset();
        }
        else
        nm_route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie17, 4)== 0)
      {
        i=17;
        nm_route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie23, 4)== 0)
      {
        i=23;
        nm_route(i);
        break;
      }

      case 0xB2:
      if (memcmp(UID_tag, startlocatie11, 4)== 0)
      {
        i=11;
        MM_gevonden(i);
        RF_nm_receive();
        if (flag == 1)
        {
        MM++;
        ack_nm();
        if (MM == 5)
        {
          CO_route(i);
          break;
        }
        else
        nm_route(i);
        break;
        }
        else
        Error();
      }

      case 0x12:
      if (memcmp(UID_tag, startlocatie13, 4)== 0)
      {
        i=13;
        nm_route(i);
        break;
      }

      case 0xA2:
      if (memcmp(UID_tag, startlocatie14, 4)== 0)
      {
        i=14;
        MM_gevonden(i);
        RF_nm_receive();
        if (flag == 1)
        {
        MM++;
        ack_nm();
        if (MM == 5)
        {
          CO_route(i);
          break;
        }
        else
        nm_route(i);
        break;
        }
        else
        Error();
      }

      case 0xE2:
      if (memcmp(UID_tag, startlocatie15, 4)== 0)
      {
        i=15;
        MM_gevonden(i);
        RF_nm_receive();
        if (flag == 1)
        {
        MM++;
        ack_nm();
        if (MM == 5)
        {
          CO_route(i);
          break;
        }
        else
        nm_route(i);
        break;
        }
        else
        Error();
      }

      case 0xFC:
      if (memcmp(UID_tag, startlocatie16, 4)== 0)
      {
        i=16;
        nm_route(i);
        break;
      }

      case 0xF2:
      if (memcmp(UID_tag, startlocatie20, 4)== 0)
      {
        i=20;
        nm_route(i);
        break;
      }

      case 0x82:
      if (memcmp(UID_tag, startlocatie21, 4)== 0)
      {
        i=21;
        nm_route(i);
        break;
      }

      case 0xF3:
      if (memcmp(UID_tag, testkaart1, 4)== 0)
      {
        i=0;
        MM_gevonden(i);
        RF_nm_receive();
        if (flag == 1)
        {
        MM++;
        ack_nm();
        if (MM == 5)
        {
          CO_route(i);
          break;
        }
        else
        nm_route(i);
        break;
        }
        else
        Error();
      }

      case 0xA5:
      if (memcmp(UID_tag, testkaart2, 4)== 0)
      {
        i=0;
        MM_gevonden(i);
        RF_nm_receive();
        if (flag==1)
        {
        MM++;
        ack_nm();
        if (MM == 5)
        {
          CO_route(i);
          break;
        }
        else
        nm_route(i);
        break;
        }
        else
        Error();
      }
      default:
      lcd.print(F("probeer opnieuw"));
      break;
    }
}
}

void nm_route(char i)
{
    lcd.setCursor(0,0);
    lcd.print(F("Route wordt"));
    lcd.setCursor(0,1);
    lcd.print(F("berekend....."));
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Route gevonden!"));
    delay(1000);
    lcd.clear();
  switch (i)
  {
    //alle mogelijke routes
    case 0:
    lcd.print(F("geen geldige"));
    lcd.setCursor(0,1);
    lcd.print(F("route gevonden"));
    break;

    case 1:
    lcd.print(F("2"));
    lcd.write(1);
    lcd.print(F(" 9"));
    lcd.write(0x7E);
    lcd.print(F(" 3"));
    lcd.write(0x7F);
    break;

    case 2:
    lcd.print(F("2"));
    lcd.write(1);
    lcd.print(F(" 41"));
    lcd.write(0x7F);
    lcd.print(F(" 32"));
    lcd.write(0x7F);
    lcd.print(F(" 6"));
    lcd.write(0x7F);
    break;

    case 3:
    lcd.print(F("1"));
    lcd.write(1);
    lcd.print(F(" 5"));
    lcd.write(0x7E);
    lcd.print(F(" 1"));
    lcd.write(0x7E);
    break;

    case 4:
    lcd.print(F("1"));
    lcd.write(1);
    lcd.print(F(" 12"));
    lcd.write(0x7E);
    lcd.print(F(" 1"));
    lcd.write(0x7E);
    break;

    case 5:
    lcd.print(F("1"));
    lcd.write(1);
    lcd.print(F(" 19"));
    lcd.write(0x7E);
    lcd.print(F(" 1"));
    lcd.write(0x7E);
    break;

    case 6:
    lcd.print(F("1"));
    lcd.write(1);
    lcd.print(F(" 28"));
    lcd.write(0x7E);
    lcd.print(F(" 1"));
    lcd.write(0x7E);
    break;

    case 7:
    lcd.print(F("1"));
    lcd.write(1);
    lcd.print(F(" 32"));
    lcd.write(0x7E);
    lcd.print(F(" 1"));
    lcd.write(0x7E);
    break;

    case 8:
    lcd.print(F("1"));
    lcd.write(0x7E);
    lcd.print(F(" 9"));
    lcd.write(0x7E);
    lcd.print(F(" 5"));
    lcd.write(0x7E);
    lcd.print(F(" 20"));
    lcd.write(0x7F);
    lcd.setCursor(0,1);
    lcd.print(F("5"));
    lcd.write(0x7F);
    break;

    case 9:
    lcd.print(F("4"));
    lcd.write(0x7F);
    lcd.print(F(" 20"));
    lcd.write(0x7F);
    lcd.print(F(" 5"));
    lcd.write(0x7F);
    break;

    case 10:
    lcd.print(F("1"));
    lcd.write(1);
    lcd.print(F(" 2"));
    lcd.write(0x7E);
    lcd.print(F(" 16"));
    lcd.write(0x7F);
    lcd.print(F(" 5"));
    lcd.write(0x7F);
    break;

    case 11:
    lcd.print(F("5"));
    lcd.write(1);
    lcd.print(F(" 21"));
    lcd.write(0x7E);
    lcd.print(F("8"));
    lcd.write(0x7F);
    lcd.print(F(" 8"));
    lcd.write(0x7E);
    break;

    case 12:
    lcd.print(F("7"));
    lcd.write(1);
    break;

    case 13:
    lcd.print(F("2"));
    lcd.write(1);
    lcd.print(F(" 13"));
    lcd.write(0x7E);
    break;

    case 14:
    lcd.print(F("9"));
    lcd.write(1);
    lcd.print(F(" 7"));
    lcd.write(0x7F);
    lcd.print(F(" 10"));
    lcd.write(0x7F);
    lcd.print(F(" 28"));
    lcd.write(0x7F);
    lcd.setCursor(0,1);
    lcd.print(F("2"));
    lcd.write(0x7F);
    lcd.print(F(" 6"));
    lcd.write(0x7F);
    break;

    case 15:
    lcd.print(F("6"));
    lcd.write(1);
    lcd.print(F(" 5"));
    lcd.write(0x7E);
    lcd.print(F(" 29"));
    lcd.write(0x7E);
    lcd.print(F(" 8"));
    lcd.write(0x7F);
    break;

    case 16:
    lcd.print(F("2"));
    lcd.write(0);
    lcd.print(F(" 6"));
    lcd.write(0x7F);
    break;

    case 17:
    lcd.print(F("10"));
    lcd.write(1);
    lcd.print(F(" 10"));
    lcd.write(0x7F);
    lcd.print(F(" 3"));
    lcd.write(0x7E);
    lcd.print(F(" 6"));
    lcd.write(0x7F);
    break;

    case 18:
    lcd.print(F("11"));
    lcd.write(1);
    lcd.print(F(" 5"));
    lcd.write(0x7E);
    lcd.print(F(" 3"));
    lcd.write(0x7F);
    lcd.print(F(" 6"));
    lcd.write(0x7F);
    break;

    case 19:
    lcd.print(F("10"));
    lcd.write(1);
    lcd.print(F(" 41"));
    lcd.write(0x7E);
    lcd.print(F(" 2"));
    lcd.write(0x7E);
    break;

    case 20:
    lcd.print(F("11"));
    lcd.write(0x7F);
    break;

    case 21:
    lcd.print(F("2"));
    lcd.write(0x7F);
    lcd.print(F(" 10"));
    lcd.write(0x7E);
    lcd.print(F(" 3"));
    lcd.write(0x7E);
    lcd.print(" 6");
    lcd.write(0x7F);
    break;

    case 22:
    lcd.print(F("18"));
    lcd.write(1);
    lcd.print(F(" 4"));
    lcd.write(0x7E);
    lcd.print(F(" 6"));
    lcd.write(0x7F);
    break;

    case 23:
    lcd.print(F("12"));
    lcd.write(1);
    lcd.print(F(" 17"));
    lcd.write(0x7F);
    lcd.print(F(" 4"));
    lcd.write(0x7E);
    lcd.print(F(" 6"));
    lcd.write(0x7F);
    break;
    
    default:
    lcd.print(F("ERROR:"));
    lcd.setCursor(0,1);
    lcd.print(F("SYSTEEMFOUT!"));
  }
}

void CO_route(char i)
{
  lcd.clear();
  lcd.print(F("De route naar CO"));
  lcd.setCursor(0,1);
  lcd.print(F("wordt geladen"));
  delay(3000);
  lcd.clear();
  switch (i)
  {
    //routes naar de centrale opslag van alle meetmodules
    case 2:
    lcd.print(F("2"));
    lcd.write(1);
    lcd.print(F(" 41"));
    lcd.write(0x7F);
    lcd.print(F(" 13"));
    lcd.write(0x7F);
    lcd.print(F(" 4"));
    lcd.write(0x7F);
    break;

    case 11:
    lcd.print(F("6"));
    lcd.write(1);
    lcd.print(F(" 20"));
    lcd.write(0x7E);
    lcd.print(F(" 4"));
    lcd.write(0x7E);
    break;

    case 14:
    lcd.print(F("9"));
    lcd.write(1);
    lcd.print(F(" 11"));
    lcd.write(0x7F);
    break;

    case 15:
    lcd.print(F("6"));
    lcd.write(1);
    lcd.print(F(" 4"));
    lcd.write(0x7E);
    lcd.print(F(" 28"));
    lcd.write(0x7E);
    lcd.print(F(" 9"));
    lcd.write(0x7E);
    lcd.setCursor(0, 1);
    lcd.print(F(" 4"));
    lcd.write(0x7F);
    break;

    case 19:
    lcd.print(F("24"));
    lcd.write(1);
    lcd.print(F(" 4"));
    lcd.write(0x7F);
    break;
  }
}

void MM_gevonden(char i)
{
  //voor het versturen en ontvangen van data is er gebruik gemaakt van het eigen geschreven protocol via RF communicatie
digitalWrite(yellow_pin, HIGH);
uint8_t functie = 0xE7;
hu_packet_t packet;
packet.start=HU_PROTOCOL_START_BYTE;
packet.end=HU_PROTOCOL_END_BYTE;
packet.function= functie;
switch (i)
{
  //In deze switch case wordt de juiste meetmodule in de data van het bericht gezet
  case 0:
  packet.data[0] = 12;
  break;

  case 2:
  packet.data[0] = 4;
  break;

  case 11:
  packet.data[0] = 8;
  break;

  case 14:
  packet.data[0] = 12;
  break;

  case 15:
  packet.data[0] = 16;
  break;

  case 19:
  packet.data[0] = 20;
  break;
}
packet.length= HU_PROTOCOL_LENGTH_NON_DATA + 1;
packet.destination = 0x2C;
packet.source = 0x4C;

lcd.print(F("Sending"));
hu_protocol_transmit(&rh_ask, &packet);
delay(500);
digitalWrite(yellow_pin, LOW);
}

void RF_nm_receive()
{
char a = 0;
lcd.clear();
lcd.print(F("Receiving"));
digitalWrite(green_pin, HIGH);
hu_packet_t packet;
while (a != 1)
{
//Deze loop wordt alleen beëindigd als er een bericht binnenkomt
hu_prot_receive_err_t err = hu_protocol_receive(&rh_ask, &packet);
if ((packet.function == 0xEB) && (packet.destination == 0x4C) )
{
  digitalWrite(green_pin, LOW);
  lcd.clear();
  flag = 1;
  a = 1;
}
else if((packet.function == 0xDB) && (packet.destination == 0x4C))
{
  lcd.clear();
  ack_nm();
  reset();
  a = 1;
}
}
}

void Error()
{
  lcd.clear();
  lcd.print(F("ERROR"));
  char k = 0;
  while (k < 10)
  {
    k++;
    digitalWrite(red_pin, LOW);
    delay(200);
    digitalWrite(red_pin, HIGH);
    delay(200);
  }
  lcd.clear();
}

void ack_nm()
{
lcd.print(F("Sending"));
delay(2000);
digitalWrite(yellow_pin, HIGH);
uint8_t functie = 0xEC;
hu_packet_t packet;

packet.start=HU_PROTOCOL_START_BYTE;
packet.end=HU_PROTOCOL_END_BYTE;
packet.function= functie;
packet.data[0]= 6;
packet.length= HU_PROTOCOL_LENGTH_NON_DATA + 1;
packet.destination = 0x2C;
packet.source = 0x4C;

hu_protocol_transmit(&rh_ask, &packet);
delay(500);
digitalWrite(yellow_pin, LOW);
}

void CO_gevonden()
{
lcd.print(F("Sending"));
delay(2000);
digitalWrite(yellow_pin, HIGH);
uint8_t functie = 0xF3;
hu_packet_t packet;

packet.start=HU_PROTOCOL_START_BYTE;
packet.end=HU_PROTOCOL_END_BYTE;
packet.function= functie;
packet.data[0]= 108;
packet.length= HU_PROTOCOL_LENGTH_NON_DATA + 1;
packet.destination = 0x2C;
packet.source = 0x4C;

hu_protocol_transmit(&rh_ask, &packet);
delay(500);
digitalWrite(yellow_pin, LOW);
}

void reset()
{
  //Reset Arduino
  wdt_enable(WDTO_15MS);
  while(1);
}
#endif

