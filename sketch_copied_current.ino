//esp8266
#include <ArduinoJson.h>
#include<ESP8266WiFi.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  
#include "DHT.h"
#define DPIN 14        //Pin to connect DHT sensor (GPIO number) D2
#define DTYPE DHT11  
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define HOST "localhost"
#define WIFI_SSID "Moon"
#define WIFI_PASSWORD "123456788"   // Include the Wi-Fi library
const char* ssid     = "Moon";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "123456788";   
WiFiClient client;
HTTPClient http;
unsigned long startTime;
const int interval = 10000; // 1000 milliseconds = 1 second
const int maxValues = 15; // Adjust based on how many values you expect
int values[maxValues];
int valuesIndex = 0;
int step=1;
float sensorValue;
float mq;
LiquidCrystal_I2C lcd(0x27, 16, 2);
  // The password of the Wi-Fi network
  DHT dht(DPIN,DTYPE);
  String sendval, sendval2, postData;
int fid=0;
 String fruitname="";

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  lcd.clear();
  lcd.setCursor(0, 0);               // Set the cursor to the first column and first row
  lcd.print("Select Fruit");     // Print some text
  lcd.setCursor(0,1);   
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

lcd.init();                       // Initialize the LCD
  lcd.backlight();                  // Turn on the backlight
  lcd.clear(); 
  dht.begin();
  Serial.println("Communication Started\n");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  checkWiFiConnection();
  }





void loop() {
  sensorValue = analogRead(A0);
mq = map(sensorValue, 0, 1023, 0, 10000); // Example calibration

  if (isnan(sensorValue)){
  Serial.println("Failed to read from MQ-4 sensor!");
    return;
  }
  float tc = dht.readTemperature(false);  //Read temperature in C
  float tf = dht.readTemperature(true);   //Read Temperature in F
  float hu = dht.readHumidity();          //Read Humidity
  
  Serial.print("GAS DATA :"+String(mq));
  Serial.print("Temperature :"+String(tc));
  Serial.println("Humadity :"+String(hu));
  
  delay(100);
  checkWiFiConnection();
 

  int temperature = random(18,38);// you can get the actual value from attached sensor
  int humidity = random(60,90);// you can get the actual value from attached sensor
  sendval = String(temperature);
  sendval2 = String(humidity);

  postData = "mq="+String(mq)+"&temp="+String(tc)+"&hum="+String(hu);
  switch(step){
    case 1: delay(2000);
            requestweb();
            break;
    case 2:start_sensing(mq,fid);
            break;
    case 3: display("processing");
            calculation(tc,hu);
             
           break;
    case 4:delay(7000);
            step=1;
            break;
  }
  delay(1000);
}

 int requestweb(){
  display("Select Fruit");
  Serial.print("Sending data: ");
 
  http.begin(client, "http://192.168.184.191/maker_group/server.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) 
  {
    String response = http.getString();
    Serial.println("Server response: " + response);
     Serial.println(response);
    if(response=="select fruit"){
      display("select fruit");
    }else{
       StaticJsonDocument<200> doc;
       
  // Deserialize the JSON document
          DeserializationError error = deserializeJson(doc, response);
                if (error) {
                  Serial.print(F("deserializeJson() failed: "));
                  Serial.println(error.f_str());
                }else{
     fid=doc[0].as<int>();
     fruitname=doc[1].as<String>();}
    }
  if(fid!=0 ){
  Serial.println(fid); 

  Serial.println(); 
  display(fruitname +" Selected");
    step=2;
  }
  }
  else
  {
    Serial.print("HTTP POST request failed with error code: ");
    Serial.println(httpCode);
    if (httpCode == HTTPC_ERROR_CONNECTION_REFUSED)
    {
      Serial.println("Connection refused by the server.");
    }
    else if (httpCode == HTTP_CODE_NOT_FOUND) 
    {
      Serial.println("Server resource not found.");
    }
    else {
      Serial.println("Unknown error occurred.");
    }
  }
  http.end();
  return 0;
}
void checkWiFiConnection() 
{

  while (WiFi.status() != WL_CONNECTED)
   {
    Serial.println("Connecting to Wi-Fi...");
    delay(1000);
  }

  Serial.println("Connected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

}
void start_sensing(float m,int id){
   display("sensing fruit");
     if (valuesIndex >= maxValues) {
     step=3;
    for (int i = 0; i < maxValues; i++) {
      Serial.println(values[i]);
      return ;
    }

    // Here, we effectively "pause" the program; you might want to reset or do something else
    while (true) {
      
      // Do nothing - or replace this with a reset if you prefer
    }

  }
  else{
    values[valuesIndex]=m;
    valuesIndex++;
     Serial.println(valuesIndex +" "+ maxValues);
  }

}
void calculation(float temp,float hum){
   Serial.print("Calculation: ");
  int length_mq=sizeof(values);
  float total_mq;
  for(int i=0;i<length_mq;i++){
     total_mq+=values[i];
  }
  float average_mq=total_mq/length_mq;
  http.begin(client, "http://192.168.184.191/maker_group/sensing.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST("mq=$average_mq&temp=$temp&hum=$hum&food_id=$fid");
  if (httpCode == HTTP_CODE_OK) 
  {
    fid=0;
    String response = http.getString();
    Serial.println("Server response: " + response);
    
    if(response=="fresh")

    {
    display("FOOD IS FRESH");}
    else{
      display("FOOD IS SPOILED");
    }
      memset(values,0,sizeof(values));
     valuesIndex=0;
    step=4;
  }
  else
  {
    Serial.print("HTTP POST request failed with error code: ");
    Serial.println(httpCode);
    if (httpCode == HTTPC_ERROR_CONNECTION_REFUSED)
    {
      Serial.println("Connection refused by the server.");
    }
    else if (httpCode == HTTP_CODE_NOT_FOUND) 
    {
      Serial.println("Server resource not found.");
    }
    else {
      Serial.println("Unknown error occurred.");
    }
  }
  http.end();
}
void display(String a){
lcd.clear();
  lcd.setCursor(0, 0);               // Set the cursor to the first column and first row
  lcd.print(a);     // Print some text
  lcd.setCursor(0,1);   
}











