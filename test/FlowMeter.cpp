#include <Arduino_JSON.h>

#include "flowmeter.hpp"
#include "config.hpp"

JSONVar data;

FlowMeter f1;

void pulseCounter() { f1.pulse_count++; };

void setup()
{
    // Initialize a serial connection for reporting values to the host
    Serial.begin(115200);
    f1.setSensorPin(_SPIN1);
    f1.setCalibrationFactor( _SS_FLOW );
    pinMode(f1.sensor_pin, INPUT_PULLUP);
    digitalWrite(f1.sensor_pin, HIGH);

    // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
    // Configured to trigger on a FALLING state change (transition from HIGH
    // state to LOW state)
    attachInterrupt(f1.sensor_pin, pulseCounter, FALLING);
}

/**
 * Main program loop
 */
void loop()
{
    f1.run();
    data[0] = f1.data;
    Serial.println(JSON.stringify(data));
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(f1.sensor_pin, pulseCounter, FALLING);
    
}
