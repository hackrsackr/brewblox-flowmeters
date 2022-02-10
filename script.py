import serial
import json
from time import sleep

from paho.mqtt import client as mqtt

HOST = '192.168.1.2'
PORT = 80
USB = '/dev/ttyUSB0'
HISTORY_TOPIC = 'brewcast/history'
TOPIC = HISTORY_TOPIC + '/flow-meters'

client = mqtt.Client(transport='websockets')
client.ws_set_options(path='/eventbus')

ser = serial.Serial(port=USB,
                    baudrate=115200,
                    timeout=1)

try:
    client.connect_async(host=HOST, port=PORT)
    client.loop_start()

    while True:
        val = ser.readline().decode().rstrip()
        #print('decoded', val)
        try:
            val = json.loads(val)
        except json.JSONDecodeError:
            continue
        print('json', val)
        
        message = {
            'key': 'FlowMeters',
            'data': val
         }

        client.publish(TOPIC, json.dumps(message))
        #print(json.dumps(message, sort_keys=False, indent=4))

finally:
    ser.close()
    client.loop_stop
