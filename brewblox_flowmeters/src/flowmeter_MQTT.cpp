#include <Arduino.h>
#include <Arduino_JSON.h>
#include <EspMQTTClient.h>
#include <Esp32HTTPUpdateServer.h>

#include "flowmeter.h"
#include "config.h"

FlowMeter f1, f2;
JSONVar data, message;
EspMQTTClient client(_SSID, _PASS, _MQTTHOST, _CLIENTID, _MQTTPORT);

void pulseCounter1() { f1.pulse_count++; }
void pulseCounter2() { f2.pulse_count++; }

void onConnectionEstablished(void);

void setup()
{
    // Initialize a serial connection for reporting values to the host
    Serial.begin(115200);
    
    // WiFi
    WiFi.begin(_SSID, _PASS);
    Serial.println("");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(_SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    client.enableHTTPWebUpdater();
    client.enableOTA();
    //client.enableDebuggingMessages();

    f1.sensor_pin = _SPIN1;
    f1.name       = _FLOW1;
    f1.cal_factor = _YF_S302;

    f2.sensor_pin = _SPIN2;
    f2.name       = _FLOW2;
    f2.cal_factor = _YF_S302;

    pinMode(f1.sensor_pin, INPUT_PULLUP);
    pinMode(f2.sensor_pin, INPUT_PULLUP);
    digitalWrite(f1.sensor_pin, HIGH);
    digitalWrite(f2.sensor_pin, HIGH);

    attachInterrupt(f1.sensor_pin, pulseCounter1, FALLING);
    attachInterrupt(f2.sensor_pin, pulseCounter2, FALLING);
}

void loop()
{
    client.loop();

    f1.flowmeter_run();
    data[0] = f1.flow_data;

    f2.flowmeter_run();
    data[1] = f2.flow_data;

    attachInterrupt(f1.sensor_pin, pulseCounter1, FALLING);
    attachInterrupt(f2.sensor_pin, pulseCounter2, FALLING);
    
    //Serial.println(JSON.stringify(data));
    //client.publish(_PUBTOPIC, JSON.stringify(data));

    message["key"] = _CLIENTID;
    message["data"] = data;
    
    Serial.println(message);
    client.publish(_PUBTOPIC, JSON.stringify(message));

    delay(5000);
}

void onConnectionEstablished()
{
    client.publish(_PUBTOPIC, JSON.stringify(message));
}
