#include <EEPROM.h>
#include "EEPROMAnything.h"

#define MEM_SIZE 1024

int trigPin=11;
int echoPin=12;
int duration = 0;

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

  return duration;
}

int incomingByte = 0;

int state = 0;
int counter = 0;
int sensor = 0;

int timeSeconds = 0;
unsigned long timeMilli = 0;

const int HIGH_TRIGGER = 7000; //some variaton
const int TRIGGER_DELAY = 100;

void loop() {

	// 0 -> menu (initial state)
	// 1 -> listen
	// 2 -> entering sensor
	// 3 -> leaving sensor (count)
	// 4 -> print & save :)

	switch (state) {

		case 0:

			incomingByte = Serial.read();

			if (incomingByte == '0')
			{
				timeMilli = millis();
				timeSeconds = (int)(timeMilli / 1000);
				
				state = 1;

				EEPROM_writeAnything(0, timeSeconds);

				Serial.print("Starting counter on ");
				Serial.println(timeSeconds);
			}
			else if (incomingByte == '1')
			{
				printMemory();

				Serial.println("Ready...");
			}
			else if (incomingByte == '2')
			{
				eraseMemory();

				Serial.println("Ready...");
			}
			break;
		case 1:

			sensor = getSensorData();

			if (HIGH_TRIGGER <= sensor)
			{
				state = 2; //reading

				//Serial.print("STATE 1: ");
				//Serial.print(initialSensorValue);
				//Serial.println();
			}

			break;

		case 2:

			delay(TRIGGER_DELAY);
			
			sensor = getSensorData();

			if (HIGH_TRIGGER > sensor)
			{
				state = 3;	

				/*
				Serial.print("STATE 2: ");
				Serial.print(sensor);
				Serial.println();
				*/
			}

			break;
		case 3:

			delay(TRIGGER_DELAY);

			sensor = getSensorData();

			if (HIGH_TRIGGER <= sensor)
			{
				state = 4;

				/*
				Serial.print("STATE 3: ");
				Serial.print(sensor);
				Serial.println();
				*/

				counter++;
			}

			break;

		case 4:

			timeMilli = millis();

			timeSeconds = (int)(timeMilli / 1000);


			EEPROM_writeAnything(counter, timeSeconds);

			Serial.println(counter);
			Serial.println(timeSeconds);

			state = 1;
			/*
			Serial.print(" : ");
			Serial.print(millis());
			Serial.println();
			*/
			break;
		default:

			state = 0;
	}
  }

  //raw_print_memory();
	void printMemory()   
	{
		int memCounter = 1;
		int total = 0;
		int s = 0;

		Serial.print("Start Time: ");
		Serial.println(EEPROM.read(0));

		Serial.print("Recordings : ");
		while(s = EEPROM.read(memCounter++))
		{
			Serial.print(s);
			Serial.print(" ");

			if (s > 0 ) total++;
		}
		
		Serial.println();

		Serial.print("Total Count: ");
		Serial.println(total);
	}

	void eraseMemory()
	{
	  Serial.println("");
	  Serial.println("ERASING MEMORY...");
	  for(int i = 0; i<=MEM_SIZE; i++)
	    {
	      EEPROM.write(i,0);
	    }
	}

	void setup() {

		pinMode(A0,INPUT);
		pinMode(trigPin, OUTPUT);
		pinMode(echoPin, INPUT);
		Serial.begin(9600);

		state = 0;

		Serial.println();
		Serial.println("ENTER 0 TO START COUNTING");
		Serial.println("ENTER 1 TO PRINT MEMORY");
		Serial.println("ENTER 2 TO ERASE MEMORY");
		Serial.println();
		Serial.println("Ready...");
	}

