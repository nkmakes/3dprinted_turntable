/**
 * ESP 8266 turntable test and example code
 * Works with accelstepper library, big thanks to creators!
 * Also implements a webserver which accepts a move by the argument:
 * 192.168.XXX.XXX/Move?Speed=250&Pos=360&Accel=50&Abs=1&Cons=1
 * Speed: Int : Speed of the move
 * Pos: Int: Desired position (In degrees), positive(CW) or negative(CCW)
 * Accel: Int : Max acceleration
 * Abs : Int : 1= Abolute position movement, 0= Relative position movement
 * Cons: Int : 1= Constant speed movement, 0= Accelerated movement 
 * Only working with accelerated moves, not constant speed
 * instagram.com/spicy_coder 01/2020
 */

#include <Arduino.h>
#include <AccelStepper.h>
/// Wifi
#include "ESP8266WiFi.h"        //I can connect to a Wifi
#include "ESP8266WebServer.h"   //I can be a server 'cos I have the class ESP8266WebServer available
#include "WiFiClient.h"
const char *ssid = "ESPap";  //Credentials to register network defined by the SSID (Service Set IDentifier)
const char *password = "PASSWORD"; //and the second one a password if you wish to use it.
ESP8266WebServer server(80);    //Class ESP8266WebServer and default port for HTTP

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::FULL4WIRE, D5, D6, D7, D8);

// 1 rotation = 14336
int oneTurn = 14336;


void handleAccel() { // Handler. 192.168.XXX.XXX/accell?Accel=100&Speed=250&Pos=360(&Abs)
  String message = "Accelerated move with: ";
  // Default values
  int msgSpeed = 500;
  int msgPos = 90;
  int msgAccel = 100;

  if (server.hasArg("Speed")) {
    msgSpeed = (server.arg("Speed")).toInt(); //Converts the string to integer.
    // set the speed for the move
    stepper.setMaxSpeed(msgSpeed);
    message += " Speed: ";
    message += server.arg("Speed");
  }

  if (server.hasArg("Pos")) {
    // sets the position
    msgPos = (server.arg("Pos")).toInt();
    message += " Pos: ";
    message += server.arg("Pos");
  }

  if (server.hasArg("Accell")) {
    // sets the acceleration
    msgAccel = (server.arg("Accel")).toInt();
    stepper.setAcceleration(msgAccel);
    message += " Accel: ";
    message += server.arg("Accel");
  }

  if (server.hasArg("Abs")) {
    stepper.moveTo(msgPos*oneTurn/360);
    message += " Absolut: Yes";
  } else {
    stepper.move(msgPos*oneTurn/360);
    message += " Absolut: No";
  }
  
  server.send(200, "text/plain", message);
  
  stepper.enableOutputs();

  while (stepper.distanceToGo() != 0) {
    // run a step and calc when next one
    stepper.run();
    // reset esp8266 watchdog
    yield();
    // manage server
    server.handleClient();
  }
  stepper.disableOutputs();
}

void handleConstant() {// Handler. 192.168.XXX.XXX/cons?Speed=250&Pos=360(&Abs)
  
  int msgPos = 90;
  int msgSpeed = 500;
  String message = "Constant move with: ";

  if (server.hasArg("Speed")) {
    msgSpeed = (server.arg("Speed")).toInt(); //Converts the string to integer.
    // set the speed for the move
    stepper.setMaxSpeed(msgSpeed);
    stepper.setSpeed(msgSpeed);
    message += " Speed: ";
    message += server.arg("Speed");
  }
  if (server.hasArg("Pos")) {
    // sets the position
    msgPos = (server.arg("Pos")).toInt();
    message += " Pos: ";
    message += server.arg("Pos");
  }
  if (server.hasArg("Abs")) {
    stepper.moveTo(msgPos*oneTurn/360);
    message += " Absolut: Yes";
  } else {
    stepper.move(msgPos*oneTurn/360);
    message += " Absolut: No";
  }

  server.send(200, "text/plain", message);

  // enable the motor before moving
  stepper.enableOutputs();
  
  while (stepper.distanceToGo() != 0) {
    // run a step and calc when next one
    stepper.run();
    stepper.setSpeed(msgSpeed);
    // reset esp8266 watchdog
    yield();
    // manage server
    server.handleClient();
  }

  // disable motors after move for to cool them down
  stepper.disableOutputs();
  
}


void handleRootPath() {
 server.send(200, "text/plain", "Ready.");
}

void setup()
{  
  Serial.begin(9600); //I can debbug through the serial port

  // Configure NODEMCU as Access Point
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid); //Password is not necessary
  IPAddress myIP = WiFi.softAPIP(); //Get the IP assigned to itself.
  Serial.print("AP IP address: "); //This is written in the PC console.
  Serial.println(myIP);

  delay(1000);
  server.on("/", handleRootPath); 
  server.on("/accel", handleAccel);
  server.on("/cons", handleConstant);

  server.begin(); //Let's call the begin method on the server object to start the server.
  Serial.println("HTTP server started");


  // Change these to suit your stepper if you want
  stepper.setMaxSpeed(200);
  stepper.setAcceleration(50);
}

void loop()
{
    // If we dont need to move we disable outputs (no heat)
    if (stepper.distanceToGo() == 0) {
      stepper.disableOutputs();
    } else {
      stepper.enableOutputs();
    }
    //handle client
    server.handleClient();
    yield();
}