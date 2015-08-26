#include <Thread.h>
#include <ThreadController.h>

#define LED         BUILTIN_LED
#define GREEN_LED   2

// create a thread controler to control all threads
ThreadController mainController = ThreadController();

// blink built in led thread
Thread blinkThread = Thread();
Thread blinkGreenLedThread = Thread();

void blinkThreadCB(){
  static bool ledStatus = false;
  ledStatus = !ledStatus;

  digitalWrite(LED, ledStatus);
}

void blinkGreenLedThreadCB(){
  static bool ledGreenStatus = false;
  ledGreenStatus = !ledGreenStatus;

  digitalWrite(GREEN_LED, ledGreenStatus);
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  
  // configure blink thread
  blinkThread.onRun(blinkThreadCB);
  blinkThread.setInterval(500);

  // configure thread for green led
  blinkGreenLedThread.onRun(blinkGreenLedThreadCB);
  blinkGreenLedThread.setInterval(200);

  // add the thread to the main controller
  mainController.add(&blinkThread);
  mainController.add(&blinkGreenLedThread);
}

void loop() {
  mainController.run();
}
