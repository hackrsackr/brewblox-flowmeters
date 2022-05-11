#include "Arduino_JSON.h"

class FlowMeter
{
public:
  uint8_t sensor_pin;
  
  volatile int pulse_count;
  
  float flow_mLs;
  float flow_rate;
  float cal_factor;
  
  unsigned long total_mLs;
  unsigned long total_pulseCount;
  unsigned long old_time;
  
  String name;
  JSONVar flow_data;

  FlowMeter();
  FlowMeter(int, String, float);
  ~FlowMeter();

  void set_sensor_pin(uint8_t);
  void set_calibration_factor(float);
  void reset_total();
  float get_flowrate();
  void flowmeter_run();
};

FlowMeter::FlowMeter(int sp, String nm, float cf)
{
  sensor_pin = sp;
  name = nm;
  cal_factor = cf;
}

FlowMeter::~FlowMeter()
{
  Serial.println("destructor invoked");
}

void FlowMeter::reset_total() { total_mLs = 0; }

void FlowMeter::set_sensor_pin(uint8_t pin) { sensor_pin = pin; }

void FlowMeter::set_calibration_factor(float cal) { cal_factor = cal; }

float FlowMeter::get_flowrate()
{
  flow_rate = 1000.0 / (millis() - old_time) * pulse_count / cal_factor;

  return flow_rate;
}

void FlowMeter::flowmeter_run()
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
    get_flowrate();

    //  Note the time this processing pass was executed. Note that because we've
    //  disabled interrupts the millis() function won't actually be incrementing right
    //  at this point, but it will still return the value it was set to just before
    //  interrupts went away.
    old_time = millis();
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flow_mLs = (flow_rate / 60.0) * 1000.0;
    total_mLs += flow_mLs;


    flow_data["Flow Rate[LPM]"] = flow_rate;
    flow_data["Total[mls]"] = total_mLs;
    //JSON.stringify(flow_data);

    // Reset the pulse counter so we can start incrementing again
    pulse_count = 0;

  }
}