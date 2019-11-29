/*
  SD card datalogger and temperature control

  created 29 Nov 2019
  by Julian Neureuther and Emanuel Sizmann
*/

#include <SPI.h>
#include <SD.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// #define RELAIS D3 // if using a relais on pin D3 (nodeMCU)

const int chipSelect = 4;

OneWire oneWire(5);
DallasTemperature sensors(&oneWire); // sensor DS18B20

DeviceAddress inside1 = { 0x28, 0xFF, 0x18, 0x97, 0x76, 0x18, 0x01, 0x97 }; // add the oneWire device address here => use sample code to find
DeviceAddress inside2 = { 0x28, 0xFF, 0x72, 0x91, 0x76, 0x18, 0x01, 0x32 };
DeviceAddress outside = { 0x28, 0xFF, 0x1E, 0x93, 0x76, 0x18, 0x01, 0x05 };

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

  //pinMode(RELAIS, OUTPUT);
  //digitalWrite(RELAIS, LOW); // important for safety if used to switch 230V: In case of power failure the relais should be off, so keep it active on!

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  Serial.println("Initializing OneWire Sensors...");
  sensors.begin();
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  sensors.setResolution(inside1, 12);
  sensors.setResolution(inside2, 12);
  sensors.setResolution(outside, 12);

  writeString(String("------------- Started -------------"));
  writeString(String("time(s);inside1;inside2;outside;")); // type here, where the sensores are placed. 
}

void writeString(String s) {
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(s);
    dataFile.close();
    // print to the serial port too:
    Serial.println(s);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
    
  }
}

void loop() {
  static unsigned long last_exec = 0;

  if (millis() >= (last_exec + 10000)) { //type here the time (in ms) between two meassurements
    last_exec = millis();

    digitalWrite(BUILTIN_LED, LOW);

    sensors.requestTemperatures(); // be aware that meassuring takes time, especially if you use high res. With three sensors apprx. 1.5 sec.

    double temp_inside1 = sensors.getTempC(inside1);
    double temp_inside2 = sensors.getTempC(inside2);
    double temp_outside = sensors.getTempC(outside);
    
    // make a string for assembling the data to log:
    String dataString = "";
  
    static unsigned long start = 0;
    if (start == 0) {
      start = millis();
    }
    dataString += String((millis() - start) / 1000);
    dataString += ";";
  
    dataString += String(temp_inside1);
    dataString += ";";
    dataString += String(temp_inside2);
    dataString += ";";
    dataString += String(temp_outside);
    dataString += ";";
  
    writeString(dataString);

/*    if(temp_inside1 < 65 && temp_inside2 < 75) //if using relais, type the max conditions here
    {
      digitalWrite(RELAIS, LOW);
    }
    else
    {
      digitalWrite(RELAIS, HIGH);
    }
*/
    digitalWrite(BUILTIN_LED, HIGH);
  }
}
