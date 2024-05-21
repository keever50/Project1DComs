#ifdef NAVIGATIE_TEST
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <LiquidCrystal_I2C.h>

#define PN532_IRQ   (2)
#define PN532_RESET (3)

char MM = 0;

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
LiquidCrystal_I2C lcd(0x027, 16, 2);

void route(char i);
char kaartbepaler();
void stapper();

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
  Serial.begin(115200);
  nfc.begin();
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, arrow_up);
  lcd.createChar(1, arrow_down);
  uint32_t status = nfc.getFirmwareVersion();
  if (!status) {
    Serial.print("Geen bord gedetecteerd!");
    while (1);
  }
  lcd.setCursor(0,0);
  lcd.print("Scan een");
  lcd.setCursor(0, 1);
  lcd.print("kaart:");
}

void loop(void)
{
  kaartbepaler();
}
char kaartbepaler()
{
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

  UID = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, UID_tag, &UIDLengte);

  if (UID)
  {
    lcd.clear();
    lcd.print("Kaart gescand");
    delay(2000);
    lcd.clear();
    Serial.println("De gevonde UID is:");

    for (uint8_t j=0; j<UIDLengte; j++)
    {
      Serial.print("0x");
      Serial.print(UID_tag[j], HEX);
      Serial.print(" ");
    }
    Serial.println("");

    switch (UID_tag[0])
    {
      case 0x92:
      if (memcmp(UID_tag, startlocatie1, 4)== 0)
      {
        i=1;
        route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie19, 4) == 0)
      {
        MM++;
        i=19;
        route(i);
        break;
      }

      case 0x1C:
      if (memcmp(UID_tag, startlocatie2, 4)== 0)
      {
        MM++;
        i=2;
        route(i);
        break;
      }

      case 0x02:
      if (memcmp(UID_tag, startlocatie3, 4)== 0)
      {
        i=3;
        route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie6, 4)== 0)
      {
        i=6;
        route(i);
        break;
      }

      case 0xD2:
      if (memcmp(UID_tag, startlocatie4, 4)== 0)
      {
        i=4;
        route(i);
        break;
      }

      case 0xC2:
      if (memcmp(UID_tag, startlocatie5, 4)== 0)
      {
        i=5;
        route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie18, 4) == 0)
      {
        i=18;
        route(i);
        break;
      }

      case 0x52:
      if (memcmp(UID_tag, startlocatie7, 4)== 0)
      {
        i=7;
        route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie10, 4)== 0)
      {
        i=10;
        route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie22, 4)== 0)
      {
        i=22;
        route(i);
        break;
      }

      case 0x72:
      if (memcmp(UID_tag, startlocatie8, 4)== 0)
      {
        i=8;
        route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie12, 4)== 0)
      {
        i=12;
        route(i);
        break;
      }

      case 0x32:
      if (memcmp(UID_tag, startlocatie9, 4)== 0)
      {
        i=9;
        route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie17, 4)== 0)
      {
        i=17;
        route(i);
        break;
      }
      if (memcmp(UID_tag, startlocatie23, 4)== 0)
      {
        i=23;
        route(i);
        break;
      }

      case 0xB2:
      if (memcmp(UID_tag, startlocatie11, 4)== 0)
      {
        MM++;
        i=11;
        route(i);
        break;
      }

      case 0x12:
      if (memcmp(UID_tag, startlocatie13, 4)== 0)
      {
        i=13;
        route(i);
        break;
      }

      case 0xA2:
      if (memcmp(UID_tag, startlocatie14, 4)== 0)
      {
        MM++;
        i=14;
        route(i);
        break;
      }

      case 0xE2:
      if (memcmp(UID_tag, startlocatie15, 4)== 0)
      {
        MM++;
        i=15;
        route(i);
        break;
      }

      case 0xFC:
      if (memcmp(UID_tag, startlocatie16, 4)== 0)
      {
        i=16;
        route(i);
        break;
      }

      case 0xF2:
      if (memcmp(UID_tag, startlocatie20, 4)== 0)
      {
        i=20;
        route(i);
        break;
      }

      case 0x82:
      if (memcmp(UID_tag, startlocatie21, 4)== 0)
      {
        i=21;
        route(i);
        break;
      }

      case 0xF3:
      if (memcmp(UID_tag, testkaart1, 4)== 0)
      {
        i=0;
        route(i);
        break;
      }

      case 0xA5:
      if (memcmp(UID_tag, testkaart2, 4)== 0)
      {
        i=0;
        route(i);
        break;
      }
      default:
      i=0;
      route(i);
      break;
    }
}
}

void route(char i)
{
    lcd.setCursor(0,0);
    lcd.print("Route wordt");
    lcd.setCursor(0,1);
    lcd.print("berekend.....");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Route gevonden!");
    delay(1000);
    lcd.clear();
  switch (i)
  {
    case 0:
    lcd.print("geen geldige");
    lcd.setCursor(0,1);
    lcd.print("route gevonden");
    break;

    case 1:
    lcd.print("2");
    lcd.write(1);
    lcd.print(" 9");
    lcd.write(0x7E);
    lcd.print(" 3");
    lcd.write(0x7F);
    break;

    case 2:
    lcd.print("1");
    lcd.write(1);
    lcd.print(" 44");
    lcd.write(0x7E);
    lcd.print(" 11");
    lcd.write(0x7E);
    break;

    case 3:
    lcd.print("1");
    lcd.write(1);
    lcd.print(" 5");
    lcd.write(0x7E);
    lcd.print(" 1");
    lcd.write(0x7E);
    break;

    case 4:
    lcd.print("1");
    lcd.write(1);
    lcd.print(" 12");
    lcd.write(0x7E);
    lcd.print(" 1");
    lcd.write(0x7E);
    break;

    case 5:
    lcd.print("1");
    lcd.write(1);
    lcd.print(" 19");
    lcd.write(0x7E);
    lcd.print(" 1");
    lcd.write(0x7E);
    break;

    case 6:
    lcd.print("1");
    lcd.write(1);
    lcd.print(" 28");
    lcd.write(0x7E);
    lcd.print(" 1");
    lcd.write(0x7E);
    break;

    case 7:
    lcd.print("1");
    lcd.write(1);
    lcd.print(" 32");
    lcd.write(0x7E);
    lcd.print(" 1");
    lcd.write(0x7E);
    break;

    case 8:
    break;

    case 9:
    break;

    case 10:
    break;

    case 11:
    break;

    case 12:
    break;

    case 13:
    break;

    case 14:
    break;

    case 15:
    break;

    case 16:
    break;

    case 17:
    break;

    case 18:
    break;

    case 19:
    break;

    case 20:
    break;

    case 21:
    break;

    case 22:
    break;

    case 23:
    break;
    
    default:
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ERROR:");
    lcd.setCursor(0,1);
    lcd.print("SYSTEEMFOUT!");
  }
}
#endif

