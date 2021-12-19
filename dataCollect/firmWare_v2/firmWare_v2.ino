#include <WiFi.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_MPU6050.h>

// Set to 1 to output debug info to Serial, 0 otherwise
#define DEBUG 1
#define LED_BUILTIN 2

// WiFi credentials
//const char ssid[] = "domeplace615";
//const char password[] =  "9609143689";

const char ssid[] = "Rutanech’s iPhone";
const char password[] =  "onyourown";

// Server, file, and port
const char hostname[] = "172.20.10.7";
const String uri = "/";
const int port = 1337;

// Settings
const float collection_period = 1.0;   // How long to collect (sec)
const int sample_rate = 200;           // How fast to collect (Hz)
const int num_dec = 7;                 // Number of decimal places
const int num_samples = (int)(collection_period * sample_rate);
const unsigned long timeout = 500;     // Time to wait for server response

// Pins
const int led_pin = LED_BUILTIN;

// Expected JSON size. Use the following to calculate values:
// https://arduinojson.org/v6/assistant/
const size_t json_capacity = (3 * JSON_ARRAY_SIZE(num_samples)) + 
                            JSON_OBJECT_SIZE(3);

// Globals
//Adafruit_MSA301 msa;
Adafruit_MPU6050 mpu;

WiFiClient client;

/*******************************************************************************
 * Functions
 */

// Blink a status code
void blinkCode(int num_blinks, int ms) {
  for (int i = 0; i < num_blinks; i++) {
    digitalWrite(led_pin, HIGH);
    delay(ms);
    digitalWrite(led_pin, LOW);
    delay(ms);
  }
}

// Send GET request to server to get "ready" flag
int getServerReadyFlag(unsigned long timeout) {

  int ret_status = -1;
  unsigned long timestamp;

  // Make sure we're connected to WiFi
  if (WiFi.status() == WL_CONNECTED) {

    // Connect to server
#if DEBUG
    Serial.print("Connecting to: ");
    Serial.println(hostname);
#endif
    if (client.connect(hostname, port)) {
    
      // Sent GET request
  #if DEBUG
      Serial.println("Sending GET request");
  #endif
      client.print("GET " + uri + " HTTP/1.1\r\n" + 
                "Host: " + hostname + "\r\n" +
                "Connection: close\r\n\r\n");
      
      // Wait for up to specified time for response from server
      timestamp = millis();
      while (!client.available()) {
        if (millis() > timestamp + timeout) {
          blinkCode(3, 100);
#if DEBUG
          Serial.println("GET response timeout");
#endif
          return -1;
        }
      }

      // Header should take up 4 lines, so throw them away
      for (int i = 0; i < 4; i++) {
        if (client.available()) {
          String resp = client.readStringUntil('\r');
        } else {
          return -1;
        }
      }

      // Response from server should be only a 0 or 1
      if (client.available()) {
        String resp = client.readStringUntil('\r');
        resp.trim();
#if DEBUG
        Serial.print("Server response: ");
        Serial.println(resp);
#endif
        if (resp == "0") {
          ret_status = 0;
        } else if (resp == "1") {
          ret_status = 1;
        }
      } else {
        return -1;
      }
    }

    // Close TCP connection
    client.stop();
#if DEBUG
    Serial.println();
    Serial.println("Connection closed");
#endif
  }

  return ret_status;
}

int sendPostRequest(DynamicJsonDocument json, unsigned long timeout) {

  unsigned long timestamp;
  
  // Connect to server
#if DEBUG
  Serial.print("Connecting to ");
  Serial.println(hostname);
#endif
  if (!client.connect(hostname, port)) {
#if DEBUG
    Serial.println("Connection failed");
#endif
    return 0;
  }

  // Send HTTP POST request
#if DEBUG
  Serial.println("Sending POST request");
#endif
  client.print("POST " + uri + " HTTP/1.1\r\n" +
               "Host: " + hostname + "\r\n" +
               "Connection: close\r\n" +
               "Content-Type: application/json\r\n" +
               "Content-Length: " + measureJson(json) + "\r\n" +
               "\r\n");

  // Send JSON data
  serializeJson(json, client);

  // Wait for up to specified time for response from server
  timestamp = millis();
  while (!client.available()) {
    if (millis() > timestamp + timeout) {
      blinkCode(3, 100);
#if DEBUG
      Serial.println("GET response timeout");
#endif
      return -1;
    }
  }

  // Print response
#if DEBUG
  while (client.available() ) {
    String ln = client.readStringUntil('\r');
    Serial.print(ln);
  }
  Serial.println();
#endif

  // Close TCP connection
  client.stop();
#if DEBUG
  Serial.println();
  Serial.println("Connection closed");
#endif

  return 1;
}

/*******************************************************************************
 * Main
 */

void setup() {
  // Initialize Serial port for debugging
#if DEBUG
  Serial.begin(115200);
  while (!Serial);
  Wire.begin(41, 40);
#endif
  // Configure pins
 pinMode(2, OUTPUT); // prepare LED
 digitalWrite(2, HIGH); 

  // Initialize accelerometer
 if (mpu.begin()) { // prepare MPU6050 sensor
#if DEBUG
    Serial.println("Failed to initialize MPU6050");
#endif
    blinkCode(3, 500);
    Serial.println("MPU6050 sensor ready");
    while (1) {
      blinkCode(1, 50);
      break;
    }
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
#if DEBUG
  Serial.print("Connecting to WiFi");
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
#if DEBUG
    Serial.print(".");
#endif
  }

  // Print connection details
#if DEBUG
  Serial.println();
  Serial.println("Connected to the WiFi network");
  Serial.print("My IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

void loop() {

  static unsigned long timestamp = millis();
  static unsigned long prev_timestamp = timestamp;

  // Check to see if server is ready to accept a new sample
  int resp_status = getServerReadyFlag(timeout);
  if (resp_status != 1) {
#if DEBUG
    Serial.println("Server is not accepting new samples");
#endif
    return;
  }

  // Create JSON document
  DynamicJsonDocument json(json_capacity);
  JsonArray json_x = json.createNestedArray("x");
  JsonArray json_y = json.createNestedArray("y");
  JsonArray json_z = json.createNestedArray("z");

  // Take a given time worth of measurements
  int i = 0;
  while (i < num_samples) {
    if (millis() >= timestamp + (1000 / sample_rate)) {
  
      // Update timestamps to maintain sample rate
      prev_timestamp = timestamp;
      timestamp = millis();
#if 0
      Serial.print("Sample rate: ");
      Serial.println(1000 / (timestamp - prev_timestamp));
      Serial.println(" Hz");
#endif

      // Take sample measurement
      sensors_event_t  a, g, temp;
         mpu.getEvent(&a, &g, &temp);
   float ax = a.acceleration.x;
   float ay = a.acceleration.y;
   float az = a.acceleration.z;
   float gx = g.gyro.x;
   float gy = g.gyro.y;
   float gz = g.gyro.z;
#if 1
      Serial.print("X: ");
      Serial.print(gx);
      Serial.print(" Y: ");
      Serial.print(gy);
      Serial.print(" Z: ");
      Serial.println(gz);
#endif

      // Add x, y, and z measurements to JSON doc
      json_x.add(gx);
      json_y.add(gy);
      json_z.add(gz);

      // Update sample counter
      i++;
    }
  }

  // Send JSON to server via POST request
#if 0
  serializeJson(json, Serial);
  Serial.println();
#endif
  digitalWrite(led_pin, HIGH);
  sendPostRequest(json, timeout);
  digitalWrite(led_pin, LOW);
}
