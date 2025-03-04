#include "DHT.h"

#define DPIN 4        //Pin to connect DHT sensor (GPIO number) D2
#define DTYPE DHT11   // Define DHT 11 or DHT22 sensor type

DHT dht(DPIN,DTYPE);


void setup(){
  Serial.begin(115200);
  dht.begin();
  delay(10);
}

void loop(){
  float h = analogRead(A0);
  if (isnan(h)){
  Serial.println("Failed to read from MQ-4 sensor!");
    return;
  }
  float tc = dht.readTemperature(false);  //Read temperature in C
  float tf = dht.readTemperature(true);   //Read Temperature in F
  float hu = dht.readHumidity();          //Read Humidity
  
  Serial.print("GAS DATA :"+String(h/1023*100));
  Serial.print("     Temperature :"+String(tc));
  Serial.println("     Humadity :"+String(hu));
  
  delay(100);
}
