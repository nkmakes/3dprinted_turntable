
// Wifi config
const char *ssid = "ESPap";        //Credentials to register network defined by the SSID (Service Set IDentifier)
const char *password = "PASSWORD"; //and the second one a password if you wish to use it.

// Webserver config
AsyncWebServer webserver(80);
AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws
AsyncEventSource events("/events"); // event source (Server-Sent events)


// Stepper Config
AccelStepper stepper(AccelStepper::FULL4WIRE, D5, D6, D7, D8); // Stepper Wiring
const int oneTurn = 14336;                                     // One turn of the platform in steps
const int STEPPER_MAX_SPEED = 200;
const int STEPPER_MAX_ACCEL = 50;
// https://stackoverflow.com/questions/9072320/split-string-into-string-array
