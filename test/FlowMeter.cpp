// Needs work



#include "Arduino.h"

class FlowMeter {
    public:
        int sensorPin, sensorInterrupt; 
        volatile int pulseCount;
        float calibrationFactor, flowRate;
        unsigned int flowMilliLiters;
        unsigned long totalMilliLiters, totalpulseCount, oldTime;

        void pulseCounter() { pulseCount++; };
        void setSensorPin(int pin) { sensorPin = pin; }
        void setCalibrationFactor(float cal) { calibrationFactor = cal; }
        float getFlowRate() {((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;}

};

FlowMeter f1;

void setup()
{
    // Initialize a serial connection for reporting values to the host
    Serial.begin(115200);
    f1.setSensorPin(26);    
    pinMode(f1.sensorPin, INPUT_PULLUP);
    digitalWrite(f1.sensorPin, HIGH);

    f1.pulseCount = 0;
    f1.flowRate = 0.0;
    f1.flowMilliLiters = 0;
    f1.totalMilliLiters = 0;
    f1.oldTime = 0;

    f1.calibrationFactor = 9.93;

    // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
    // Configured to trigger on a FALLING state change (transition from HIGH
    // state to LOW state)
    attachInterrupt(f1.sensorPin, f1.pulseCounter, FALLING);
}

/**
 * Main program loop
 */
void loop()
{
    if ((millis() - f1.oldTime) > 1000) // Only process counters once per second
    {
        // Disable the interrupt while calculating flow rate and sending the value to
        // the host
        detachInterrupt(f1.sensorPin);

        // Because this loop may not complete in exactly 1 second intervals we calculate
        // the number of milliseconds that have passed since the last execution and use
        // that to scale the output. We also apply the calibrationFactor to scale the output
        // based on the number of pulses per second per units of measure (litres/minute in
        // this case) coming from the sensor.
        //flowRate = ((1000.0 / (millis() - oldTime)) * fpulseCount) / f1.calibrationFactor;
        f1.getFlowRate();
        // Note the time this processing pass was executed. Note that because we've
        // disabled interrupts the millis() function won't actually be incrementing right
        // at this point, but it will still return the value it was set to just before
        // interrupts went away.
        f1.oldTime = millis();

        // Divide the flow rate in litres/minute by 60 to determine how many litres have
        // passed through the sensor in this 1 second interval, then multiply by 1000 to
        // convert to millilitres.
        f1.flowMilliLiters = (f1.flowRate / 60) * 1000;
        f1.totalMilliLiters += f1.flowMilliLiters;

        // Print the flow rate for this second in litres / minute
        Serial.print("Flow rate: ");
        Serial.print(f1.flowRate);
        Serial.print(" L/min");
        Serial.print("\t"); // Print tab space

        // Print the cumulative total of litres since starting
        Serial.print("Total: ");
        Serial.print(f1.totalMilliLiters);
        Serial.println(" mL");
        // Reset the pulse counter so we can start incrementing again
        f1.pulseCount = 0;

        // Enable the interrupt again now that we've finished sending output
        attachInterrupt(f1.sensorPin, f1.pulseCounter, FALLING);
    }
}
