import paho.mqtt.client as mqtt

host = "broker.hivemq.com"
port = 1883

chk_bt = True

def on_connect(self, client, userdata, rc):
    print("MQTT Connected.")
    self.subscribe("/@TU/prediction")
    self.subscribe("/@TU/bt")

def on_message(client, userdata,msg):
    data = msg.payload.decode("utf-8", "strict")
    print(data)
    if data == "ON":
        global chk_bt
        chk_bt = not chk_bt
        print(chk_bt)
    elif data == "1":
        if chk_bt:
            print("fff")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(host)
client.loop_forever()

