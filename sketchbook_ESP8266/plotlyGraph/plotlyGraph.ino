#include <ESP8266WiFi.h>

#define ssid      "Hogwarts"
#define password  "lacasa37304"
#define userName  "luis.alfredo.sega"
#define APIKey    "dky1fxqme8"
#define fileName  "test"
#define fileopt   "overwrite"
#define nTraces   1
#define maxpoints "30"
#define world_readable  true
#define convertTimestamp  true
#define timezone  "Australia/Malbourne"

char *tokens[nTraces] = {"af5oolbi8s"};
char stream_site[25] = {0};

WiFiClient client;

/* ----------------- Setup -----------------*/
void setup() {
  // Setup Serial
  Serial.begin(9600);
  delay(2000);
  Serial.println();
  Serial.println();
  ESP8266_Init();
  ESP8266_Connect("plot.ly", 80);
  Serial.println("Initializing plot with Plot.ly server...");
  plotly_init();
  Serial.println("Make sure disconnected...");
  client.stop();
  Serial.println("Done!");
  ESP8266_Connect("arduino.plot.ly", 80);  
}

/* ----------------- Loop -----------------*/

void loop() {
  // put your main code here, to run repeatedly:
  // int val = analogRead(A0);
  int val = random(0,255);
  plotly_plot(millis(), val, tokens[0]);
  delay(200);
}

/* ----------------- Functions -----------------*/
void ESP8266_Init()
{
  Serial.println("------------------------------");
  Serial.println("        INITIALIZING...       ");
  Serial.println("------------------------------"); 

  // set WiFi into Station mode
  Serial.println("1. Setting WiFi into station mode...");
  WiFi.mode(WIFI_STA);
  Serial.println("  Done!\r\n");

  // connect to WiFi
  Serial.print("2. Connecting to: \"");
  Serial.print(ssid);
  Serial.print("\",\"");
  Serial.print(password);
  Serial.println("\"...");

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("  WiFi Connected!");
  Serial.print("  IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.println("------------------------------");
  Serial.println("   INITIALIZATION COMPLETE!   ");
  Serial.println("------------------------------"); 
}

void ESP8266_Connect(char* url, int port)
{
  Serial.print("Connecting to: \"");
  Serial.print(url);
  Serial.println("\" server...");

  if(!client.connect(url, port)){
    Serial.println("*Connection Failed!\r\n");
    while(1){};
  } else {
    Serial.println("Connected!");
  }
}

void plotly_init()
{
  unsigned int i = 0;
  char charbuffer;

  // Calculate content length
  unsigned int contentLength = 126 + strlen(userName) + 
    strlen(fileopt) + nTraces*(87+strlen(maxpoints)) + 
    (nTraces - 1)*2 + strlen(fileName);
  if(world_readable){
    contentLength += 4;
  } else {
    contentLength += 5;
  }

  String contentLengthString = String(contentLength);
  const char* contentLengthConstString = contentLengthString.c_str();

  unsigned int postLength = contentLength + 94 + strlen(contentLengthConstString);

  // send post and initialization data
  client.print(
    String("POST /clientresp HTTP/1.1\r\n")+
    "Host: plot.ly:80\r\n" +
    "User-Agent: Arduino/0.6.0\r\n" +
    "Content-Length: " + String(contentLength) + "\r\n\r\n" +
    "version=2.3&origin=plot&platform=arduino&un=" + userName +
    "&key=" + APIKey + "&args=["
  );

  for(int i=0; i<nTraces; i++){
    client.print(
      String("{\"y\": [], \"x\": [], \"type\": \"scatter\", \"stream\": {\"token\": \"") +
      String(tokens[i]) + 
      "\", \"maxpoints\": " +
      String(maxpoints) +
      "}}"
    );
    if(nTraces > 1 && i != nTraces - 1){
      client.print(", ");
    }
  }
  
  client.print(
    String("]&kwargs={\"fileopt\": \"") + fileopt +
    "\", \"filename\": \"" + fileName +
    "\", \"world_readable\": "
  );
  if(world_readable){
    client.print("true}");
  } else {
    client.print("false}");
  }

  // look for "~" in server response to see web address, the 
  // next '"' is the end
  if(client.find("~")){
    i = 0;
    while(1){
      while(!client.available()){}
      charbuffer = client.read();
      if(charbuffer == '\"'){
        break;
      }
      stream_site[i] = charbuffer;
      i++;
    }
    Serial.println("Successfully Initialized.");
    Serial.print("Please visit: \"http://plot.ly/~");
    i = 0;
    while(stream_site[i]){
      Serial.print(stream_site[i]);
      i++;
    }
    Serial.println("\".");
  } else {
    Serial.println("*ERROR!* Can`t find \"~\"");
    while(1){}
  }
}

void plotly_plot(unsigned long x, int y, char* token)
{
  String xString = String(x);
  String yString = String(y);

  const char* xConstString = xString.c_str();
  const char* yConstString = yString.c_str();

  unsigned int jasonLength = 44 + strlen(xConstString) +
    strlen(yConstString);
  String jasonLengthString = String(jasonLength, HEX);

  const char* ConstJasonLengthString = jasonLengthString.c_str();
  unsigned int postLength = 167 + strlen(ConstJasonLengthString) +
    jasonLength;

  //Serial.print("Data sending... ");
  client.print(
    String("POST / HTTP/1.1\r\n") +
    "Host: arduino.plot.ly\r\n" +
    "User-Agent: Arduino\r\n" +
    "Transfer-Encoding: chunked\r\n" +
    "Connection: close\r\n" +
    "plotly-convertTimestamp: \"Australia/Melbourne\"\r\n\r\n" +
    jasonLengthString +
    "\r\n{\"x\": " + xString + ", \"y\": " + yString +
    ", \"streamtoken\": \"" + token + "\"}\n\r\n" + "0\r\n\r\n"
  );
  //Serial.println("Data Sent!");
}











