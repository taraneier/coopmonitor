#include <DHT.h>

#include <Bridge.h>
#include <Process.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

/*-----( Import needed libraries )-----*/
// Get 1-wire Library here: http://www.pjrc.com/teensy/td_libs_OneWire.html
#include <OneWire.h>

//Get DallasTemperature Library here:  http://milesburton.com/Main_Page?title=Dallas_Temperature_Control_Library
#include <DallasTemperature.h>

/*-----( Declare Constants and Pin Numbers )-----*/
#define ONE_WIRE_BUS_PIN 4

/*-----( Declare objects )-----*/
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS_PIN);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
DeviceAddress Probe01 = { 0x28, 0xE7, 0x79, 0x28, 0x06, 0x00, 0x00, 0x91 };
DeviceAddress Probe02 = { 0x28, 0x4F, 0xE8, 0x26, 0x06, 0x00, 0x00, 0x03 };
DeviceAddress Probe03 = { 0x28, 0x02, 0x85, 0x26, 0x06, 0x00, 0x00, 0xC1 };
DeviceAddress Probe04 = { 0x28, 0x6D, 0x0D, 0x56, 0x06, 0x00, 0x00, 0x7B };
DeviceAddress Probe05 = { 0x28, 0x26, 0xE6, 0x27, 0x06, 0x00, 0x00, 0x7B };
DeviceAddress Probe06 = { 0x28, 0x7A, 0xD0, 0x25, 0x06, 0x00, 0x00, 0x37 };
const int lightPin = A3;
const int reedPin = 7;

void setup() {
  // put your setup code here, to run once:
 Bridge.begin();
 dht.begin();
 pinMode(reedPin, INPUT);
 digitalWrite(reedPin, HIGH);
 //DS18B20 setup
 sensors.begin();
 
 //Resolution if necessary
 // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
//  sensors.setResolution(Probe01, 10);

}

void loop() {
  // put your main code here, to run repeatedly:
  int lum1 = readLight();
  float hum = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float t0 = dht.readTemperature(true);
  Serial.println("Readings from Integrated Temp/Hum Sensor");
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(t0);
  Serial.println(" *F");
  Serial.print("Light Value: ");
  Serial.print(lum1);
  int door = readReed();
  Serial.print("\tDoor: ");
  Serial.print(door);
  Serial.println(" ");
  
  Serial.println("Reading array of DS18B20 sensors");
   sensors.requestTemperatures();  
  float t1, t2, t3, t4, t5, t6;
  Serial.print("\nProbe 01 temperature is:   ");
  t1 = printTemperature(0);
  Serial.println();

  Serial.print("\nProbe 02 temperature is:   ");
  t2 = printTemperature(1);
  Serial.println();
 
  Serial.print("\nProbe 03 temperature is:   ");
  t3 = printTemperature(2);
  
    Serial.print("\nProbe 04 temperature is:   ");
  t4 = printTemperature(3);
    Serial.print("\nProbe 05 temperature is:   ");
  t5 = printTemperature(4);
    Serial.print("\nProbe 06 temperature is:   ");
  t6 = printTemperature(5);
  
  Serial.println();
//  (request, hum, t0, t1, t2, t3, t4, t5, t6, lum1, lum2, door):
int lum2 = 0;
  runCurl(hum, t0, t1, t2, t3, t4, t5, t6, lum1, lum2, door);
  delay(2000);
}



int readReed(){
  int sensorVal = digitalRead(reedPin);
  return sensorVal;
}

int readLight(){
  int l = analogRead(lightPin); 
  return l;
}

/*-----( Declare User-written Functions )-----*/
float printTemperature(int index)
{

float tempC = sensors.getTempCByIndex(index);
  
   if (tempC == -127.00) 
   {
   Serial.print("Error getting temperature  ");
      Serial.print(tempC);
   } 
   else
   {
   Serial.print("C: ");

   Serial.print(" F: ");
   float tempF = DallasTemperature::toFahrenheit(tempC);
   Serial.print(DallasTemperature::toFahrenheit(tempC));
   return tempF;
   }
}// End printTemperature
//  runCurl(hum, t0, t1, t2, t3, t4, t5, t6, lum1, lum2, door);
void runCurl(float hum, float t0, float t1, float t2, float t3, float t4, float t5, float t6, int lum1, int lum2, int door){
//  int light = analogRead(lightPin);
  // Launch "curl" command and get Arduino ascii art logo from the network
  // curl is command line program for transferring data using different internet protocols
  String url = "http://api-flockstats.rhcloud.com/coop/upload/" ;
  url = url + hum;
    url = url + "/" + t0 + "/" + t1 + "/" + t2 + "/" + t3 + "/" + t4 + "/" + t5 + "/" + t6 + "/" + lum1 + "/" + lum2 + "/" + door;  
  Serial.print(url);
//  return;
  Process p;        // Create a process and call it "p"
  p.begin("curl");  // Process that launch the "curl" command
  p.addParameter(url); // Add the URL parameter to "curl"
  p.run();      // Run the process and wait for its termination

  // Print arduino logo over the Serial
  // A process output can be read with the stream methods
  while (p.available()>0) {
    char c = p.read();
    Serial.print(c);
  }
  // Ensure the last bit of data is sent.
  Serial.flush();
}

