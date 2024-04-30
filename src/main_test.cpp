#ifdef testhayan
#include <Arduino.h>

//hayan//
//resv pin3 
// zender pin11


#include <DFRobot_LCD.h>


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

void setup()
{
Serial.begin(9600);
}

void loop()
{
    
}

#endif