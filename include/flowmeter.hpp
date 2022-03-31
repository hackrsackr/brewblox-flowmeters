#include <Arduino_JSON.h>

class FlowMeter
{
public:
  int sensor_pin;
  unsigned int flow_mLs;
  volatile int pulse_count;
  float flow_rate;
  float cal_factor;
  unsigned long total_mLs;
  unsigned long total_pulseCount;
  unsigned long old_time;
  String name;
  JSONVar data;

  void pulseCounter() { pulse_count++; };
  void setSensorPin(int pin) { sensor_pin = pin; }
  void setCalibrationFactor(float cal) { cal_factor = cal; }
  float getFlowRate()
  {
    flow_rate = 1000.0 / (millis() - old_time) * pulse_count / cal_factor;

    return flow_rate;
  }

  void run()
  {
    if ((millis() - cal_factor) > 1000) // Only process counters once per second
    {
      // Disable the interrupt while calculating flow rate and sending the value to
      // the host
      detachInterrupt(sensor_pin);

      // Because this loop may not complete in exactly 1 second intervals we calculate
      // the number of milliseconds that have passed since the last execution and use
      // that to scale the output. We also apply the calibrationFactor to scale the output
      // based on the number of pulses per second per units of measure (litres/minute in
      // this case) coming from the sensor.
      flow_rate = ((1000.0 / (millis() - old_time)) * pulse_count) / cal_factor;
      // getFlowRate();
      //  Note the time this processing pass was executed. Note that because we've
      //  disabled interrupts the millis() function won't actually be incrementing right
      //  at this point, but it will still return the value it was set to just before
      //  interrupts went away.
      old_time = millis();

      // Divide the flow rate in litres/minute by 60 to determine how many litres have
      // passed through the sensor in this 1 second interval, then multiply by 1000 to
      // convert to millilitres.
      flow_mLs = (flow_rate / 60) * 1000;
      total_mLs += flow_mLs;

      data[name]["Flow Rate[LPM]"] = flow_rate;
      data[name]["Total[mls]"] = total_mLs;
      //JSON.stringify(data);

      // Reset the pulse counter so we can start incrementing again
      pulse_count = 0;

    }
    // Enable the interrupt again now that we've finished sending output
    //attachInterrupt(sensor_pin, pulseCounter, FALLING);
  }
};