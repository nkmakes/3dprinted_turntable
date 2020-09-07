

#include <Arduino.h>
#include <AccelStepper.h>
/// Wifi
#include "ESP8266WiFi.h" //I can connect to a Wifi
#include "WiFiClient.h"
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
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
void constantMove(int pos, int speed, bool abs)
{

  stepper.setMaxSpeed(speed);

  if (abs == true)
  {
    stepper.moveTo((pos * oneTurn) / 360);
    if (pos * oneTurn / 360 < stepper.targetPosition())
    {
      speed = -speed;
    }
  }
  else
  {
    stepper.move((pos * oneTurn) / 360);
    if (pos < 0)
    {
      speed = -speed;
    }
  }

  stepper.enableOutputs();

  while (stepper.distanceToGo() != 0)
  {
    // run a step and calc when next one
    stepper.run();
    stepper.setSpeed(speed);
    // reset esp8266 watchdog
    yield();
  }

  // disable motors after move for to cool them down
  stepper.disableOutputs();
}

// Handles the accelerated moves of the platform
void acceleratedMove(int pos, int speed, int accel, bool abs)
{

  stepper.setAcceleration(accel);
  stepper.setMaxSpeed(speed);

  //if absoulte = true, moves to a absolut position
  if (abs == true)
  {
    stepper.moveTo((pos * oneTurn) / 360);
  }
  else
  {
    stepper.move((pos * oneTurn) / 360);
  }

  stepper.enableOutputs();

  while (stepper.distanceToGo() != 0)
  {
    // run a step and calc when next one
    stepper.run();
    // reset esp8266 watchdog
    yield();
  }
  stepper.disableOutputs();
}



void setup()
{
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
      String moveReq = request->argName(i).c_str() ;
      String moveArg = request->arg(i).c_str();

      int pos = (getValue(moveArg, ';', 0)).toInt();
      int speed = (getValue(moveArg, ';', 1)).toInt();

      if (moveReq == "cons")
      {
        //constantMove(pos, speed, false);
      }
      else if (moveReq == "accel")
      {
        int accel = (getValue(moveArg, ';', 2)).toInt();
        //acceleratedMove(pos, speed, accel, false);
      }
    }

    
  });

  webserver.begin();

  // Change these to suit your stepper if you want
  stepper.setMaxSpeed(STEPPER_MAX_SPEED);
  stepper.setAcceleration(STEPPER_MAX_ACCEL);
}


void loop()
{
  // If we dont need to move we disable outputs (no heat)
  /**
  while (client.available())
  {
    client.poll();

    if (stepper.distanceToGo() == 0)
    {
      stepper.disableOutputs();
    }
    else
    {
      stepper.enableOutputs();
    }
    //handle client
    yield();

  }**/
}