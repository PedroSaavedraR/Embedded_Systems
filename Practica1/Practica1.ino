const int LED=2; //LEDdaDevKitC

void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
}

void loop() {
  digitalWrite(LED, HIGH); delay(500);
  digitalWrite(LED, LOW); delay(500);
  Serial.println("blink");
}