import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Connected")
    client.subscribe("embsw/#")

def on_message(client, uswerdata, msg):
    if(msg.topic == "embsw/sw1"):
        with open("sw1.txt", "w") as sw1:
            sw1.write(msg.payload.decode())
    else:
        with open("sw2.txt", "w") as sw2:
            sw2.write(msg.payload.decode())

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("192.168.1.1", 1883, 60)
client.loop_forever()