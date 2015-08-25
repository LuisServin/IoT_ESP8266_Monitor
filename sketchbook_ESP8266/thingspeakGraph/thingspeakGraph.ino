// Plot DTH11 data on thingspeak.com using an ESP8266
// April 11 2015
// Author: Jeroen Beemster
// Website: www.arduinesp.com

#include <ESP8266WiFi.h>

String apiKey = "X9VKUQGAL6CVVAXB";
const char* ssid = "Hogwarts";
const char* password = "lacasa37304";

const char* server = "api.thingspeak.com";

WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
}

void loop() {
  // put your main code here, to run repeatedly:
  float h = random(0, 255);
  float t = random(0, 255);

  if(client.connect(server,80)) {
    String  postStr = apiKey;
            postStr += "&field1=";
            postStr += String(t);
            postStr += "&field2=";
            postStr += String(h);
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

    Serial.println("Data sent!...");
  }
  client.stop();

  Serial.println("Waiting...");
  // thingspeak needs minimum 15 sec delay between updates
  delay(20000);
}














