#include <EEPROM.h>
#include "EEPROMAnything.h"
#define MEM_SIZE 1024

int sensor = 0;
int sensorValue;
int lowTrigger = 0;
int highTrigger = 150;

int timeNow;
unsigned long timeRaw;
int minStamp =0;
int secStamp;
int timeOffset;
int latestMin;
int timeout = 6000000000;
int timeSlot;

int countThis;
int totalCount;
int incomingByte = 0;

int trigPin=11;
int echoPin=12;
int sensorData;
int duration;
int cm;
int in;

void setup() {
pinMode(A0,INPUT);
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
Serial.begin(9600);

EEPROM_readAnything(0,totalCount);
EEPROM_readAnything((totalCount*2)+1,timeOffset);

if(totalCount<0)
  {
    eraseMemory();
  }

  Serial.println("Hello.");
  Serial.println("___________________________________________________");
  Serial.println("ENTER 1 TO PRINT MEMORY");
  Serial.println("ENTER 2 TO ERASE MEMORY");
  Serial.println("___________________________________________________");
}

void loop() {
  delay(200);
  //Detection Start
  sensorValue = getSensorData();
  if(sensorValue < highTrigger)
    {
          countThis = 1;
    }

  //Reading accepted and recorded
  if((sensorValue > highTrigger) && (countThis == 1))
    {
      totalCount++;
      timeSlot = totalCount*2;
      timeRaw = millis()/1000;
      timeNow = timeRaw;
      EEPROM_writeAnything(0, totalCount);
      EEPROM_writeAnything(timeSlot, timeNow);
      Serial.print("Current Count = ");
      Serial.println(totalCount);
      Serial.print("Timestamp = ");
      Serial.println(timeNow);
    
    //reset variables
    countThis = 0;
    }
  
  
  if (Serial.available() > 0)
    {
      incomingByte = Serial.read(); // read the incoming byte:
        if (incomingByte == '1')
          {
            printMemory();
          }
        if (incomingByte == '2') 
          {
            //Serial.println("");
            Serial.println("ARE YOU SURE YOU WANT TO ERASE THE MEMORY? Enter Y/N");
          }
        if (incomingByte == 'N' || incomingByte == 'n')
          {
            Serial.println("MEMORY ERASE CANCELLED");
            Serial.println("___________________________________________________");
          }
        if (incomingByte == 'Y' || incomingByte == 'y') 
          {
            eraseMemory();  
            printMemory();
          }
    }

}
int getSensorData()
{
 
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(12);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // convert the time into a distance
  sensorData = (duration/2) / 29.1;
  return sensorData;
  delay(100);
}


  void printMemory()   
    {
      if (totalCount > 0)
        {
          Serial.println("");
          Serial.println("Count , Time (Minutes)");
          for (int i=1; i<= totalCount; i++)
            {
              Serial.print(i);
              Serial.print(" ");
              long y = EEPROM.read(2*i);
              Serial.print(y);
              //Serial.print(" , ");
              latestMin = y;    
            }
        }

      Serial.println(""); 
      Serial.print("Total Count, ");
      Serial.println(totalCount);//read memory
      Serial.print("Total Minutes Measured, ");
      Serial.println(latestMin);
      Serial.print("Traffic Rate (users per min), ");
      if ((totalCount/latestMin) <= 0)
        {
          Serial.println("0");
        }
      else
        {
          Serial.println(totalCount/latestMin);
        }
    Serial.println("___________________________________________________");
    }

  void raw_print_memory()
    {
      Serial.println("EEPROM REPORT: ");
      Serial.print("[");
      for (int i = 0; i <= MEM_SIZE; i++)
        {
          int h = EEPROM.read(i);
          Serial.print(h);
          if (i < MEM_SIZE)
          Serial.print(",");
        }
  Serial.println("]");

}


  void eraseMemory()
    {
      Serial.println("");
      Serial.println("ERASING MEMORY...");
      for(int i = 0; i<=MEM_SIZE; i++)
        {
          EEPROM.write(i,0);
        }
        totalCount=0;
        timeOffset=0;
        latestMin=0;
    }

