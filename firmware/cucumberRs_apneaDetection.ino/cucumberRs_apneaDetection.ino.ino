#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_HTS221.h>
Adafruit_BMP280 bmp;
Adafruit_HTS221 hts;
Adafruit_MPU6050 mpu;

// Local includes
#include "md_model-normal.h"

extern "C" {
#include "utils.h"
};

// Settings
constexpr int NUM_AXES = 3;           // Number of axes on accelerometer
constexpr int MAX_MEASUREMENTS = 128; // Number of samples to keep in each axis
constexpr float MAD_SCALE = 1.4826;   // Scale MAD to be inline with SciPy calcs
constexpr float THRESHOLD = 1.0;     // Any MD over this is an anomaly
constexpr int WAIT_TIME = 1000;       // ms between sample sets
constexpr int SAMPLE_RATE = 200;      // How fast to collect measurements (Hz)


void setupHardware() {
 Wire.begin(41, 40);
 if (bmp.begin(0x76)) { // prepare BMP280 sensor
   Serial.println("BMP280 sensor ready");
 }
 if (hts.begin_I2C()) { // prepare HTS221 sensor
   Serial.println("HTS221 sensor ready");
 }
 if (mpu.begin()) { // prepare MPU6050 sensor
   Serial.println("MPU6050 sensor ready");
 } 
 pinMode(2, OUTPUT); // prepare LED
 digitalWrite(2, HIGH); 
 pinMode(13, INPUT);
}

const char *SSID = "domeplace615";
const char *PWD = "9609143689";



long last_time = 0;
char data[100];

// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 

char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;


void connectToWiFi() {
  Serial.print("Connectiog to ");
 
  WiFi.begin(SSID, PWD);
  Serial.println(SSID);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.print("Connected.");
  
} 

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Callback - ");
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}

void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  // set the callback function
  mqttClient.setCallback(callback);
}

int buttonState = 0; 
void setup() {
  Serial.begin(9600);
  
  connectToWiFi();

setupHardware();

  setupMQTT();
    
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        mqttClient.subscribe("/@TU/commands");
      }
      
  }
}

float prediction = 0;
float tem = 0;

void loop() {

  float sample[MAX_MEASUREMENTS][NUM_AXES];
  float measurements[MAX_MEASUREMENTS];
  float mad[NUM_AXES];
  float mahal;

  static unsigned long timestamp = millis();
  static unsigned long prev_timestamp = timestamp;
  int i = 0;
  while (i < MAX_MEASUREMENTS) {
    if (millis() >= timestamp + (1000 / SAMPLE_RATE)) {
  
      prev_timestamp = timestamp;
      timestamp = millis();

        sensors_event_t temp, humid;
    sensors_event_t a, g;
    mpu.getEvent(&a, &g, &temp);

      sample[i][0] = g.gyro.x;
      sample[i][1] = g.gyro.y;
      sample[i][2] = g.gyro.z;

      i++;
    }
  }

  for (int axis = 0; axis < NUM_AXES; axis++) {
    for (int i = 0; i < MAX_MEASUREMENTS; i++) {
      measurements[i] = sample[i][axis];
    }
    mad[axis] = MAD_SCALE * calc_mad(measurements, MAX_MEASUREMENTS);
  }

  mahal = mahalanobis(mad, model_mu, *model_inv_cov, model_mu_apnea);
  if (mahal > THRESHOLD) {
    prediction = 1;
  } else {
    prediction = 0;
  }
  
  if (!mqttClient.connected())
    reconnect();

  mqttClient.loop();
  buttonState = digitalRead(13);
  if (buttonState == LOW) {
    mqttClient.publish("/@TU/bt", "ON");
    digitalWrite(2, HIGH);
  } 
  else {
   digitalWrite(2, LOW);
  }


  
  long now = millis();
  if (now - last_time > 15000) {
 char json_body[200];
 const char json_tmpl[] = "{\"pressure\": %.2f," 
                          "\"temperature\": %.2f," 
                          "\"humidity\": %.2f,"
                          "\"acceleration\": [%.2f,%.2f,%.2f],"
                          "\"angular_velocity\":[%.2f,%.2f,%.2f]}";
 sensors_event_t temp, humid;
 sensors_event_t a, g;

   float p = bmp.readPressure();
   hts.getEvent(&humid, &temp);
   float t = temp.temperature;
   float h = humid.relative_humidity;
   mpu.getEvent(&a, &g, &temp);
   float ax = a.acceleration.x;
   float ay = a.acceleration.y;
   float az = a.acceleration.z;
   float gx = g.gyro.x;
   float gy = g.gyro.y;
   float gz = g.gyro.z;
   
   sprintf(json_body, json_tmpl, prediction, t, h, ax, ay, az, gx, gy, gz);
   Serial.println(json_body);
   mqttClient.publish("/@TU/cucumber", json_body);
   mqttClient.publish("/@TU/bt", "OFF");
   mqttClient.publish("/@TU/prediction", String(prediction));
   buttonState=0;
 
    last_time = now;
  
}
}
