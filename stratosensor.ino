#include <Wire.h>
#include <SoftwareSerial.h>

#include <SPI.h>
#include <SD.h>

#include <I2Cdev.h>
#include <MPU6050.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_VEML6070.h>
#include <Adafruit_ADXL345_U.h>

#include <Sodaq_BMP085.h>

#include <Makerblog_TSL45315.h>

///////////////////////////////////////////////////////////////
#define interval 1000 /* Interval between messurements in ms */

// Calibration
// Warning: It is essential that you calibrate your sensor by
//          your own! These values are specific to our setup.
//
// Note:    Values from the sensors are slightly depending by
//          the temperature around the sensor.
//          We use an average value from 26000
//          single messurements raised from 37°C to -50°C.

#define x_gyrooff -543
#define y_gyrooff 162
#define z_gyrooff -95
#define x_accloff -238
#define y_accloff 240
#define z_accloff 18577

///////////////////////////////////////////////////////////////

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

Adafruit_VEML6070 uv = Adafruit_VEML6070();

Makerblog_TSL45315 luxsensor = Makerblog_TSL45315(TSL45315_TIME_M4);

MPU6050 accelgyro;

Sodaq_BMP085 bmp;

SoftwareSerial GPS(D3, D4, false, 8096); // RX, TX, inverseLogic, buffSize

// D1/D2 i2c

const int chipSelect = D8;

void setup() {
    Serial.begin(57600);

    Wire.begin();

    GPS.begin(9600);
    // Do not enable Tx!
    GPS.enableRx(true);

    bmp.begin();
    luxsensor.begin();

    accelgyro.initialize();

    accelgyro.setXGyroOffset(x_gyrooff);
    accelgyro.setYGyroOffset(y_gyrooff);
    accelgyro.setZGyroOffset(z_gyrooff);
    accelgyro.setXAccelOffset(x_accloff);
    accelgyro.setYAccelOffset(y_accloff);
    accelgyro.setZAccelOffset(z_accloff);

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

void loop() {
    static unsigned long lastMeasurement = 0;

    // Variables to buffer GPS output. We only write to SD card if the buffer
    // is full.
    static const size_t gpsBufSize = 4096;
    static char gpsBuf[gpsBufSize + 1] = {};
    static size_t gpsIdx = 0;

    // Variables to read each line, if a line is complete and starts with $GPRMC then we copy the content
    // to our gprmcBuf and parse the current timestamp which we store in time
    static char lineBuf[128];
    static size_t lineIdx;
    static char gprmcBuf[128] = {};
    static unsigned long time;

    while (GPS.available() > 0) {
        int c = GPS.read();

        gpsBuf[gpsIdx++] = c;
        if ((gpsIdx > 2048 && c == '\r') || gpsIdx == gpsBufSize) {
            gpsBuf[gpsIdx] = '\0';
            File sdFile = SD.open("GPS.LOG", FILE_WRITE);
            if (sdFile) {
                sdFile.print(gpsBuf);
            } else {
                Serial.println("gpssd\terror");
            }
            gpsIdx = 0;
        }

        if (c == '$') {
            // new message, start again
            lineBuf[lineIdx] = '\0';
            if (strncmp(lineBuf, "$GPRMC,", 7) == 0) {
                strcpy(gprmcBuf, lineBuf);
                gprmcBuf[strlen(gprmcBuf)-2] = '\0'; // strip \r\n
                unsigned long newTime;
                if (sscanf(gprmcBuf, "$GPRMC,%lu", &newTime) == 1) {
                    time = newTime;
                }
            } else if (gprmcBuf[0] == '\0') { // switch between \0 and - while gprmcBuf is not set but we still receive data
                gprmcBuf[0] = '-';
            } else if (gprmcBuf[0] == '-') {
                gprmcBuf[0] = '\0';
            }
            lineIdx = 0;
        }
        lineBuf[lineIdx++] = c;
        if (lineIdx >= 128) {
            // unexpected long line, restart to prevent buffer overflow
            lineIdx = 0;
        }
        /* Serial.print((char)c); */
    }

    if(millis() - lastMeasurement > interval) {
        char *buf;

        File sdFile = SD.open("DATA.LOG", FILE_WRITE);
        if (!sdFile) {
            Serial.println("sd\terror");
        }

        Serial.print("time\t");Serial.print(millis());Serial.print("\t");Serial.println(time);
        if (sdFile) {
        sdFile.print("time\t");sdFile.print(millis());sdFile.print("\t");sdFile.println(time);
        }

        Serial.print("gps\t");Serial.print(millis());Serial.print("\t");Serial.println(gprmcBuf);
        if (sdFile) {
        sdFile.print("gps\t");sdFile.print(millis());sdFile.print("\t");sdFile.println(gprmcBuf);
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
        lastMeasurement = millis();
    }
}
