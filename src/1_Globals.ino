#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LittleFS.h>

//-------------------------------------------------------Wifi-----------------------------------------------------

const char* ssid     = "Bbox-362256C8";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "UVJ5DsD6sv65E91nnQ";     // The password of the Wi-Fi network

//-------------------------------------------------------Server---------------------------------------------------

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

//-------------------------------------------------------DNS------------------------------------------------------

const String dnsName  = "esp8266";      // The name of the DNS adress

//-------------------------------------------------------Time-----------------------------------------------------

const long utcOffset = 2 * 3600;    //The relative time between UTC and UTC+2

//We define NTP client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "fr.pool.ntp.org", utcOffset);

unsigned long timeOfLastRequest;
unsigned int durationUntilNextMeasure;
const int timeOfMeasureInHour[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60};
int nbOfMeasures;

//-------------------------------------------------------DS18B20--------------------------------------------------

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature thSensor(&oneWire);

// The time that the mesure can take in milliseconds
const short measureDuration = 750;
unsigned short durationUntilRead;

bool temperatureRequested = false;
int temperature;

//-------------------------------------------------------FileManager----------------------------------------------

String actualDirectory;
String maxminDirectory = "/data/max-min.csv";

//-------------------------------------------------------Function prototypes--------------------------------------
//ServerManager :
void StartServer();
void handleRoot();
String sendLastTemp();
void handleNotFound();
String getContentType(String fileName);
bool sendFileToClient(String fileName);

//FileManager :
String GetDirectoryName();
void WriteMaximumMinimum(String fileDirectory);

//Other :
int GetTimeNextMeasureInSeconds();
String getTime();
String getDate();
