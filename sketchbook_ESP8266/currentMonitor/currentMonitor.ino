#include <ESP8266WiFi.h>
#include <Thread.h>
#include <ThreadController.h>

#define GREEN_LED           2         //GPIO2
#define BAUD_RATE           9600    
#define UPDATE_GRAPH_TIME   1000
#define UPDATE_SENSOR_TIME  200

#define SSID              "Hogwarts"
#define PASSWORD          "lacasa37304"
#define USER_NAME         "luis.alfredo.sega"
#define API_KEY           "dky1fxqme8"
#define FILE_NAME         "test"
#define FILE_OPT          "overwrite"
#define N_TRACES          1
#define MAX_POINTS        "30"
#define WORLD_READABLE    true
#define CONVERT_TIMESTAMP true
#define TIME_ZONE         "America/Mexico_City"

char *tokens[N_TRACES] = {"af5oolbi8s"};
char stream_site[25] = {0};

// Program variables declaration
int val;

// create the instance for the WiFi Client
WiFiClient client;

// crete a thread controller to control all threads
ThreadController mainController = ThreadController();

// create the threads to be use
// Thread for reading the value in the currentSensor
Thread tCurrentSensor = Thread();
// Thread for uploading the value to the cloud
Thread tUploadGraph = Thread();

// callback function for reading current sensor thread
void tCurrentSensorCB(){
  val = random(0,255);
}

// callback function for uploading the graph in the cloud
void tUploadGraphCB(){
  plotly_plot(millis(), val, tokens[0]);
}

void setup() {
  // Initialization rutine for connecting to the network
  Serial.begin(BAUD_RATE);
  delay(2000);

  // set GPIO2 pin as OUTPUT for visual signal
  pinMode(GREEN_LED, OUTPUT);
  
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

  // configure thread for reading sensor
  tCurrentSensor.onRun(tCurrentSensorCB);
  tCurrentSensor.setInterval(UPDATE_SENSOR_TIME);

  // configure thread for updating graph
  tUploadGraph.onRun(tUploadGraphCB);
  tUploadGraph.setInterval(UPDATE_GRAPH_TIME);

  // add the thread to the main thread controller
  mainController.add(&tCurrentSensor);
  mainController.add(&tUploadGraph);
}

void loop() {
  mainController.run();
}

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
  Serial.print(SSID);
  Serial.print("\",\"");
  Serial.print(PASSWORD);
  Serial.println("\"...");

  WiFi.begin(SSID, PASSWORD);
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
  unsigned int contentLength = 126 + strlen(USER_NAME) + 
    strlen(FILE_OPT) + N_TRACES*(87+strlen(MAX_POINTS)) + 
    (N_TRACES - 1)*2 + strlen(FILE_NAME);
  if(WORLD_READABLE){
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
    "version=2.3&origin=plot&platform=arduino&un=" + USER_NAME +
    "&key=" + API_KEY + "&args=["
  );

  for(int i=0; i<N_TRACES; i++){
    client.print(
      String("{\"y\": [], \"x\": [], \"type\": \"scatter\", \"stream\": {\"token\": \"") +
      String(tokens[i]) + 
      "\", \"maxpoints\": " +
      String(MAX_POINTS) +
      "}}"
    );
    if(N_TRACES > 1 && i != N_TRACES - 1){
      client.print(", ");
    }
  }
  
  client.print(
    String("]&kwargs={\"fileopt\": \"") + FILE_OPT +
    "\", \"filename\": \"" + FILE_NAME +
    "\", \"world_readable\": "
  );
  if(WORLD_READABLE){
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
  Serial.println("Data Sent!");
}
