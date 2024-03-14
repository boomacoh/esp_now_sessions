#include <Arduino.h>

// RECIEVER SKETCH 2 CHIP JSON

#include <ArduinoJson.h>
#include <WiFi.h>     // the extra wifi .hs screwed us up
#include <esp_now.h>  // not ion library list but is in system when we got esp32 board

//memst

////Slave  ESP Board MAC Address:  48:27:E2:B5:57:B4   0x48,0x27,0xE2,0xB5,0x57,0xB4
// Master  STA MAC: 68:67:25:07:0C:A0       0x68,0x67,0x25,0x07,0x0C,0xA0

// CREATE VARS **********************************8
// u_int8_t
String jsondata;
StaticJsonDocument<200> doc;

const char* ssid = "ZipherWifi";          // your network SSID (name of wifi network)
const char* password = "Sh3r1d4nZ1ph3r";  // your network password


// THIS IS THE RECIVE *** OPPOSIT OF CALL BACK  *mac IS THE ADDRESS OF THE SENDER
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
  char buff[len + 4];
  strcpy(buff, (const char*)incomingData);
  jsondata = String(buff);

  Serial.print("Recieved: ");
  Serial.println(jsondata);  // ****LETS SEE THE DATA STRING
  DeserializationError error = deserializeJson(doc, jsondata);
  if (!error) {
    Serial.println("There was no error");
  }

  else {
    Serial.print(F("deserialization fiunction failed: "));  // if there is a Error of ArduinoJason
    Serial.println(error.f_str());
    return;

  }  // else error

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  wifi_connect();

  // Init ESP-NOW  USE IN BOTH SEND & RECIEVE ....................................
  if (esp_now_init() != ESP_OK) {
    Serial.println("error initializing esp-now");
    return;  // jumps out of loop
  }
  esp_now_register_recv_cb(OnDataRecv);  // DATA RECIEVED ............................ defined before SETUP
  // esp_wifi_set_channel(5, WIFI_SECOND_CHAN_NONE);
  int32_t channel = getWiFiChannel(ssid);
  Serial.print("Channel: ");
  Serial.print(channel);


}  // end of setup

void loop() {
}
