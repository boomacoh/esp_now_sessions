#include <Arduino.h>

// RECIEVER SKETCH 2 CHIP JSON

#include <ArduinoJson.h>
#include <WiFi.h>     // the extra wifi .hs screwed us up
#include <esp_now.h>  // not ion library list but is in system when we got esp32 board
#include <config.h>
// u_int8_t
String jsondata;
StaticJsonDocument<200> doc;



void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
  char buff[len + 4];
  strcpy(buff, (const char*)incomingData);
  jsondata = String(buff);

  Serial.print("Recieved: ");
  Serial.println(jsondata);  // ****LETS SEE THE DATA STRING
  DeserializationError error = deserializeJson(doc, jsondata);
  if (error) {
    Serial.print(F("Deserialization function failed: "));  // if there is a Error of ArduinoJason
    Serial.println(error.f_str());
    return;

  }  // else error
   // Extract values
  Serial.println(F("Response:"));
  Serial.println(doc["sensor"].as<const char*>());
  Serial.println(doc["time"].as<long>());
  Serial.println(doc["data"][0].as<float>(), 6);
  Serial.println(doc["data"][1].as<float>(), 6);

}  // end dataTRecv
int32_t getWiFiChannel(const char* ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
    for (uint8_t i = 0; i < n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
        return WiFi.channel(i);
      }
    }
  }
  return 0;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  // wifi_connect();
  

  // Init ESP-NOW  USE IN BOTH SEND & RECIEVE ....................................
  if (esp_now_init() != ESP_OK) {
    Serial.println("error initializing esp-now");
    return;  // jumps out of loop
  }
  esp_now_register_recv_cb(OnDataRecv);  
  int32_t channel = getWiFiChannel(ssid);
  Serial.print("Channel: ");
  Serial.print(channel);


}  // end of setup

void loop() {
}
