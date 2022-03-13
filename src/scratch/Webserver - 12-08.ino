#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <FS.h>

//-------------------------------------------------------Variables-----------------------------------------------------

const char* ssid     = "Bbox-362256C8";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "UVJ5DsD6sv65E91nnQ";     // The password of the Wi-Fi network

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)

void handleRoot();              // function prototypes for HTTP handlers
void handleButton();
void handleNotFound();

const String dnsName  = "esp8266";      // The name of the DNS adress

const long utcOffset = 2 * 3600;    //The relative time between UTC and UTC+2

//We define NTP client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "fr.pool.ntp.org", utcOffset);

unsigned long timeOfLastRequest;
unsigned int durationUntilNextMeasure;
const int timeOfMeasureInHour[] = {0, 15, 20, 25, 30, 35, 40, 45, 50, 55};
int nbOfMeasures;

//Set DS18B20 :

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

//-------------------------------------------------------Setup---------------------------------------------------------
void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  thSensor.setWaitForConversion(false);      //Don't block the program while the temperature sensor is reading
  thSensor.begin();                         //Start the temperature sensor

  SPIFFS.begin();                           // Start the SPI Flash Files System
  
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print('.');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

  if (!MDNS.begin(dnsName)) {             // Start the mDNS responder for esp8266.local
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
  Serial.println("Local URL : " + dnsName + ".local");

  //We set the NTP client
  timeClient.begin();

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");

  
  server.on("/", handleRoot);               // Call the 'handleRoot' function when a client requests URI "/"
  server.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(server.uri()))                  // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });

  nbOfMeasures = sizeof(timeOfMeasureInHour);

  timeClient.update();
  timeOfLastRequest = millis();
  durationUntilNextMeasure = GetTimeNextMeasureInSeconds() * 1000;
}

//-------------------------------------------------------Loop----------------------------------------------------------
void loop() {
  server.handleClient();

  if (millis() - timeOfLastRequest >= durationUntilNextMeasure){
    if (temperatureRequested){
      
    }
    
    timeClient.update();

    timeOfLastRequest = millis();
    durationUntilNextMeasure = GetTimeNextMeasureInSeconds() * 1000;

    Serial.println("Requesting temperature at " + getTime());
    thSensor.requestTemperatures();
    temperatureRequested = true;
  }
  else if (temperatureRequested && millis() - timeOfLastRequest >= measureDuration) {
    Serial.println("Reading temperature at " + getTime());

    temperatureRequested = false;
    temperature = thSensor.getTempCByIndex(0);

    Serial.println(temperature);

    File tempLog = SPIFFS.open("/temp.csv", "a"); // Write the time and the temperature to the csv file
    tempLog.print(getTime());
    tempLog.print(',');
    tempLog.println(temperature);
    tempLog.close();
  }
}

//-------------------------------------------------------Functions-----------------------------------------------------
void handleRoot() {
  server.send(200, "text/html", "<body>" + String(millis() - timeOfLastRequest) + ", " + String(durationUntilNextMeasure) + "</body>");   // Send HTTP status 200 (Ok) and send some text to the browser/client
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

String getContentType(String filename){
  if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}


//Calculates the delay from the next measure
int GetTimeNextMeasureInSeconds() {
  int actMin = timeClient.getMinutes();
  int actSec = timeClient.getSeconds();

  for (int i=1; i < nbOfMeasures; i++) {
    if (actMin < timeOfMeasureInHour[i]) {
      return 60 * (timeOfMeasureInHour[i] - actMin) - actSec;
    }
  }
  return 60 * (60 - actMin) - actSec;
}

String getTime() {
   time_t rawtime = timeClient.getEpochTime();
   struct tm * ti;
   ti = localtime (&rawtime);

   uint16_t year = ti->tm_year + 1900;
   String yearStr = String(year);

   uint8_t month = ti->tm_mon + 1;
   String monthStr = month < 10 ? "0" + String(month) : String(month);

   uint8_t day = ti->tm_mday;
   String dayStr = day < 10 ? "0" + String(day) : String(day);

   uint8_t hours = ti->tm_hour;
   String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

   uint8_t minutes = ti->tm_min;
   String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

   uint8_t seconds = ti->tm_sec;
   String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

   return dayStr + "-" + monthStr + "-" + yearStr + " " +
          hoursStr + ":" + minuteStr + ":" + secondStr;
}
