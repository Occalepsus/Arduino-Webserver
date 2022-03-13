const String months[12] = {"Jan", "Fev", "Mar", "Avr", "Mai", "Jun", "Jul", "Aou", "Sep", "Oct", "Nov", "Dec"};

String GetDirectoryName() {
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);

  uint16_t year = ti->tm_year;
  uint8_t monthIdx = ti->tm_mon;
  uint8_t day = ti->tm_mday;

  return "/data/" + months[monthIdx] + "_" + String(year + 1900) + 
         "/" + day + ".csv";
}

void WriteTemperature(int temperature){
    File tempLog = LittleFS.open(actualDirectory, "a"); // Write the time and the temperature to the csv file
    tempLog.print(getTime());
    tempLog.print(',');
    tempLog.println(temperature);
    tempLog.close();
}

void WriteMaximumMinimum(String fileDirectory) {
  File tempLog = LittleFS.open(fileDirectory, "a+");
  int max = -99;
  int min = 99;
  while (tempLog.available() > 0) {
    String tempStream = "";
    char tempChar = (char)tempLog.read();
    
    if (tempChar == '@') {                      //if we encounter the char of max/min, we do nothing
      tempLog.close();
      Serial.println("Max / Min already done, closing file...");
      return;
    }

    
    while (tempChar != ',' && tempChar != '\n') {
      tempStream += tempChar;
      tempChar = (char)tempLog.read();
    }

    
    if (tempChar == '\n') {
      int value = tempStream.toInt();
      if (value > max) {
        max = value;
      }
      if (value < min) {
        min = value;
      }
    }
  }
  tempLog.print('@');
  tempLog.close();

  File mmLog = LittleFS.open(maxminDirectory, "a");

  mmLog.print(fileDirectory.substring(15, 17));
  mmLog.print(' ');
  mmLog.print(fileDirectory.substring(6, 9));
  mmLog.print(' ');
  mmLog.print(fileDirectory.substring(10, 14));
  mmLog.print(',');
  mmLog.print(max);
  mmLog.print(',');
  mmLog.print(min);
  mmLog.print('\n');

  mmLog.close();
}
