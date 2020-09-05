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