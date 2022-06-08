#include "ArduinoJson.h"
#include "EspMQTTClient.h"

//#include "flowmeter.hpp"
//#include "flow_config.hpp"
#include "FlowMeter/flowmeter.hpp"
#include "FlowMeter/flow_config.hpp"

FlowMeter f1(_SPIN1, _FLOW1, _YFS402B);

EspMQTTClient client(_SSID, _PASS, _MQTTHOST, _CLIENTID, _MQTTPORT);

void pulseCounter() { f1.pulse_count++; }
void onConnectionEstablished(void);
void publishData(void);

void setup()
{
    // Initialize a serial connection for reporting values to the host
    Serial.begin(115200);
    
    // WiFi
    WiFi.disconnect(true);
    delay(1000);
    WiFi.begin(_SSID, _PASS);
    
    uint8_t failed_connections = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("connecting..");
        failed_connections ++;
        if (failed_connections > 20) 
        {
            Serial.println("restarting..");
            ESP.restart();
        }
    }
    
    Serial.print("Connected to ");
    Serial.println(WiFi.localIP());

    pinMode(f1.sensor_pin, INPUT_PULLUP);
    attachInterrupt(f1.sensor_pin, pulseCounter, FALLING);
}


void publish_data()
{
    if (client.isConnected())
    {
        StaticJsonDocument<200> message;
        message["key"] = _CLIENTID;

        f1.flowmeter_run();
        message["data"][f1.name]["Flow rate[LPM]"] = f1.flow_rate;
        message["data"][f1.name]["Total[mLs]"] = f1.total_mLs;
        attachInterrupt(f1.sensor_pin, pulseCounter, FALLING);
        
        client.publish(_PUBTOPIC, message.as<String>());
        serializeJsonPretty(message, Serial);
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
