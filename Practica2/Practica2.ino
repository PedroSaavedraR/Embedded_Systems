#include <WiFi.h>

const char* SSID = "HW";      // <-- o voso
const char* PASS = "1q2w3e4r";      // <-- o voso

void onWiFiEvent(arduino_event_id_t e, arduino_event_info_t info) {
  switch (e) {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.printf("[%6lu ms] asociado ao AP\n", millis());
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.printf("[%6lu ms] IP %s   gateway %s   RSSI %d dBm\n", millis(),
                    WiFi.localIP().toString().c_str(),
                    WiFi.gatewayIP().toString().c_str(), WiFi.RSSI());
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.printf("[%6lu ms] DESCONECTADO\n", millis());
      break;
    default: break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);
  WiFi.onEvent(onWiFiEvent);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(SSID, PASS);
}

void loop() {
  static uint32_t t = 0;
  if (millis() - t >= 5000) {
    t = millis();
    Serial.printf("estado=%d  RSSI=%d dBm\n", WiFi.status(), WiFi.RSSI());
  }
}