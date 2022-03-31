#include <Arduino_JSON.h>
#include <Arduino.h>
#include <EspMQTTClient.h>
#include <Esp32HTTPUpdateServer.h>

#include "flowmeter.h"
#include "config.h"

#define NUMBER_OF_FLOWMETERS 3

EspMQTTClient client(_SSID, _PASS, _MQTTHOST, _CLIENTID, _MQTTPORT);

FlowMeter f1, f2, f3;

FlowMeter flow_arr[NUMBER_OF_FLOWMETERS] = {f1, f2, f3};
const int sensor_pins[NUMBER_OF_FLOWMETERS] = {_SPIN1, _SPIN2, _SPIN3};
const float cal_factors[NUMBER_OF_FLOWMETERS] = {_SS_FLOW, _SS_FLOW, _YFS402B};
String meter_names[NUMBER_OF_FLOWMETERS] = {_FLOW1, _FLOW2, _FLOW3};

int count;
JSONVar data, message;

void pulseCounter() { flow_arr[count].pulse_count++; }

void onConnectionEstablished()
{
  client.subscribe(_SUBTOPIC, [](const String &payload)
  {
   if ((millis() - flow_arr[0].old_time) > 1000) // Only process counters once per second
   {
     for (int i = 0; i > NUMBER_OF_FLOWMETERS; i++) 
     {
       detachInterrupt(flow_arr[i].sensor_pin);

       // Calculate flow rate
       flow_arr[i].flow_rate = ((1000.0 / (millis() - flow_arr[i].old_time)) * flow_arr[i].pulse_count) / flow_arr[i].cal_factor;
       flow_arr[i].old_time = millis();
       flow_arr[i].flow_mLs = (flow_arr[i].flow_rate / 60.0) * 1000.0;
       flow_arr[i].total_mLs += flow_arr[i].flow_mLs;

       Serial.println(flow_arr[i].flow_mLs);

       data[flow_arr[i].name]["LPM"] = flow_arr[i].flow_rate;
       data[flow_arr[i].name]["Total"] = flow_arr[i].total_mLs;

       attachInterrupt(flow_arr[i].sensor_pin, pulseCounter, FALLING);
      }
   }
    // Output
    // Serial.println(count);
    // Serial.println(data);
    message["key"] = _CLIENTID;
    message["data"] = data;
  
    client.publish(_PUBTOPIC, JSON.stringify(message)); });
  delay(5000);
}

void setup()
{
  // Initialize a serial connection for reporting values to the host
  Serial.begin(115200);
  client.enableHTTPWebUpdater();
  // client.setMaxPacketSize(4096);
  client.enableDebuggingMessages();

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
}