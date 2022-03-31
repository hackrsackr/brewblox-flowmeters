#include "secrets.h"

#define _SSID SECRET_SSID
#define _PASS SECRET_PASS

// Brewblox IP address
#define _MQTTHOST "192.168.1.2"
#define _MQTTPORT 1883
#define _CLIENTID "flow-meters-test"
//#define _SUBTOPIC "brewcast/history/spark-two"
#define _PUBTOPIC "brewcast/history/flow-meters"

// Meter Names
#define _FLOW1 "flow-meter 1"
#define _FLOW2 "flow-meter 2"
#define _FLOW3 "flow-meter 3"
#define _FLOW4 "flow-meter 4"
// Calibration Factors
#define _YF_S302 21.85 // [23*Q-3] f = 23*60-(23*3) = 1311 bits per liter 1311/60 = 21.85
#define _YFS402B 32.00 // f = Q*32 32*60 = 1920 bits per liter
#define _SS_FLOW 9.93  // 596 bits per liter 596/60 = 9.93  

// Pin of the semsor of each spunder
#define _SPIN1 39
#define _SPIN2 35
#define _SPIN3 36