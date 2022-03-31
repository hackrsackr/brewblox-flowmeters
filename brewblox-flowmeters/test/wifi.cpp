#include <WiFi.h>
#include <EspMQTTClient.h>
#include <Esp32HTTPUpdateServer.h>

#include "config.h"

void setup()
{
  Serial.begin(115200);
  delay(10);
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

}