#include <WiFi.h>
#include <EspMQTTClient.h>
#include <Esp32HTTPUpdateServer.h>
#include <Arduino_JSON.h>

#include "flowmeter.h"
#include "config.h"


EspMQTTClient client(_SSID, _PASS, _MQTTHOST, _CLIENTID, _MQTTPORT);
String message = "{\"key\": \"flow-meters\",\"data\": 48.75}";

void onConnectionEstablished()
{
    client.publish(_PUBTOPIC, message);
}

void setup()
{
  Serial.begin(115200);
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
}

void loop()
{
    client.loop();
    delay(10);

    while (client.isConnected())
    {
      client.publish(_PUBTOPIC, message);
      Serial.println(message);
      delay(5000);
    }
}