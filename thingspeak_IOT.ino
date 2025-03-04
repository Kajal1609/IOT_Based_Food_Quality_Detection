#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include "MQ135.h"
#include <ESP8266HTTPClient.h>

#define DPIN 14        //Pin to connect DHT sensor (GPIO number) D5
#define DTYPE DHT11  
#define MQ_PIN A0

#define HOST "localhost"
#define WIFI_SSID "Moon"
#define WIFI_PASSWORD "123456788"

const char* ssid = "Moon";
const char* password = "123456788";
const char* server = "api.thingspeak.com";
String apiKey = "5GVS54V7O74IHFCR";

WiFiClient client;
HTTPClient http;

DHT dht(DPIN, DTYPE);
MQ135 gasSensor(MQ_PIN);

LiquidCrystal_I2C lcd(0x27, 16, 2);

int values[15];
int valuesIndex = 0;
int step = 1;
int fid = 0;

void setup() {
  Serial.begin(115200);
  delay(10);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select Fruit");

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  dht.begin();
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  float mqValue = gasSensor.getPPM();

  if (isnan(humidity) || isnan(temperature) || isnan(mqValue)) {
    Serial.println("Failed to read from sensors!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t Temperature: ");
  Serial.print(temperature);
  Serial.print(" *C\t MQ Value: ");
  Serial.println(mqValue);

  sendToThingSpeak(temperature, humidity, mqValue);

  delay(10000); // Wait for 10 seconds before taking the next reading
}

void sendToThingSpeak(float temperature, float humidity, float mqValue) {
  if (client.connect(server, 80)) {
    String postData = "api_key=" + apiKey;
    postData += "&field1=" + String(temperature);
    postData += "&field2=" + String(humidity);
    postData += "&field3=" + String(mqValue);

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postData.length());
    client.print("\n\n");
    client.print(postData);

    Serial.println("Data sent to ThingSpeak");
  } else {
    Serial.println("Failed to connect to ThingSpeak");
  }

  client.stop();
}

void displayMessage(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
}

void checkWiFiConnection() {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to Wi-Fi...");
    delay(1000);
  }
}

void calculation(float temp, float hum) {
  Serial.print("Calculation: ");
  int length_mq = valuesIndex;
  float total_mq = 0;

  for (int i = 0; i < length_mq; i++) {
    total_mq += values[i];
  }

  float average_mq = total_mq / length_mq;
  Serial.print("average_mq = " + String(average_mq));

  http.begin(client, "http://192.168.184.191/maker_group/sensing.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST("mq=" + String(average_mq) + "&temp=" + String(temp) + "&hum=" + String(hum) + "&food_id=" + String(fid));

  if (httpCode == HTTP_CODE_OK) {
    fid = 0;
    String response = http.getString();
    Serial.println("Server response: " + response);

    // Trim leading and trailing whitespace from the response
    response.trim();

    Serial.println("Trimmed response: " + response); // Debug print

    // Check the equality condition
    if (response.equals("fresh")) {
      displayMessage("FOOD IS FRESH");
    } else {
      displayMessage("FOOD IS SPOILED");
    }

    // Reset values array and index
    memset(values, 0, sizeof(values));
    valuesIndex = 0;
    step = 4;
  }
  else {
    Serial.print("HTTP POST request failed with error code: ");
    Serial.println(httpCode);
    if (httpCode == HTTPC_ERROR_CONNECTION_REFUSED) {
      Serial.println("Connection refused by the server.");
    }
    else if (httpCode == HTTP_CODE_NOT_FOUND) {
      Serial.println("Server resource not found.");
    }
    else {
      Serial.println("Unknown error occurred.");
    }
  }

  http.end();
}