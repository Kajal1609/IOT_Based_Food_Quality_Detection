
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define HOST "localhost"
#define WIFI_SSID "Moon"
#define WIFI_PASSWORD "123456788"

String sendval, sendval2, postData;

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

void setup() 
{
  Serial.begin(115200);
  Serial.println("Communication Started\n");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  checkWiFiConnection();
}

void loop()
 {
  checkWiFiConnection();
  WiFiClient client;
  HTTPClient http;

  int temperature = random(18,38);// you can get the actual value from attached sensor
  int humidity = random(60,90);// you can get the actual value from attached sensor
  sendval = String(temperature);
  sendval2 = String(humidity);

  postData = "";
  Serial.print("Sending data: ");
  Serial.println(postData);

  http.begin(client, "http://192.168.182.191/maker_group/server.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST(postData);

  if (httpCode == HTTP_CODE_OK) 
  {
    String response = http.getString();
    Serial.println("Server response: " + response);
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
  delay(5000);
}