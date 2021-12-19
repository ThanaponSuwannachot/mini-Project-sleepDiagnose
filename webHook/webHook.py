from flask import Flask, request
from flask_mqtt import Mqtt
from linebot.models import *
from linebot import *
import json
import requests
import paho.mqtt.client as mqtt

app = Flask(__name__)
app.config['SECRET'] = 'my secret key'
app.config['TEMPLATES_AUTO_RELOAD'] = True
app.config['MQTT_BROKER_URL'] = 'broker.hivemq.com'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = ''
app.config['MQTT_PASSWORD'] = ''
app.config['MQTT_KEEPALIVE'] = 5
app.config['MQTT_TLS_ENABLED'] = False
app.config['MQTT_CLEAN_SESSION'] = True

mqtt = Mqtt(app)

line_bot_api = LineBotApi('5JSxNCtAC3Rb24MH7QwNosL4K82yrfhIT7T3zV5xjOEmXIXkiBcQA6tuYcFW6v3Njj4vB8Yryl4Mwc9x9EoBVFbuzzZ5kJzPpTO9PbyFi9raetbIhlXDU1zw2qhaCA8mNwcICX6mwfZH+aUlZHiKbQdB04t89/1O/w1cDnyilFU=')
handler = WebhookHandler('1ce4e471fa2a78266e552f20d7b9c5d8')
chk_bt = False
cucomber = {}


@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    mqtt.subscribe('/@TU/predictions')
    mqtt.subscribe("/@TU/bt")
    mqtt.subscribe('/@TU/cucumber')
    
@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    data = dict(
        topic=message.topic,
        payload=message.payload.decode()
    )
    if data['payload'] == "ON":
        global chk_bt
        chk_bt = not chk_bt
        print(chk_bt)
    elif data['payload'] == "1":
        if chk_bt:
            text_message = TextSendMessage(text='เกิดภาวะหยดหายใจ')
            line_bot_api.push_message('U51e756c91e2f9dc6fa92296dc2652cc5', text_message)
    elif data['topic'] == '/@TU/cucumber':
        global cucomber
        cucomber = json.loads(data['payload'])
        

@app.route('/getimage', methods=['GET'])
def get_image():
    filename = BASE_DIR + "promote2-10sec (1).gif"
    return send_file(filename) ,200


@app.route("/callback", methods=['POST'])
def callback():
    body = request.get_data(as_text=True)
    # print(body)
    req = request.get_json(silent=True, force=True)
    intent = req["queryResult"]["intent"]["displayName"] 
    text = req['originalDetectIntentRequest']['payload']['data']['message']['text'] 
    reply_token = req['originalDetectIntentRequest']['payload']['data']['replyToken']
    id = req['originalDetectIntentRequest']['payload']['data']['source']['userId']
    disname = line_bot_api.get_profile(id).display_name

    print('id = ' + id)
    print('name = ' + disname)
    print('text = ' + text)
    print('intent = ' + intent)
    print('reply_token = ' + reply_token)
    
    reply(intent,text,reply_token,id,disname)

    return 'OK'
    
    
def reply(intent,text,reply_token,id,disname):
    if intent == 'Covid 19':
        data = requests.get('http://covid19.th-stat.com/json/covid19v2/getTodayCases.json')
        json_data = json.loads(data.text)
        
        Confirmed = json_data['Confirmed']  # ติดเชื้อสะสม
        Recovered = json_data['Recovered']  # หายแล้ว
        Hospitalized = json_data['Hospitalized']  # รักษาอยู่ใน รพ.
        Deaths = json_data['Deaths']  # เสียชีวิต
        NewConfirmed = json_data['NewConfirmed']  # บวกเพิ่ม
        text_message = TextSendMessage(
            text='โควิดวันนี้\nติดเชื้อสะสม = {} คน(+เพิ่ม{})\nหายแล้ว = {} คน\nรักษาอยู่ใน รพ. = {} คน\nเสียชีวิต = {} คน'.format(
                Confirmed, NewConfirmed, Recovered, Hospitalized, Deaths))

        print("data",data)
        print("text_message-->",text)
        line_bot_api.reply_message(reply_token, text_message)

    if intent == 'ซักถามอาการ - custom - yes':
        req = request.get_json(silent=True, force=True)
        fulfillmentText = ''
        sum = 0
        query_result = req.get('queryResult')
        weight = int(query_result.get('parameters').get('weight'))
        hight = int(query_result.get('parameters').get('hight'))
        age = int(query_result.get('parameters').get('age'))
        
        bmi = weight/((hight/100)**2) 

        print("Your BMI is: {0} and you are: ".format(bmi), end='')

        #conditions
        if ( bmi < 16):
            fulfillmentText = 'คุณอายุ{0}คุณมี BMI={1} ฉันคิดว่าคุณผอมเกินไปควรทานอาหารที่ให้พลังงานมากขึ้นนะคะ'.format(age,bmi)
            print("severely underweight")

        elif ( bmi >= 16 and bmi < 18.5):
            fulfillmentText = 'คุณอายุ{0}คุณมี BMI={1} ฉันคิดว่าคุณมีน้ำหนักน้อยเกินไปควรทานให้มากขึ้นนะคะ'.format(age,bmi)
            print("underweight")

        elif ( bmi >= 18.5 and bmi < 25):
            fulfillmentText = 'คุณอายุ{0}คุณมี BMI={1} น่าอิจฉาจุงเบยหุ่นดีมากกกกก'.format(age,bmi)
            print("Healthy")

        elif ( bmi >= 25 and bmi < 30):
            fulfillmentText = 'คุณอายุ{0}คุณมี BMI={1} ควรลดของมันและของทอดนะคะ'.format(age,bmi)
            print("overweight")

        elif ( bmi >=30):
            fulfillmentText = 'คุณอายุ{0}คุณมี BMI={1} ไอ่ต้าวววว กินให้น้อยลงหน่อยน๊า'.format(age,bmi)
            print("severely overweight")



        text_message = TextSendMessage(text=fulfillmentText)
        line_bot_api.reply_message(reply_token,text_message)
        

    if intent == 'ควรกินน้ำ':
        text_message = TextSendMessage(text='ทดสอบสำเร็จ')
        line_bot_api.reply_message(reply_token,text_message)

    
    if intent == 'ความชื้น':
        temp = 'ความชื่นบริเวณตัวคุณคือ {0} องศา'.format(cucomber['temperature'])
        text_message = TextSendMessage(text=temp)
        line_bot_api.reply_message(reply_token,text_message)
        
    if intent == 'อุณหภูมิ':
        temp = 'อุณหภูมิบริเวณตัวคุณคือ {0} องศา'.format(cucomber['temperature'])
        text_message = TextSendMessage(text=temp)
        line_bot_api.reply_message(reply_token,text_message)
        
    if intent == 'เริ่มทดสอบ':
        global chk_bt
        chk_bt = not chk_bt
        temp = 'สเตตัสการทดสอบ {0} '.format(chk_bt)
        text_message = TextSendMessage(text=temp)
        line_bot_api.reply_message(reply_token,text_message)

        
if __name__ == "__main__":
    app.run()
