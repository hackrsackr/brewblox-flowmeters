#include "Arduino.h"
#include "ArduinoJson.h"
#include "EspMQTTClient.h"

//int sensorInterrupt1 = 26; // 0 = digital pin 2
int sg_flowPin = 35;
int sg_relayPin = 34;

/* 4380 pulses per liter
 * f = 4380 * Q(L/s);
 * Q(L/s) = f/4380
 * Q(L/m) = f*60/4380 = f/73
 * 
 * 4380/60 = 73
*/ 

// FOR YF-S402
// The hall-effect flow sensor outputs approximately 4380 pulses per
// liter of flow. 4380/60 = 73 pulses per liter/minute.
int sg_calibrationFactor = 73;

volatile int sg_pulseCount;

float sg_flowRate;

unsigned int sg_flowMilliLiters;
unsigned long sg_totalMilliLiters;
unsigned long sg_totalpulseCount;

unsigned long sg_oldTime;

void sg_pulseCounter() { sg_pulseCount++; }

void setup()
{
  // Initialize a serial connection for reporting values to the host
  Serial.begin(115200);
  pinMode(sg_flowPin, INPUT_PULLUP);
  pinMode(sg_relayPin, OUTPUT);
  digitalWrite(sg_flowPin, HIGH);
  digitalWrite(sg_relayPin, HIGH);

  sg_pulseCount = 0;
  sg_flowRate = 0.0;
  sg_flowMilliLiters = 0;
  sg_totalMilliLiters = 0;
  sg_totalpulseCount = 0;
  sg_oldTime = 0;

  attachInterrupt(sg_flowPin, sg_pulseCounter, FALLING);
}

void loop()
{
  StaticJsonDocument<100> doc;
  if ((millis() - sg_oldTime) > 1000) // Only process counters once per second
  {
    detachInterrupt(sg_flowPin);
    sg_flowRate = ((1000.0 / (millis() - sg_oldTime)) * sg_pulseCount) / sg_calibrationFactor;
    sg_oldTime = millis();
    sg_flowMilliLiters = (sg_flowRate / 60) * 1000;
    sg_totalMilliLiters += sg_flowMilliLiters;

    doc["Flow rate-1 [l/min]"] = sg_flowRate;
    doc["Total-1 [mL]"] = sg_totalMilliLiters;

    // Reset the pulse counter so we can start incrementing again
    sg_pulseCount = 0;

    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorPin1, pulseCounter1, FALLING);

    // Enable the interrupt again now that we've finished sending output
    serializeJsonPretty(doc, Serial);
  }
}