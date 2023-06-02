import time
import serial
import paho.mqtt.publish as publish
import psutil
import string

serial1 = serial.Serial('/dev/ttyUSB0', 9600)


channel_ID = "2160141"

# The hostname of the ThingSpeak MQTT broker.
mqtt_host = "mqtt3.thingspeak.com"

# TASK6: Replace Your MQTT credentials for the device
mqtt_client_ID = "LgwEOiILAwglDzsbEDk1EQI"
mqtt_username  = "LgwEOiILAwglDzsbEDk1EQI"
mqtt_password  = "Ps/+fRLZJ+CyfFImsbM0H9d1"

t_transport = "websockets"
t_port = 80
# Create the topic string.
topic = "channels/" + channel_ID + "/publish"
index = 1
my_float = 0.23

while True:
    datastring = serial1.readline()
    highest_detected_confidence = float(datastring) 
    # build the payload string.
    payload = "field1=" + str(highest_detected_confidence)
    #print(payload)
    # attempt to publish this data to the topic.
    try:
      print ("Writing Payload = ", payload," to host: ", mqtt_host, " clientID= ", mqtt_client_ID, " User ", mqtt_username, " PWD ", mqtt_password)
      publish.single(topic, payload, hostname=mqtt_host, transport=t_transport, port=t_port, \
      client_id=mqtt_client_ID, auth={'username':mqtt_username,'password':mqtt_password})
    except Exception as e:
      print(e)
      time.sleep(2)
ser.close()#end of while loop, close the serial interface
