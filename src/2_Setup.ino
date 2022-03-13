void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  thSensor.setWaitForConversion(false);      //Don't block the program while the temperature sensor is reading
  thSensor.begin();                         //Start the temperature sensor

  LittleFS.begin();                           // Start the SPI Flash Files System
  
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

//  if (!MDNS.begin(dnsName)) {             // Start the mDNS responder for esp8266.local
//    Serial.println("Error setting up MDNS responder!");
//  }
//  Serial.println("mDNS responder started");
//  Serial.println("Local URL : " + dnsName + ".local");

  //We set the NTP client
  timeClient.begin();

  //We start the server
  StartServer();

  nbOfMeasures = sizeof(timeOfMeasureInHour);

  timeClient.update();
  timeOfLastRequest = millis();
  durationUntilNextMeasure = GetTimeNextMeasureInSeconds() * 1000;

  actualDirectory = GetDirectoryName();
  Serial.println(actualDirectory);

  WriteMaximumMinimum("/data/Aou_2021/29.csv");
  WriteMaximumMinimum("/data/Aou_2021/30.csv");
  WriteMaximumMinimum("/data/Aou_2021/31.csv");
  WriteMaximumMinimum("/data/Sep_2021/1.csv");
}
