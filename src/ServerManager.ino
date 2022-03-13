void StartServer() {
  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");

  server.on("/", handleRoot);               // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/home", handleRoot);
  server.on("/charts", []() {
    sendFileToClient("/pages/charts.html");
  });
  server.onNotFound([]() {                              // If the client requests any URI
    if (!sendFileToClient(server.uri()))                  // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });
}


void handleRoot() {
  sendFileToClient("/pages/home.html");
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

String getContentType(String fileName){
  if(fileName.endsWith(".html")) return "text/html";
  else if(fileName.endsWith(".css")) return "text/css";
  else if(fileName.endsWith(".js")) return "application/javascript";
  else if(fileName.endsWith(".ico")) return "image/x-icon";
  else if(fileName.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool sendFileToClient(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (LittleFS.exists(pathWithGz) || LittleFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (LittleFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = LittleFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}
