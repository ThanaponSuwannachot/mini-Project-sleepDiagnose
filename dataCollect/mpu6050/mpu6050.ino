#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_HTS221.h>
Adafruit_BMP280 bmp;
Adafruit_HTS221 hts;
Adafruit_MPU6050 mpu;

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
}

void setup() {
 Serial.begin(115200);
 setupHardware();

}

void loop() {
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

      Serial.print("X: ");
      Serial.print(ax);
      Serial.print(" Y: ");
      Serial.print(ay);
      Serial.print(" Z: ");
      Serial.println(az);
}
