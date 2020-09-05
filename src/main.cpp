
#include <Arduino.h>
#include <AccelStepper.h>
/// Wifi
#include "ESP8266WiFi.h"        //I can connect to a Wifi
#include "ESP8266WebServer.h"   //I can be a server 'cos I have the class ESP8266WebServer available
#include "WiFiClient.h"
#include <FS.h>
#include "config.h"
/**
 * ESP 8266 turntable test and example code
 * 
 * Also implements a webserver which accepts two move by the argument:
 * For constant moves:
 * 192.168.XXX.XXX/cons?Speed=250&Pos=360(&Abs)
 * 
 * For accelerated moves:
 * 192.168.XXX.XXX/accel?Speed=250&Pos=360&Accel=100(&Abs)
 * 
 * Speed: Int : Speed of the move
 * Pos: Int: Desired position (In degrees), positive(CW) or negative(CCW)
 * Accel: Int : Max acceleration
 * Abs : boolean : If present, moves to absolut position
 * 
 * Works with accelstepper library, big thanks to airspayce!
 * Check out latest version of documentation in link below
 * http://www.airspayce.com/mikem/arduino/AccelStepper
 * 
 */


const char *ssid = "ESPap";  //Credentials to register network defined by the SSID (Service Set IDentifier)
const char *password = "PASSWORD"; //and the second one a password if you wish to use it.
ESP8266WebServer server(80);    //Class ESP8266WebServer and default port for HTTP

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::FULL4WIRE, D5, D6, D7, D8);

// 1 rotation = 14336
int oneTurn = 14336;
// https://stackoverflow.com/questions/9072320/split-string-into-string-array

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


// handles the constant speed moves of the platform
void constantMove(int pos, int speed, bool abs) {

  stepper.setMaxSpeed(speed);
  
  if (abs==true) {
    stepper.moveTo((pos*oneTurn)/360);
      if(pos*oneTurn/360<stepper.targetPosition()) {
        speed=-speed;
      }
  } else {
    stepper.move((pos*oneTurn)/360);
    if (pos<0) {
      speed=-speed;
    }
  }

  stepper.enableOutputs();

  while (stepper.distanceToGo() != 0) {
    // run a step and calc when next one
    stepper.run();
    stepper.setSpeed(speed);
    // reset esp8266 watchdog
    yield();
    // manage server
    server.handleClient();
  }

  // disable motors after move for to cool them down
  stepper.disableOutputs();
}

// Handles the accelerated moves of the platform
void acceleratedMove(int pos, int speed, int accel, bool abs) {
  
  stepper.setAcceleration(accel);
  stepper.setMaxSpeed(speed);

  //if absoulte = true, moves to a absolut position
  if (abs==true) {
    stepper.moveTo((pos*oneTurn)/360);
  } else {
    stepper.move((pos*oneTurn)/360);
  }

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

// handles one HTTP message for multiple moves
// Handler. 192.168.XXX.XXX/multi?cons=pos-speed&accel=pos-speed-accel
void handleMultiple() {
  String message = "multiple moves";
  server.send(200, "text/plain", message);
  for (int i = 0; i < server.args(); i++) {
    message += "Arg nº" + (String)i + " –> ";
    message += server.argName(i) + ": ";
    message += server.arg(i) + "\n";
    if (server.argName(i)=="cons") {
      int pos = (getValue(server.arg(i),'_',0)).toInt();
      int speed = (getValue(server.arg(i),'_',1)).toInt();
      constantMove(pos,speed,false);
    } else {
      int pos = (getValue(server.arg(i),'_',0)).toInt();
      int speed = (getValue(server.arg(i),'_',1)).toInt();
      int accel = (getValue(server.arg(i),'_',2)).toInt();
      acceleratedMove(pos,speed,accel,false);
    }
    server.send(200, "text/plain", message);
  }
  Serial.print(message);
}

// handles the HTTP messages for Accelerated moves
void handleAccel() { // Handler. 192.168.XXX.XXX/accell?Accel=100&Speed=250&Pos=360(&Abs)
  // Default values
  int msgSpeed = 500;
  int msgPos = 90;
  int msgAccel = 100;
  int msgAbs = false;

  if (server.hasArg("Speed")) stepper.setMaxSpeed((server.arg("Speed")).toInt());

  if (server.hasArg("Pos")) msgPos = (server.arg("Pos")).toInt();
  
  if (server.hasArg("Accell")) stepper.setAcceleration((server.arg("Accel")).toInt());

  if (server.hasArg("Abs")) msgAbs = true;
  
  acceleratedMove(msgPos, msgSpeed, msgAccel, msgAbs);
}

// handles the HTTP messages for constant speed moves
void handleConstant() { // Handler. 192.168.XXX.XXX/cons?Speed=250&Pos=360(&Abs)
  
  int msgPos = 90;
  int msgSpeed = 500;
  bool msgAbs = false;

  if (server.hasArg("Speed")) stepper.setMaxSpeed((server.arg("Speed")).toInt());

  if (server.hasArg("Pos")) msgPos = (server.arg("Pos")).toInt();
  
  if (server.hasArg("Abs")) msgAbs = true;

  constantMove(msgPos, msgSpeed, msgAbs);
}


void handleRootPath() {
//  String s = MAIN_page;
//  server.send(200, "text/html", s);
  server.serveStatic("/", SPIFFS, "/index.htm");
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

  //Assigns each handler to each url
  delay(1000);
  server.on("/", handleRootPath);
  
  server.on("/accel", handleAccel);
  server.on("/cons", handleConstant);
  server.on("/multi", handleMultiple);

  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
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