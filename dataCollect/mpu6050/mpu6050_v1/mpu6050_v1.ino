#include <Wire.h>

#include <Adafruit_MPU6050.h>


Adafruit_MPU6050 mpu;

void setupHardware() {
 Wire.begin(41, 40);
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
float prediction = 0;
float tem = 0;
long last_time = 0;
void loop() {
 
 sensors_event_t  a, g, temp;
 

   mpu.getEvent(&a, &g, &temp);
   float ax = a.acceleration.x;
   float ay = a.acceleration.y;
   float az = a.acceleration.z;
   float gx = g.gyro.x;
   float gy = g.gyro.y;
   float gz = g.gyro.z;
   prediction = prediction + abs(abs(ax)- tem);
   tem = abs(ax);
//      Serial.print("X: ");
//      Serial.print(gx);
//      Serial.print(" Y: ");
//      Serial.print(gy);
//      Serial.print(" Z: ");
//      Serial.print(gz);
//      Serial.print(" || ");
//      Serial.print("X: ");
//      Serial.print(ax);
//      Serial.print(" Y: ");
//      Serial.print(ay);
//      Serial.print(" Z: ");
//      Serial.println(az);

      
    long now = millis();
  if (now - last_time > 15000) {
    Serial.print(" prediction: ");  
      Serial.println(prediction);
      prediction = 0;
      last_time = now;
}


  

}
