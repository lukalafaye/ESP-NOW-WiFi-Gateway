// ----------------------------------------------------------------------------
// ESP-NOW network + WiFi gateway to the Internet
// ----------------------------------------------------------------------------
// ESP-NOW receiver acting as a WiFi gateway to the Internet
// ----------------------------------------------------------------------------

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// ----------------------------------------------------------------------------
// WiFi handling
// ----------------------------------------------------------------------------

char* WIFI_AP_SSID = "SSIDs";
char* WIFI_AP_PASS = "PASSWORD";

char* WIFI_SSID = "SSID";
char* WIFI_PASS = "PASSWORD";

void initWiFi() {
  WiFi.mode(WIFI_MODE_APSTA);
  esp_wifi_set_ps(WIFI_PS_NONE); // Otherwise wifi card goes to sleep if inactive
  connect_esp32_wifi_network(WIFI_SSID, WIFI_PASS);
  WiFi.printDiag(Serial);
  WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS); // Start local access point
}

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

void onReceive(const uint8_t *mac_addr, const uint8_t *data, int len) {
  Serial.printf("received: %3u from %02x:%02x:%02x:%02x:%02x:%02x\n",
    (uint8_t) *data,
    mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]
  );
} // You can use boolean variables to trigger wifi-related events in the loop function following onReceive() call.

void initEspNow() {
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP NOW failed to initialize");
        while (1);
    }
    esp_now_register_recv_cb(onReceive);
}

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

void setup() {
    Serial.begin(115200);
    delay(500);
    initWiFi();
    initEspNow();
}

// ----------------------------------------------------------------------------
// Main control loop
// ----------------------------------------------------------------------------

void loop() {} 
// All intructions related to WiFi gateway should be executed here or in setup()
// It is not possible to execute wifi-related instructions in the callback function since it needs to use the built-in wifi card until it returns.

