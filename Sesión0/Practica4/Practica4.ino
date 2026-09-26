#include <WiFi.h>

#include <WebServer.h>

#include <ArduinoOTA.h>

void setupOTA() {
  ArduinoOTA.setHostname("esp32-parella-07");     // <-- o voso número
  ArduinoOTA.setPassword("hola");

  ArduinoOTA.onStart   ([]{ Serial.println("\n[ota] comeza"); });
  ArduinoOTA.onProgress([](unsigned int f, unsigned int t) {
      Serial.printf("[ota] %u%%\r", f * 100 / t); });
  ArduinoOTA.onEnd     ([]{ Serial.println("\n[ota] feito, reiniciando"); });
  ArduinoOTA.onError   ([](ota_error_t e){ Serial.printf("[ota] erro %u\n", e); });

  ArduinoOTA.begin();
}

WebServer server(80);
const int LED = 2;


String paxina() {
  char buf[420];
  snprintf(buf, sizeof(buf),
    "<!DOCTYPE html><meta name='viewport' content='width=device-width'>"
    "<h2>ESP32 &mdash; parella Pedro &</h2>"
    "<p>LED: <b>%s</b></p>"
    "<p><a href='/on'>ACENDER</a> &nbsp;|&nbsp; <a href='/off'>APAGAR</a></p>"
    "<hr><p>RSSI %d dBm &middot; uptime %lu s &middot; heap %u B</p>",
    digitalRead(LED) ? "aceso" : "apagado",
    WiFi.RSSI(), millis() / 1000, ESP.getFreeHeap());
  return String(buf);
}

const char* SSID = "Se-josepedro";      // <-- o voso
const char* PASS = "pedrosav";      // <-- o voso

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

  pinMode(LED, OUTPUT);
  server.on("/",    []{ server.send(200, "text/html", paxina()); });
  server.on("/on",  []{ digitalWrite(LED, HIGH); server.sendHeader("Location", "/"); server.send(303); });
  server.on("/off", []{ digitalWrite(LED, LOW);  server.sendHeader("Location", "/"); server.send(303); });
  server.begin();
  Serial.println("Servidor web en marcha");
  setupOTA();
}



void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  static uint32_t t = 0;
  if (millis() - t >= 5000) {
    t = millis();
    Serial.printf("estado=%d  RSSI=%d dBm\n", WiFi.status(), WiFi.RSSI());
  }
}
