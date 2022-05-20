#include "ArduinoJson.h"
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

        f1.flowmeter_run();
        attachInterrupt(f1.sensor_pin, pulseCounter1, FALLING);

        f2.flowmeter_run();
        attachInterrupt(f2.sensor_pin, pulseCounter2, FALLING);

        DynamicJsonDocument message(256);

        message["key"] = _CLIENTID;
        message["data"]["f1"] = f1.flow_data;
        message["data"]["f2"] = f2.flow_data;

        //String jsonString = message.as<String>();
        client.publish(_PUBTOPIC, message.as<String>());

        serializeJsonPretty(message, Serial);
        Serial.println();

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
