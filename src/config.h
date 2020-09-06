const char *ssid = "ESPap";        //Credentials to register network defined by the SSID (Service Set IDentifier)
const char *password = "PASSWORD"; //and the second one a password if you wish to use it.
ESP8266WebServer server(80);       //Class ESP8266WebServer and default port for HTTP

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::FULL4WIRE, D5, D6, D7, D8);

// 1 rotation = 14336
int oneTurn = 14336;
// https://stackoverflow.com/questions/9072320/split-string-into-string-array