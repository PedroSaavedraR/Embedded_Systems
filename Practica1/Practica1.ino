const int LED = 2;

void setup()
{
  Serial.begin(115200);
  delay(300);
  pinMode(LED, OUTPUT);
  Serial.println("Listo. Comandos: on | off | mem | uptime");
}

void loop()
{
  if (Serial.available())
  {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if      (cmd == "on")     { digitalWrite(LED, HIGH); Serial.println("LED aceso"); }
    else if (cmd == "off")    { digitalWrite(LED, LOW);  Serial.println("LED apagado"); }
    else if (cmd == "mem")    Serial.printf("heap: %u B\n", ESP.getFreeHeap());
    else if (cmd == "uptime") Serial.printf("uptime: %lu s\n", millis() / 1000);
    else if (cmd.length())    Serial.println("? on | off | mem | uptime");
  }
}