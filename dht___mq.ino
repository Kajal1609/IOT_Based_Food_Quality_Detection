

#include <DHT.h>  // Including library for dht
#include "MQ135.h"
#include <ESP8266WiFi.h> 
#include "DHT.h"
#define DPIN 14        //Pin to connect DHT sensor (GPIO number) D2
#define DTYPE DHT11  
 
String apiKey = "5GVS54V7O74IHFCR";     //  Enter your Write API key from ThingSpeak
 
const char *ssid =  "Moon";     // replace with your wifi ssid and wpa2 key
const char *pass =  "123456788";
const char* server = "api.thingspeak.com";
const int sensorPin= 0;
int methane_level;
float sensorValue;

DHT mydht(DPIN, DTYPE);
 
WiFiClient client;
 
void setup() 
{
       Serial.begin(115200);
       delay(10);
       mydht.begin();
 
       Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
 
}
 
void loop() {

    sensorValue = analogRead(A0);
    methane_level= map(sensorValue, 0, 1023, 0, 10000); // Example calibration

  if (isnan(sensorValue)){
  Serial.println("Failed to read from MQ-4 sensor!");
    return;
  }
  
      float h = mydht.readHumidity();
      float t = mydht.readTemperature();
      //int gasValue = analogRead(gas);
      //MQ135 gasSensor = MQ135(A0);
      //methane_level = gasSensor.getPPM();
             if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr +="&field3=";
                             postStr += String(methane_level);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.print("%, Gas Value: ");
                             Serial.print(methane_level);
                             //Serial.print("%, Methane Level: ");
                             //Serial.print(methane_level);
                             Serial.println("Send to Thingspeak.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates
  delay(1000);
  
}
