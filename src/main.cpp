

#include <Arduino.h>
#include <AccelStepper.h>
/// Wifi
#include "ESP8266WiFi.h" //I can connect to a Wifi
#include "WiFiClient.h"
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include <FS.h>

#include "config.h"
volatile int movementType = 0;
volatile int pos, speed, accel;
String moveList;

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
 * 
 * ESPASyncWebServer
 * https://github.com/me-no-dev/ESPAsyncWebServer#principles-of-operation
 */

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// handles the constant speed moves of the platform
void constantMove()
{
  stepper.setMaxSpeed(speed);

  stepper.move((pos * oneTurn) / 360);
  if (pos < 0)
  {
    speed = -speed;
  }
}

// Handles the accelerated moves of the platform
void acceleratedMove()
{
  stepper.setAcceleration(accel);
  stepper.setMaxSpeed(speed);
  //if absoulte = true, moves to a absolut position

  stepper.move((pos * oneTurn) / 360);
}

boolean getNextMove()
{
  if ((moveList == "") | (moveList == ":"))
    return false;

  int endCommandIndex = moveList.indexOf(':');
  String fullMove = moveList.substring(0, endCommandIndex);
  String mType = getValue(fullMove, '|', 0);
  String specs = getValue(fullMove, '|', 1);

  //Serial.println(fullMove);
  pos = getValue(specs, ';', 0).toInt();
  speed = getValue(specs, ';', 1).toInt();
  if (mType == "0")
  {
    movementType = 0;
  }
  if (mType == "1")
  {
    accel = getValue(specs, ';', 2).toInt();
    movementType = 1;
  }
  Serial.print(moveList + "-----  ");
  moveList = moveList.substring(endCommandIndex + 1);
  Serial.print(moveList);

  if (moveList == ":")
  {
    moveList = "";
  }

  //Serial.println(moveList);
  return true;
}

void setup()
{
  moveList = "";
  Serial.begin(9600); //I can debbug through the serial port

  // Configure NODEMCU as Access Point
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid);                //Password is not necessary
  IPAddress myIP = WiFi.softAPIP(); //Get the IP assigned to itself.
  Serial.print("AP IP address: ");  //This is written in the PC console.
  Serial.println(myIP);

  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  //ESP.wdtDisable();

  //Assigns each handler to each url
  delay(1000);

  webserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/index.htm", "text/html");
    request->send(response);
  });

  webserver.on("/main.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/main.css", "text/css");
    request->send(response);
  });

  webserver.on("/scripts.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/scripts.js", "application/javascript");
    request->send(response);
  });

  webserver.on("/multi", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("message received");
    request->send(200, "text/plain", "Message Arrived");
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
      Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
      String moveReq = request->argName(i).c_str();
      String moveArg = request->arg(i).c_str();

      //pos = (getValue(moveArg, ';', 0)).toInt();
      //speed = (getValue(moveArg, ';', 1)).toInt();

      if (moveReq == "cons")
      {
        moveArg = "0|" + moveArg;
        //constantMove(pos, speed, false);
      }
      else if (moveReq == "accel")
      {
        //accel = (getValue(moveArg, ';', 2)).toInt();
        //acceleratedMove(pos, speed, accel, false);
        moveArg = "1|" + moveArg;
      }
      moveList = moveList + moveArg + ":";
    }
  });

  webserver.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request) {
    moveList = "";
    stepper.move(0);
    movementType = 0;
    stepper.stop();
    request->send(200, "text/plain", "Motor stopped");
  });

  webserver.on("/infinite", HTTP_GET, [](AsyncWebServerRequest *request) {
    int i = 0;
    request->send(200, "text/plain", "Motor started");
    String moveReq = request->argName(i).c_str();
    String moveArg = request->arg(i).c_str();
    Serial.println(moveArg);
    speed = moveArg.toInt();
    if (speed > 0)
    {
      pos = 20;
    }
    else
    {
      pos = -20;
      speed = -speed;
    }

    movementType = 2;
    constantMove();
    request->send(200, "text/plain", "Motor stopped");
  });

  webserver.begin();

  // Change these to suit your stepper if you want
  stepper.setMaxSpeed(STEPPER_MAX_SPEED);
  stepper.setAcceleration(STEPPER_MAX_ACCEL);
}

void loop()
{
  if (getNextMove() != false)
  {
    stepper.enableOutputs();
    if (movementType == 0)
    {
      constantMove();
    }
    else if (movementType == 1)
    {
      acceleratedMove();
    }

    while (stepper.distanceToGo() != 0)
    {
      // run a step and calc when next one
      if (movementType == 0)
      {
        stepper.run();
        stepper.setSpeed(speed);
      }
      else if (movementType == 1)
      {
        stepper.run();
      }

      // reset esp8266 watchdog
      yield();
    }
  }
  else if (movementType == 2 && stepper.distanceToGo() != 0)
  {
    while (stepper.distanceToGo() != 0)
    {
      stepper.run();
      stepper.setSpeed(speed);
      yield();
      if (stepper.distanceToGo() < 100)
      {
        stepper.move(stepper.distanceToGo() * 2);
      }
    }
  }
  else
  {
    stepper.disableOutputs();
  }
}
