#include "Arduino.h"
#include "ArduinoJson.h"
#include "EspMQTTClient.h"

//int sensorInterrupt1 = 26; // 0 = digital pin 2
int sensorPin1 = 34;
int sensorPin2 = 35;

// The hall-effect flow sensor outputs approximately 596 pulses per
// liter of flow. 596/60 = 9.93 pulses per liter/minute.
float calibrationFactor1 = 9.93;
float calibrationFactor2 = 9.93;

volatile int pulseCount1, pulseCount2;

float flowRate1, flowRate2;

unsigned int flowMilliLiters1, flowMilliLiters2;
unsigned long totalMilliLiters1, totalMilliLiters2;
unsigned long totalpulseCount1, totalpulseCount2;

unsigned long oldTime1, oldTime2;

void pulseCounter1() { pulseCount1++; }
void pulseCounter2() { pulseCount2++; }

void setup()
{
  // Initialize a serial connection for reporting values to the host
  Serial.begin(115200);
  pinMode(sensorPin1, INPUT_PULLUP);
  pinMode(sensorPin2, INPUT_PULLUP);
  digitalWrite(sensorPin1, HIGH);
  digitalWrite(sensorPin2, HIGH);

  pulseCount1 = 0;
  pulseCount2 = 0;
  flowRate1 = 0.0;
  flowRate2 = 0.0;
  flowMilliLiters1 = 0;
  flowMilliLiters2 = 0;
  totalMilliLiters1 = 0;
  totalMilliLiters2 = 0;
  totalpulseCount1 = 0;
  totalpulseCount2 = 0;
  oldTime1 = 0;
  oldTime2 = 0;

  attachInterrupt(sensorPin1, pulseCounter1, FALLING);
  attachInterrupt(sensorPin2, pulseCounter2, FALLING);
}

void loop()
{
  StaticJsonDocument<100> doc;
  if ((millis() - oldTime1) > 1000) // Only process counters once per second
  {
    detachInterrupt(sensorPin1);
    flowRate1 = ((1000.0 / (millis() - oldTime1)) * pulseCount1) / calibrationFactor1;
    oldTime1 = millis();
    flowMilliLiters1 = (flowRate1 / 60) * 1000;
    totalMilliLiters1 += flowMilliLiters1;

    doc["Flow rate-1 [l/min]"] = flowRate1;
    doc["Total-1 [mL]"] = totalMilliLiters1;

    // Reset the pulse counter so we can start incrementing again
    pulseCount1 = 0;

    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorPin1, pulseCounter1, FALLING);

    detachInterrupt(sensorPin2);
    flowRate2 = ((1000.0 / (millis() - oldTime2)) * pulseCount2) / calibrationFactor2;
    oldTime2 = millis();
    flowMilliLiters2 = (flowRate2 / 60) * 1000;
    totalMilliLiters2 += flowMilliLiters2;

    doc["Flow rate-2 [l/min]"] = flowRate2;
    doc["Total-2 [mL]"] = totalMilliLiters2;
    
    // Reset the pulse counter so we can start incrementing again
    pulseCount2 = 0;

    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorPin2, pulseCounter2, FALLING);
    serializeJsonPretty(doc, Serial);
  }
  Serial.println("");
  delay(5000);
}