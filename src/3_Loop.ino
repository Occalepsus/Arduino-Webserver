void loop() {
  server.handleClient();
  MDNS.update();

  if (millis() - timeOfLastRequest >= durationUntilNextMeasure){
    if (temperatureRequested){
      Serial.println("Error, rebooting");
      ESP.reset();
    }
    
    timeClient.update();

    timeOfLastRequest = millis();
    durationUntilNextMeasure = GetTimeNextMeasureInSeconds() * 1000;

    Serial.println("Requesting temperature at " + getTime());
    thSensor.requestTemperatures();
    temperatureRequested = true;

    if (timeClient.getHours() == 0 && timeClient.getMinutes() == 0) {
      WriteMaximumMinimum(actualDirectory);
      actualDirectory = GetDirectoryName();
    }
  }
  else if (temperatureRequested && millis() - timeOfLastRequest >= measureDuration) {
    Serial.println("Reading temperature at " + getTime());

    temperatureRequested = false;
    temperature = thSensor.getTempCByIndex(0);

    WriteTemperature(temperature);
    Serial.printf("Temperature of %f successfully written in file :", temperature);
    Serial.println(actualDirectory);
  }
}
