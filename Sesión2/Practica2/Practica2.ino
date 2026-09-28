const int LDR = 34;
const int LED = 2;

const int ESCURO = 500;      //TODO: axustade estes dous valores
const int CLARO  = 800;

bool aceso = false;


void setup()
{
  Serial.begin(115200);

  pinMode(LED, OUTPUT);

}

void loop()
{
  int read = analogRead(LDR);
  int readMv = analogReadMilliVolts(LDR);


  Serial.print("Lectura en bruto: ");
  Serial.print(read);
  Serial.print(" | Milivoltios: ");
  Serial.print(readMv);
  Serial.println(" mV");


  //Serial.printf ("%d, %d\n", read, readMv);
  static bool aceso=false;

  int v = analogRead(LDR);

  if (!aceso&&v< ESCURO)
    aceso=true;
  else if( aceso&&v>CLARO )aceso=false;
    digitalWrite(LED, aceso);
  delay(20);


  delay(100);
}