import paho.mqtt.client as mqtt
import time

client = mqtt.Client()
client.connect("192.168.1.1", 1883)
client.loop_start()

while True:
    time.sleep(0.1)
    with open("/dev/rpigpio", "r") as fd:
        l = fd.read()
        client.publish("embsw/sw1", str(l.count("1")),retain=True, qos=2)
        client.publish("embsw/sw2", str(l.count("0")),retain=True, qos=2)
