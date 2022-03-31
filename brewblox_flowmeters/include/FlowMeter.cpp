#include "Arduino_JSON.h"
#include "EspMQTTClient.h"
#include "Esp32HTTPUpdateServer.h"

#include "flowmeter.h"
#include "flow_config.h"

FlowMeter f1;
JSONVar message;

void pulseCounter() { f1.pulse_count++; }

void setup()
{
    // Initialize a serial connection for reporting values to the host
    Serial.begin(115200);

    // FlowMeter setup
    f1.sensor_pin = _SPIN1;
    f1.name       = _FLOW1;
    f1.cal_factor = _YFS402B;

    pinMode(f1.sensor_pin, INPUT_PULLUP);
    digitalWrite(f1.sensor_pin, HIGH);
    attachInterrupt(f1.sensor_pin, pulseCounter, FALLING);
}

void loop()
{
    f1.flowmeter_run();
    attachInterrupt(f1.sensor_pin, pulseCounter, FALLING);

    message["key"] = _CLIENTID;
    message["data"] = f1.flow_data;

    JSON.stringify(message);
    Serial.println(message);
    delay(5000);
}
