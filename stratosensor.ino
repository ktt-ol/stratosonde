#include <dummy.h>

#include <Wire.h>
#include <I2Cdev.h>
/* #include <MPU6050_9Axis_MotionApps41.h> */
#include <MPU6050.h>
/* #include <helper_3dmath.h> */
/* #include <MPU6050_6Axis_MotionApps20.h> */

#include <Adafruit_Sensor.h>
#include <Adafruit_VEML6070.h>
#include <Adafruit_ADXL345_U.h>

#include <Sodaq_BMP085.h>

#include <Makerblog_TSL45315.h>


/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

Adafruit_VEML6070 uv = Adafruit_VEML6070();

Makerblog_TSL45315 luxsensor = Makerblog_TSL45315(TSL45315_TIME_M4);

MPU6050 accelgyro;

Sodaq_BMP085 bmp;

void setup() {
    Serial.begin(9600);
    delay(1000);
    Wire.begin();


    bmp.begin();
    luxsensor.begin();
    
    accelgyro.initialize();
    // TODO initialize
    accelgyro.setXGyroOffset(220);
    accelgyro.setYGyroOffset(76);
    accelgyro.setZGyroOffset(-85);

    accel.begin();
    uv.begin(VEML6070_1_T); // pass in the integration time constant
}

void logBMP() {
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");

    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print("Real altitude = ");
    Serial.print(bmp.readAltitude(101500));
    Serial.println(" meters");

    Serial.println();
}


void logLux() {
    uint32_t lux = luxsensor.readLux();
    Serial.println("=== TSL Lux");
    Serial.println(lux, DEC);
}

void logVEML() {
    Serial.println("=== VEML");
    Serial.print("UV light level: "); Serial.println(uv.readUV());
}

void logMPU6050() {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    Serial.print(ax); Serial.print(" ");
    Serial.print(ay); Serial.print(" ");
    Serial.print(az); Serial.print(" ");
    Serial.print(gx); Serial.print(" ");
    Serial.print(gy); Serial.print(" ");
    Serial.print(gz); Serial.println();
}


void logADXL() {
    sensors_event_t event;
    accel.getEvent(&event);

    /* Display the results (acceleration is measured in m/s^2) */
    Serial.println("=== ADXL");
    Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
}
long timer = 0;
void loop() {
    if(millis() - timer > 1000) {
        logMPU6050();
        logADXL();
        logVEML();
        logLux();
        logBMP();
        timer = millis();
    }
}
