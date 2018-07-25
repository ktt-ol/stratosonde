#include <Wire.h>

#include <SPI.h>
#include <SD.h>

#include <I2Cdev.h>
#include <MPU6050.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_VEML6070.h>
#include <Adafruit_ADXL345_U.h>

#include <DHT.h>
#include <DHT_U.h>

#include <Sodaq_BMP085.h>

#include <Makerblog_TSL45315.h>


/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

Adafruit_VEML6070 uv = Adafruit_VEML6070();

Makerblog_TSL45315 luxsensor = Makerblog_TSL45315(TSL45315_TIME_M4);

MPU6050 accelgyro;

DHT_Unified dht(D3, DHT22);

Sodaq_BMP085 bmp;

const int chipSelect = D8;

void setup() {
    Serial.begin(9600);
    Wire.begin();

    bmp.begin();
    luxsensor.begin();
    dht.begin();

    accelgyro.initialize();
    // TODO initialize
    // 32767	5296	-18748	32767	329	-10123
    
    /* accelgyro.setXGyroOffset(-32767); */
    /* accelgyro.setYGyroOffset(-5296); */
    /* accelgyro.setZGyroOffset(18748); */
    /* accelgyro.setXAccelOffset(-32767); */
    /* accelgyro.setYAccelOffset(-329); */
    /* accelgyro.setZAccelOffset(10123); */

    accel.begin();
    uv.begin(VEML6070_1_T); // pass in the integration time constant

    Serial.print("Initializing SD card...");

    // see if the card is present and can be initialized:
    if (!SD.begin()) {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        return;
    }
    Serial.println("card initialized.");
}

char *readBMP() {
    static char buf[8+1+6];
    snprintf(buf, sizeof(buf), "%-2.2f\t%d", bmp.readTemperature(), bmp.readPressure());
    return buf;
}

char *readLux() {
    static char buf[8];
    snprintf(buf, sizeof(buf), "%d", luxsensor.readLux());
    return buf;
}

char *readUV() {
    static char buf[8];
    snprintf(buf, sizeof(buf), "%d", uv.readUV());
    return buf;
}

char *readDHT() {
    static char buf[18];
    sensors_event_t eventT;
    sensors_event_t eventH;
    dht.temperature().getEvent(&eventT);
    dht.humidity().getEvent(&eventH);
    snprintf(buf, sizeof(buf), "%5.2f\t%5.2f", eventT.temperature, eventH.relative_humidity);
    return buf;
}

char *readMPU6050() {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    static char buf[6 * 6 /* signed int16 */ + 5 /*;*/ + 1 /*\0*/];
    snprintf(buf, sizeof(buf), "%d\t%d\t%d\t%d\t%d\t%d", ax, ay, az, gx, gy, gz);
    return buf;
}

char *readADXL() {
    sensors_event_t event;
    accel.getEvent(&event);

    static char buf[32];
    snprintf(buf, sizeof(buf), "%6.2f\t%6.2f\t%6.2f",
        event.acceleration.x,
        event.acceleration.y,
        event.acceleration.z
    );
    return buf;
}

unsigned long timer = 0;
void loop() {
    if(millis() - timer > 1000) {
        Serial.println("reading");
        char *buf;

        File sdFile = SD.open("DATA.LOG", FILE_WRITE);
        if (!sdFile) {
            Serial.println("sd\terror");
        }

        buf = readMPU6050();
        Serial.print("6d\t");Serial.print(millis());Serial.print("\t");Serial.println(buf);
        if (sdFile) {
        sdFile.print("6d\t");sdFile.print(millis());sdFile.print("\t");sdFile.println(buf);
        }

        buf = readADXL();
        Serial.print("3d\t");Serial.print(millis());Serial.print("\t");Serial.println(buf);
        if (sdFile) {
        sdFile.print("3d\t");sdFile.print(millis());sdFile.print("\t");sdFile.println(buf);
        }

        buf = readLux();
        Serial.print("lux\t");Serial.print(millis());Serial.print("\t");Serial.println(buf);
        if (sdFile) {
        sdFile.print("lux\t");sdFile.print(millis());sdFile.print("\t");sdFile.println(buf);
        }

        buf = readDHT();
        Serial.print("dht\t");Serial.print(millis());Serial.print("\t");Serial.println(buf);
        if (sdFile) {
        sdFile.print("dht\t");sdFile.print(millis());sdFile.print("\t");sdFile.println(buf);
        }

        buf = readUV();
        Serial.print("uv\t");Serial.print(millis());Serial.print("\t");Serial.println(buf);
        if (sdFile) {
        sdFile.print("uv\t");sdFile.print(millis());sdFile.print("\t");sdFile.println(buf);
        }

        buf = readBMP();
        Serial.print("bmp\t");Serial.print(millis());Serial.print("\t");Serial.println(buf);
        if (sdFile) {
        sdFile.print("bmp\t");sdFile.print(millis());sdFile.print("\t");sdFile.println(buf);
        }

        if (sdFile) {
            sdFile.close();
        }
        timer = millis();
    }
}
