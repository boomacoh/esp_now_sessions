#include <Arduino.h>

#include <WiFiClientSecure.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <ArduinoJson.h>

const char* ssid = "ZipherWifi";          // your network SSID (name of wifi network)
const char* password = "Sh3r1d4nZ1ph3r";  // your network password
// uint8_t slaveAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t broadcastAddress[] = { 0x48, 0x31, 0xB7, 0x3F, 0xD5, 0xFC };  // reciever chip B ***


const char* server = "api.openweathermap.org";  // Server URL
String jsondata;                                // ???? MAYBE CHANGE
//char jsondata[30];

StaticJsonDocument<200> doc;
// Insert your SSID
constexpr char WIFI_SSID[] = "ZipherWifi";
// Structure to keep the temperature and humidity data from a DHT sensor

// Create a struct_message called myData
// Callback to have a track of sent messages
void OnSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nSend message status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sent Successfully" : "Sent Failed");
}
//function to get the current channel of wifi
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

const char* test_root_ca =
  "-----BEGIN CERTIFICATE-----\n"
  "MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB\n"
  "iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n"
  "cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n"
  "BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw\n"
  "MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV\n"
  "BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU\n"
  "aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy\n"
  "dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK\n"
  "AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B\n"
  "3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY\n"
  "tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/\n"
  "Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2\n"
  "VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT\n"
  "79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6\n"
  "c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT\n"
  "Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l\n"
  "c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee\n"
  "UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE\n"
  "Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd\n"
  "BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G\n"
  "A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF\n"
  "Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO\n"
  "VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3\n"
  "ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs\n"
  "8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR\n"
  "iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze\n"
  "Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ\n"
  "XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/\n"
  "qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB\n"
  "VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB\n"
  "L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG\n"
  "jjxDah2nGN59PRbxYvnKkKj9\n"
  "-----END CERTIFICATE-----\n";

// You can use x.509 client certificates if you want
//const char* test_client_key = "";   //to verify the client
//const char* test_client_cert = "";  //to verify the client
const int apireponseSize = 500;
char apiResponse[apireponseSize] = "{\"coord\":{\"lon\":-0.1257,\"lat\":51.5085}}";  // temp Data
int ctr = 0;

WiFiClientSecure client;

uint8_t channel = 1;

//This is the code to call the API and get response. it stores the response to the char array apiResponse
void get_weatherData() {
  client.setCACert(test_root_ca);
  //client.setCertificate(test_client_cert); // for client verification
  //client.setPrivateKey(test_client_key);	// for client verification

  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    // Make a HTTP request:

    client.println("GET https://api.openweathermap.org/data/2.5/weather?q=London&appid=d8bc72e0527be6de0235c3753007d4f5 HTTP/1.0");
    client.println("Host: api.openweathermap.org");
    client.println("Connection: close");
    client.println();

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
      // ctr++;
      apiResponse[ctr++] = c;
    }
    Serial.print("Counter: ");
    Serial.print(ctr);
    DeserializationError error = deserializeJson(doc, String(apiResponse));
    client.stop();
  }
}
//Function: checks if api response is more than the sending limit of 200. If it is divides it per batch. and send them one batch at a time.
void send_api_multiplebatches() {
  int num_batch = 0;
  int batch_ctr = 0;
  int sendingLimit = 200;
  batch_ctr = ctr / sendingLimit;

  // If there's any remaining data, account for another batch
  if (ctr % sendingLimit != 0) {
    batch_ctr++;
  }

  printf("Number of batches: %d\n", batch_ctr);
  char api_response_batch[sendingLimit];

  if (ctr > sendingLimit) {
    for (int i = 0; i < batch_ctr; i++) {
      Serial.print("Sending Batch: ");
      Serial.println(i);
      if (i == (batch_ctr - 1)) {
        for (int j = 0; j < (ctr % sendingLimit); j++) {
          api_response_batch[j] = apiResponse[j + (i * sendingLimit)];
        }
        for (int k = 0; k < (ctr % sendingLimit); k++) {
          Serial.print(api_response_batch[k]);
        }
      } else {
        for (int j = 0; j < sendingLimit; j++) {
          api_response_batch[j] = apiResponse[j + (i * sendingLimit)];
        }
        for (int k = 0; k < sendingLimit; k++) {
          Serial.print(api_response_batch[k]);
        }
      }

      Serial.println();
      Serial.println();

      uint8_t byte_message[sizeof(api_response_batch)];
      memcpy(byte_message, api_response_batch, sizeof(api_response_batch));
      esp_now_send(broadcastAddress, (uint8_t*)byte_message, sizeof(byte_message));
      memset(api_response_batch, 0, sizeof(api_response_batch));
    }
  }
}
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
void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  delay(100);
  wifi_connect();

  get_weatherData();


  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  int32_t channel = getWiFiChannel(ssid);
  Serial.print("Channel: ");
  Serial.print(channel);
  esp_wifi_set_ps(WIFI_PS_NONE);

  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // ALWAYS DO THIUS IF SENDING DATA ******
  esp_now_register_send_cb(OnSent);  // call back has to know to call back here to the master ******


  // COULD DEFINE ***peerInfo.peer_addr[] = {0x48,0x27,0xE2,0xB5,0x57,0xB4};
  // Register peer
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));  // ?????????????????????????????


  for (int ii = 0; ii < 6; ++ii) { peerInfo.peer_addr[ii] = (uint8_t)broadcastAddress[ii]; }

  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer  ??? define *mac_addr ???
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Peer added successfully.");
  send_api_multiplebatches();
}
void loop() {


}
