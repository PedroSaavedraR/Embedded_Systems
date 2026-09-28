const int SENSOR = 27;

int      eventos      = 0;
bool     anterior     = LOW;
uint32_t ultimoCambio = 0;

const uint32_t ANTIRREBOTE = 50;    // ms

void setup()
{
  Serial.begin(115200);
  pinMode(SENSOR, INPUT);
}

void loop()
{
  //TODO: ler o estado actual
  int read = digitalRead(SENSOR);

  //TODO: se cambiou respecto a 'anterior' E xa pasou ANTIRREBOTE dende
  //      'ultimoCambio': actualizar os dous, e se o novo estado detecta,
  //      incrementar 'eventos' e imprimilo
  if (read != anterior && millis()- ultimoCambio > 50) {
    ultimoCambio = millis();
    anterior = read;
    if (read == LOW){
      eventos++;
      Serial.println("Evento detectado");
      }
  }
}