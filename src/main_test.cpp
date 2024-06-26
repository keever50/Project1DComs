#ifdef testhayan
#include <Arduino.h>
#include <hu_protocol.h>
//hayan//
//resv pin3 
//zender pin11

#include <DFRobot_RGBLCD1602.h>
#include <RH_ASK.h>

RH_ASK rh_ask(500, 3, 11, 0, false); // Bitrate, receive pin, transmit pin, select pin(unused), select inverse(unused)

int ThermistorPin = A1;
int Vo; 
#define R1  10000
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

int LDRPin = A0;
int LDRvalue = 0;

const int numReadings = 10; // Number of readings to store
int thermistorReadings[numReadings];
int ldrReadings[numReadings];
int potReadings[numReadings];
int thermistorIndex = 0;
int ldrIndex = 0;
int potIndex = 0;


void light();
void Temp();
void pot();
void storeReading(int value, int* readings, int* index);
void printStats();

void sensoren_verwerken();

void res();
void zend(uint8_t);

void setup()
{
  Serial.begin(9600);
  rh_ask.init();
}

void loop()
{
  // for (int i = 0; i < numReadings; i++) 
  // {
  //   Temp();
  //   light();
  //   pot();
  //   printStats(); // Print statistics after collecting 10 readings
  //   delay(1000); // Delay before taking the next set of readings
  // }
}

void zend(uint8_t source_van_aanvrager)
{
  hu_packet_t packet;
  packet.start=HU_PROTOCOL_START_BYTE;
  packet.end=HU_PROTOCOL_END_BYTE;
  packet.function=HU_PROTOCOL_FUNTION_OVERDRAGEN_MEETWAARDES;
  packet.length=(HU_PROTOCOL_MIN_PACKET_LEN+(4*sizeof(float));
  packet.destination = source_van_aanvrager;
  packet.source = 0//

//data   // mijn functie in de header


  hu_protocol_transmit( &rh_ask, &packet );
}


void res(void)
{
  hu_packet_t packet;
  uint8_t functie, data_length;
  uint8_t *data;

  hu_prot_receive_err_t ret = hu_protocol_receive( &rh_ask, &packet );
  if(ret == HU_PROT_RECEIVE_RECEIVED)
  {
    function = packet.function;
    return;
  }

  if (functie == HU_PROTOCOL_FUNCTION_OPVRAAG_MEETWAARDES)
  {
    delay(500);
    //zend ...FC06(HU_PROTOCOL_FUNTION_OVERDRAGEN_MEETWAARDES)
    
    
  }
}

void sensoren_verwerken()
{
  for (int i = 0; i < numReadings; i++) {
  Temp();
  light();
  pot();
  printStats(); // Print statistics after collecting 10 readings  //voor mij
  delay(1000); // Delay before taking the next set of readings
  }
}


void light() 
{
  LDRvalue = analogRead(LDRPin);
  storeReading(LDRvalue, ldrReadings, &ldrIndex);
}

void Temp() 
{
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  storeReading((int)T, thermistorReadings, &thermistorIndex);
}

void pot() 
{
  int sensorValue = analogRead(A2);
  storeReading(sensorValue, potReadings, &potIndex);
}

void storeReading(int value, int* readings, int* index) 
{
  readings[*index] = value;
  *index = (*index + 1) % numReadings; // Move to the next index, wrapping around if necessary
}

void calculateStats(int* readings, int* max, int* min, float* avg) 
{
*max = readings[0];
*min = readings[0];
*avg = 0;
  
for (int i = 0; i < numReadings; i++)
{
if (readings[i] > *max) {
  *max = readings[i];
}
if (readings[i] < *min) {
  *min = readings[i];
}
*avg += readings[i];
}  
*avg /= numReadings;
}

void printStats() 
{
  int maxTemp, minTemp;
  float avgTemp;
  calculateStats(thermistorReadings, &maxTemp, &minTemp, &avgTemp);
    
  int maxLDR, minLDR;
  float avgLDR;
  calculateStats(ldrReadings, &maxLDR, &minLDR, &avgLDR);
    
  int maxPot, minPot;
  float avgPot;
  calculateStats(potReadings, &maxPot, &minPot, &avgPot);
    
  Serial.println("Temperature:");
  Serial.print("Max: ");
  Serial.println(maxTemp);
  Serial.print("Min: ");
  Serial.println(minTemp);
  Serial.print("Avg: ");
  Serial.println(avgTemp);
    
  Serial.println("LDR:");
  Serial.print("Max: ");
  Serial.println(maxLDR);
  Serial.print("Min: ");
  Serial.println(minLDR);
  Serial.print("Avg: ");
  Serial.println(avgLDR);
    
  Serial.println("Potentiometer:");
  Serial.print("Max: ");
  Serial.println(maxPot);
  Serial.print("Min: ");
  Serial.println(minPot);
  Serial.print("Avg: ");
  Serial.println(avgPot);
  Serial.println();
}


#endif