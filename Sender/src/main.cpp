#include <Arduino.h>


#include <config.h>
#include <WiFiClient.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
String jsondata;
JsonDocument doc;
void On_Sent(const uint8_t* mac_addr, esp_now_send_status_t status) {

  Serial.print("\r\nSend message status:\t");

  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sent Successfully" : "Sent Failed");

}


WiFiClient client;


//function to connect to wifi.
void wifi_connect() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);
}
void readJsonFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    String fileContents;
    while (file.available()) {
        fileContents += (char)file.read();
    }

    // Close the file
    file.close();

    // Parse JSON data
    // DynamicJsonDocument doc(1024); // Adjust the size accordingly
    DeserializationError error = deserializeJson(doc, fileContents);
    if (error) {
        Serial.print("Failed to parse JSON file: ");
        Serial.println(error.c_str());
        return;
    }

     Serial.println(F("Response:"));
  Serial.println(doc["sensor"].as<const char*>());
  Serial.println(doc["time"].as<long>());
  Serial.println(doc["data"][0].as<float>(), 6);
  Serial.println(doc["data"][1].as<float>(), 6);
}

void writeJsonFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}
void get_api(){
  client.setTimeout(10000);
  if (!client.connect(server, port)) {
    Serial.println(F("Connection failed"));
    return;
  }

  Serial.println(F("Connected!"));
  client.println(F("GET /example.json HTTP/1.0"));
  client.println(F("Host: arduinojson.org"));
  client.println(F("Connection: close"));
  if (client.println() == 0) {
    Serial.println(F("Failed to send request"));
    client.stop();
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    client.stop();
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    client.stop();
    return;
  }
  
  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    client.stop();
    return;
  }

  // Extract values
  Serial.println(F("Response:"));
  Serial.println(doc["sensor"].as<const char*>());
  Serial.println(doc["time"].as<long>());
  Serial.println(doc["data"][0].as<float>(), 6);
  Serial.println(doc["data"][1].as<float>(), 6);

  // Disconnect
  client.stop();
}

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  delay(100);
  if (!LittleFS.begin(true)) {
        Serial.println("Failed to mount LittleFS");
        return;
    }
  readJsonFile(LittleFS, "/data.json");
  Serial.print("Doc size: ");
  Serial.println(doc.size());
  if (doc.size() == 0) {
  wifi_connect();
  get_api();
  String jsonString;
    serializeJson(doc, jsonString);

    Serial.print("Saving: ");
    Serial.println(jsonString);
    writeJsonFile(LittleFS, "/data.json", jsonString.c_str());
    ESP.restart();
  }

    
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(On_Sent); 

  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));  


  for (int ii = 0; ii < 6; ++ii) { peerInfo.peer_addr[ii] = (uint8_t)broadcastAddress[ii]; }

  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer  ??? define *mac_addr ???
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Peer added successfully.");
}
void send_espnow() {

  String jsonString;
  serializeJson(doc, jsonString);
  esp_now_send(broadcastAddress, (uint8_t*)jsonString.c_str(), strlen(jsonString.c_str()));


}
void loop() {
  send_espnow();
  delay(5000);


}



