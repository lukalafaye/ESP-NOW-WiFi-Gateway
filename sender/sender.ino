// ----------------------------------------------------------------------------
// ESP-NOW network + WiFi gateway to the Internet
// ----------------------------------------------------------------------------
// ESP-NOW senders
// ----------------------------------------------------------------------------

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// ----------------------------------------------------------------------------
// WiFi handling
// ----------------------------------------------------------------------------

char* WIFI_AP_SSID = "SSID";
char* WIFI_AP_PASS = "PASSWORD";

void connect_esp32_wifi_network(char* ssid, char* password) {
  Serial.println("------------------------------------------------------------");
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Ok\nConnected!");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());
  Serial.print("; MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.println("------------------------------------------------------------\n");
  Serial.print("WiFi channel: ");
  Serial.println(WiFi.channel());
}

// ----------------------------------------------------------------------------
// ESP-NOW handling
// ----------------------------------------------------------------------------

constexpr uint8_t ESP_NOW_RECEIVER[] = { 0x30, 0xC6, 0xF7, 0x20, 0x65, 0x74};
esp_now_peer_info_t peerInfo;
bool channelFound = false;
uint8_t channel = 1;

void initEspNow() {
  if (esp_now_init() != ESP_OK) {
      Serial.println("ESP NOW failed to initialize");
      while (1);
  }

  memcpy(peerInfo.peer_addr, ESP_NOW_RECEIVER, 6);
  // peerInfo.ifidx   = ESP_IF_WIFI_STA;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("ESP NOW pairing failure");
      while (1);
  }
  
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  
  esp_now_register_send_cb(onDataSent);
}


// ----------------------------------------------------------------------------
// Offsets wifi channel by 1 when called
// ----------------------------------------------------------------------------
void tryNextChannel() {
  Serial.println("Changing channel from " + String(channel) + " to " + String(channel+1));
  channel = channel % 13 + 1;
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
}

// ----------------------------------------------------------------------------
// Callback function for when data is sent  
// ----------------------------------------------------------------------------
void onDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  if (!channelFound && status != ESP_NOW_SEND_SUCCESS) {
    Serial.println("Delivery Fail because channel" + String(channel) + " does not match receiver channel.");
    tryNextChannel(); // If message was not delivered, it tries on another wifi channel.
  }
  else {
    Serial.println("Delivery Successful ! Using channel : " + String(channel));
    channelFound = true;
  }
}

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(WIFI_PS_NONE);
  connect_esp32_wifi_network(WIFI_AP_SSID, WIFI_AP_PASS);
  esp_wifi_set_ps(WIFI_PS_NONE);
  WiFi.printDiag(Serial);
  initEspNow();
}

// ----------------------------------------------------------------------------
// Main control loop
// ----------------------------------------------------------------------------

uint32_t last;

void loop() {
  if (millis() - last > 2000) {
      uint8_t data = random(1, 256);
      esp_now_send(ESP_NOW_RECEIVER, (uint8_t *) &data, sizeof(uint8_t));
      Serial.printf("sent: %3u on channel: %u\n", data, WiFi.channel());
      last = millis();
  }
}
