#include <Thread.h>

#define LED BUILTIN_LED

// create a simple thread
Thread myThread = Thread();

// callback for myThread
void myThreadCB(){
  static bool ledStatus = false;
  ledStatus = !ledStatus;

  digitalWrite(LED, ledStatus);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
  myThread.onRun(myThreadCB);
  myThread.setInterval(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(myThread.shouldRun()){
    myThread.run();
  }
}
