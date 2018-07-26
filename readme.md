Stratospheric flight to meassure environment variables

Preamble:
    We got the opportuity to get around 300 gramm of usable weight during
    a weatherballonflight up to 35 km height. 
    We used it to messure an amount of different variables.    

Used equipment:
BMP180 (Temperature, Humidity, Pressure) [Outside]
MPU-6050 (Gyro, Acceleration)  [Outside]
TSL45315 (Luxmeter)  [Outside]
HDC1000 (Temperature, Humidity)  [Outside]
DHT22 (Temperatur) [Inside]
ADXL345 [Outside]
MicroSD-Card incl. PCB for i²C
NodeMCUv3 (with 5V out)
2x 18650-Cells in parallel
self-designed PCB (need to be improved)

Used Libs:
    
MMA7760 http://www.timzaman.com/2011/05/acceleration-mma7660-3-axis-accelerometer-library/
ADXL345 https://github.com/adafruit/Adafruit_ADXL345
MPU6050 https://github.com/tockn/MPU6050_tockn
HDC1000 https://github.com/adafruit/Adafruit_HDC1000_Library
TSL45315 https://github.com/adidax/Makerblog_TSL45315
BMP180 https://github.com/adafruit/Adafruit_BMP085_Unified

Recommended: https://github.com/adafruit/Adafruit_Sensor
Required: https://github.com/jrowberg/i2cdevlib

Note:
    From our messurements we assume that the system can live up to 20 hours while messure every 500ms.
    For reliability reasons we recommend to put two 18650-Packs in parallel to avoid a loss of data in case of
    a batteryfailure. 

////////////////////////////////////////////////////////////////////////
German version below


Preambel:
    Wir hatten die Möglichkeit 300 Gramm an Material auf einen Wetterballonflug mitzuschicken.
    Wir nutzten das um eine Vielzahl an Umgebungsvariablen aufzuzeichnen.
    
Verwendetes Material
BMP180 (Temperatur, Feuchtigkeit, Druck) [Außen]
MPU-6050 (Gyro, Beschleunigung)  [Außen]
TSL45315 (Luxmeter)  [Außen]
HDC1000 (Temperatur, Feuchtigkeit)  [Außen]
DHT22 (Temperatur) [Innen]
ADXL345 (Beschleunigung) [Außen]
MicroSD-Card inkl. PCB für i²C [Innen]
NodeMCUv3 (with 5V out) [Innen]
2x 18650-Cells parallel geschaltet [Innen]
self-designed PCB (verbessungsbedüftig) [Innen]

Used Libs:
    
MMA7760 http://www.timzaman.com/2011/05/acceleration-mma7660-3-axis-accelerometer-library/
ADXL345 https://github.com/adafruit/Adafruit_ADXL345
MPU6050 https://github.com/tockn/MPU6050_tockn
HDC1000 https://github.com/adafruit/Adafruit_HDC1000_Library
TSL45315 https://github.com/adidax/Makerblog_TSL45315
BMP180 https://github.com/adafruit/Adafruit_BMP085_Unified

Optional: https://github.com/adafruit/Adafruit_Sensor
Benötigt: https://github.com/jrowberg/i2cdevlib
