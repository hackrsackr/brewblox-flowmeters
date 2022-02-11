#include "Arduino.h"
#include "ArduinoJson.h"
#include "EspMQTTClient.h"

//int sensorInterrupt1 = 26; // 0 = digital pin 2
int sensorPin1 = 34;
int sensorPin2 = 35;
int sensorPin3 = 36;

// The hall-effect flow sensor outputs approximately 596 pulses per
// liter of flow. 596/60 = 9.93 pulses per liter/minute.
float calibrationFactor1 = 9.93;
float calibrationFactor2 = 9.93;
float calibrationFactor3 = 9.93;

volatile int pulseCount1, pulseCount2, pulseCount3;

float flowRate1, flowRate2, flowRate3;

unsigned int flowMilliLiters1, flowMilliLiters2, flowMilliLiters3;
unsigned long totalMilliLiters1, totalMilliLiters2, totalMilliLiters3;
unsigned long totalpulseCount1, totalpulseCount2, totalpulseCount3;

unsigned long oldTime1, oldTime2, oldTime3;

void pulseCounter1() { pulseCount1++; }
void pulseCounter2() { pulseCount2++; }
void pulseCounter3() { pulseCount3++; }

void setup()
{
  // Initialize a serial connection for reporting values to the host
  Serial.begin(115200);
  pinMode(sensorPin1, INPUT_PULLUP);
  pinMode(sensorPin2, INPUT_PULLUP);
  pinMode(sensorPin3, INPUT_PULLUP);
  digitalWrite(sensorPin1, HIGH);
  digitalWrite(sensorPin2, HIGH);
  digitalWrite(sensorPin3, HIGH);

  pulseCount1 = 0;
  pulseCount2 = 0;
  pulseCount3 = 0;
  flowRate1 = 0.0;
  flowRate2 = 0.0;
  flowRate3 = 0.0;
  flowMilliLiters1 = 0;
  flowMilliLiters2 = 0;
  flowMilliLiters3 = 0;
  totalMilliLiters1 = 0;
  totalMilliLiters2 = 0;
  totalMilliLiters3 = 0;
  totalpulseCount1 = 0;
  totalpulseCount2 = 0;
  totalpulseCount3 = 0;
  oldTime1 = 0;
  oldTime2 = 0;
  oldTime3 = 0;

  attachInterrupt(sensorPin1, pulseCounter1, FALLING);
  attachInterrupt(sensorPin2, pulseCounter2, FALLING);
  attachInterrupt(sensorPin3, pulseCounter3, FALLING);
}

void loop()
{
  StaticJsonDocument<100> doc;
  if ((millis() - oldTime1) > 1000) // Only process counters once per second
  {
    ///////////////////////////////////////////////////////////////////////////
    // Disable the interrupt while calculating flow rate
    detachInterrupt(sensorPin1);

    // Calculate flow rate
    flowRate1 = ((1000.0 / (millis() - oldTime1)) * pulseCount1) / calibrationFactor1;
    oldTime1 = millis();
    flowMilliLiters1 = (flowRate1 / 60) * 1000;
    totalMilliLiters1 += flowMilliLiters1;

    // Create JsonObject to contain data for flowmeter
    JsonObject flow_meter1 = doc.createNestedObject("flow-meter1");
    flow_meter1["L/min"] = flowRate1;
    flow_meter1["Total"] = totalMilliLiters1;

    // Reset the pulse counter so we can start incrementing again
    pulseCount1 = 0;

    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorPin1, pulseCounter1, FALLING);

    ///////////////////////////////////////////////////////////////////////////
    // Disable the interrupt while calculating flow rate
    detachInterrupt(sensorPin2);
    
    // Calculate flow rate
    flowRate2 = ((1000.0 / (millis() - oldTime2)) * pulseCount2) / calibrationFactor2;
    oldTime2 = millis();
    flowMilliLiters2 = (flowRate2 / 60) * 1000;
    totalMilliLiters2 += flowMilliLiters2;

    // Create JsonObject to contain data for flowmeter
    JsonObject flow_meter2 = doc.createNestedObject("flow-meter2");
    flow_meter2["L/min"] = flowRate2;
    flow_meter2["Total"] = totalMilliLiters2;
    
    // Reset the pulse counter so we can start incrementing again
    pulseCount2 = 0;

    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorPin2, pulseCounter2, FALLING);

    ///////////////////////////////////////////////////////////////////////////
    // Disable the interrupt while calculating flow rate
    detachInterrupt(sensorPin3);

    // Calculate flow rate
    flowRate3 = ((1000.0 / (millis() - oldTime3)) * pulseCount3) / calibrationFactor3;
    oldTime3 = millis();
    flowMilliLiters3 = (flowRate3 / 60) * 1000;
    totalMilliLiters3 += flowMilliLiters3;

    // Create JsonObject to contain data for flowmeter
    JsonObject flow_meter3 = doc.createNestedObject("flow-meter3");
    flow_meter3["L/min"] = flowRate3;
    flow_meter3["Total"] = totalMilliLiters3;
    
    // Reset the pulse counter so we can start incrementing again
    pulseCount3 = 0;

    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorPin3, pulseCounter3, FALLING);

    ///////////////////////////////////////////////////////////////////////////
    // Output data as a Json string 
    serializeJson(doc, Serial);
  }
  Serial.println("");
  delay(5000);
}