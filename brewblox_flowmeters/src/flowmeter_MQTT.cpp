#include "Arduino_JSON.h"
#include "EspMQTTClient.h"
#include "ESP32HTTPUpdateServer.h"

#include "flowmeter.hpp"
#include "flow_config.hpp"

EspMQTTClient client(_SSID, _PASS, _MQTTHOST, _CLIENTID, _MQTTPORT);

FlowMeter f1(_SPIN1, _FLOW1, _YFS402B); 
FlowMeter f2(_SPIN2, _FLOW2, _YFS402B); 

void pulseCounter1() { f1.pulse_count++; }
void pulseCounter2() { f2.pulse_count++; }

void onConnectionEstablished(void);
void publishData(void);

void setup()
{
    // Initialize a serial connection for reporting values to the host
    Serial.begin(115200);
    
    client.enableHTTPWebUpdater();
    client.setMaxPacketSize(4000);
    client.enableOTA();
    // client.enableDebuggingMessages();
    
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
    
    pinMode(f1.sensor_pin, INPUT_PULLUP);
    pinMode(f2.sensor_pin, INPUT_PULLUP);

    digitalWrite(f1.sensor_pin, HIGH);
    digitalWrite(f2.sensor_pin, HIGH);
    
    attachInterrupt(f1.sensor_pin, pulseCounter1, FALLING);
    attachInterrupt(f2.sensor_pin, pulseCounter2, FALLING);
}


void publish_data()
{
    if (client.isConnected())
    {
        JSONVar data;
        JSONVar message;

        f1.flowmeter_run();
        data[0] = f1.flow_data;
        attachInterrupt(f1.sensor_pin, pulseCounter1, FALLING);

        f2.flowmeter_run();
        data[1] = f2.flow_data;
        attachInterrupt(f2.sensor_pin, pulseCounter2, FALLING);
        
        message["key"] = _CLIENTID;
        message["data"] = data;

        client.publish(_PUBTOPIC, JSON.stringify(message));

        Serial.println(message);
        Serial.println("");

        delay(5000);
    }
}

void onConnectionEstablished()
{
    publish_data();    
}

void loop()
{
    client.loop();
    publish_data();
}