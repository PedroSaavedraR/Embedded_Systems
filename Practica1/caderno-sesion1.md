# Caderno de prácticas Sesión 1

## Comunicacións no ESP32: serie, WiFi e OTA


**Parella:** 

**Alumnos:** Pedro Saavedra Rubinos  e  José Martínez Estévez

---

## Como se usa este caderno

1. **Cubrídeo mentres traballades**.
2. Nos bloques **Discutide** hai que escribir unha resposta breve **consensuada**.
   Se non vos poñedes de acordo, escribide as dúas posturas: iso tamén vale.
3. Ides **rotar roles** en cada práctica:
   - **Piloto**: teclado, IDE e placa.
   - **Navegante**: le os pasos en voz alta, anota no caderno e cronometra.

   Marcade quen fai que en cada práctica. O navegante *non* toca o teclado:
   se ve un erro, dío.
4. **Entrega**: este mesmo ficheiro cuberto, **un por parella**, na tarefa de
   Moodle. Podedes pegar tamén capturas da consola serie onde vos pareza.

### Punto de partida

Dáse por feito o do seminario do Arduino IDE: o IDE instalado co core **esp32 de
Espressif**, a placa recoñecida no menú *Port* e o Blink funcionando. Se algo
diso non vai, resólvedeo antes de seguir.

Dúas cousas específicas de hoxe:

- *Tools* -> *Partition Scheme* -> **Default 4MB with spiffs**. Fai falla para a
  práctica 4: é un dos esquemas **con OTA**.
- Un **móbil con datos** na parella, para abrir un punto de acceso.

---

## Práctica 1: A consola serie

**Piloto:** ______________  **Navegante:** ______________

Obxectivo: comunicarse coa placa a través do monitor serie.
Isto é moi habitual como ferramenta de depuración.

```cpp
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
```

Fixádevos nas funcións que son parte da biblioteca do Arduino/ESP32 ou doutras:
pinMode(), digitalWrite(), millis(), Serial.*.
Aínda que non tedes unha referencia delas, é sinxelo adiviñar o que fan, non?
Se non o tedes claro, preguntade!

- [ ] Os catro comandos funcionan desde o monitor serie.

Heap libre que devolve `mem`: ______________ bytes

**Discutide:** o chip ten 520 KB de SRAM. Por que o voso sketch ve bastante
menos? A onde foi o resto?

**R:** _______________________________________________________________________

**Experimento 1.1. O final de liña.** No desplegable do monitor serie, cambiade
de **"Nova liña"** a **"Sen axuste de liña"** e escribide `on`.
Probade tamén a escribilo varias veces seguidas.

Que pasa? **R:** _________________________________________________________

Por que? **R:** ___________________________________________________________

**Experimento 1.2. Os baudios.** Cambiade o monitor a **9600** (sen tocar o
código) e mirade a saída.

Que vedes? **R:** ________________________________________________________

**Experimento 1.3. O bloqueo.** Engadide ao final do `loop()`:

```cpp
  Serial.println("tic");
  delay(2000);
```

- [ ] Recompilade e probade a escribir `on` varias veces seguidas.

**Discutide:** `readStringUntil` espera ata **1 segundo** a que chegue o final de
liña, e o `delay(2000)` para todo outros dous. Cantos comandos vos comeu? Que
problema tería isto nun aparello que ademais ten que atender a rede?

**R:** _______________________________________________________________________

_______________________________________________________________________________

_______________________________________________________________________________

> **Quitade o `delay(2000)`** antes de seguir.

---

## Práctica 2: WiFi

**Piloto:** ______________  **Navegante:** ______________

Obxectivo: meter a placa na rede. Non usaremos a WiFi da facultade.
Montade un punto de acceso co móbil, e así vedes todas as pezas.

### 2.1 Abride o punto de acceso

Configuración do móbil (*Zona WiFi* / *Compartir internet* / *Hotspot*):

- **Banda: 2,4 GHz.** É o punto crítico. O ESP32 **non ve** as redes de 5 GHz.
  - *Android*: na configuración da zona WiFi, "Banda do AP" -> 2,4 GHz.
  - *iPhone*: activade **"Maximizar compatibilidade"**, que é o mesmo.
- **Contrasinal: mínimo 8 caracteres**, WPA2.
- Poñédelle un nome que vos identifique, p. ex. `AP-parella-07`.

|                 | Valor |
|---|---|
| SSID            |       |
| Contrasinal     |       |
| Modelo de móbil |       |

### 2.2 Que ve a placa

```cpp
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(300);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.println("Buscando redes...");
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; i++) {
    Serial.printf("%2d  %-26s  canle %2d  %4d dBm  %s\n", i,
                  WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i),
                  WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "aberta" : "cifrada");
  }
}

void loop() {}
```

- [ ] O voso AP aparece na lista.

Canle: ______   RSSI: ______ dBm   Redes totais atopadas: ______

> **Se o voso AP non aparece**, está en 5 GHz. Volvede ao paso 2.1. Se aparece e
> desaparece, o móbil apaga a zona WiFi cando non hai clientes: volvede a
> activala xusto antes de gravar o sketch seguinte.

**Discutide:** cantas das redes da lista están na mesma canle? Que lle pasa ao
caudal cando varias redes comparten canle?

**R:** _______________________________________________________________________

### 2.3 Conectar (e ver como se cae)

```cpp
#include <WiFi.h>

const char* SSID = "";      // <-- o voso
const char* PASS = "";      // <-- o voso

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
```

- [ ] A placa conecta e imprime a IP.

| Dato | Valor |
|---|---|
| IP da placa | ___ . ___ . ___ . ___ |
| Gateway (o voso móbil) | ___ . ___ . ___ . ___ |
| Tempo desde o arranque ata a IP | ______ ms |
| RSSI ao lado do móbil | ______ dBm |

**Experimento 2.4. A cobertura.** O navegante colle o móbil e afástase. O
piloto le o RSSI na consola.

| Distancia                   | RSSI (dBm) | Segue conectada? |
|---|---|---|
| Ao lado (< 1 m)             |            |                  |
| 5 m, mesma sala             |            |                  |
| Fóra da sala, porta pechada |            |                  |

A que distancia se cortou? ______________

**Experimento 2.5. A caída.** Apagade a zona WiFi do móbil, contade 30
segundos, e volvédea acender. **Non toquedes a placa.**

|                            | Tempo (ms) |
|---|---|
| Instante do `DESCONECTADO` |            |
| Instante da nova IP        |            |
| **Tardou en recuperarse**  |            |

Recuperou a **mesma** IP? ______

**Discutide:** ninguén programou esa reconexión no `loop()`. Quen a fixo? E que
pasaría se o código fose o típico `while (WiFi.status() != WL_CONNECTED) {}` do
`setup()`?

**R:** _______________________________________________________________________

_______________________________________________________________________________

_______________________________________________________________________________

---

## Práctica 3. O ESP32 como servidor web

**Piloto:** ______________  **Navegante:** ______________

Obxectivo: que o móbil controle a placa desde o navegador. Partimos do sketch da
práctica 2 e **engadímoslle** cousas.

**1.** Engadide arriba, xunto ao `#include <WiFi.h>`:

```cpp
#include <WebServer.h>
WebServer server(80);
const int LED = 2;

String paxina() {
  char buf[420];
  snprintf(buf, sizeof(buf),
    "<!DOCTYPE html><meta name='viewport' content='width=device-width'>"
    "<h2>ESP32 &mdash; parella ___</h2>"
    "<p>LED: <b>%s</b></p>"
    "<p><a href='/on'>ACENDER</a> &nbsp;|&nbsp; <a href='/off'>APAGAR</a></p>"
    "<hr><p>RSSI %d dBm &middot; uptime %lu s &middot; heap %u B</p>",
    digitalRead(LED) ? "aceso" : "apagado",
    WiFi.RSSI(), millis() / 1000, ESP.getFreeHeap());
  return String(buf);
}
```

**2.** Ao final do `setup()`:

```cpp
  pinMode(LED, OUTPUT);
  server.on("/",    []{ server.send(200, "text/html", paxina()); });
  server.on("/on",  []{ digitalWrite(LED, HIGH); server.sendHeader("Location", "/"); server.send(303); });
  server.on("/off", []{ digitalWrite(LED, LOW);  server.sendHeader("Location", "/"); server.send(303); });
  server.begin();
  Serial.println("Servidor web en marcha");
```

**3.** Ao principio do `loop()`: `server.handleClient();`

- [ ] Desde o **navegador do móbil**, abride `http://` + a IP da práctica 2.
- [ ] O LED acende e apaga desde a páxina.
- [ ] Recargade a páxina: o *uptime* sobe.

Funcionou á primeira? Se non, que fallaba? **R:** __________________________

_______________________________________________________________________________

**Experimento 3.1: o portátil tamén.** Conectade o **portátil** á mesma zona
WiFi do móbil e abride a mesma IP.

- [ ] Funciona desde o portátil.
- [ ] Non funciona -> probade a apagar "illamento de clientes" na zona WiFi, ou
      seguide co móbil (o punto de acceso **si** fala cos seus clientes).

**Experimento 3.2: dous á vez.** Abride a páxina no móbil e no portátil e
premede ACENDER nun e APAGAR no outro, rápido.

Que vedes? **R:** ________________________________________________________

**Discutide:** o `loop()` chama a `server.handleClient()` unha vez por volta.
Que pasaría se o `loop()` tardase 2 segundos en dar a volta (como na práctica
1.3)? E que relación ten isto co `delay()` que quitastes?

**R:** _______________________________________________________________________

_______________________________________________________________________________

> **Para ir máis alá** (no anexo de referencia, `anexo-comunicacions.pdf`): como
> viaxa realmente unha orde ata a placa, como probala con `curl` sen navegador,
> as cinco formas de mandarlle parámetros e, sobre todo, como escribir o HTML e
> o CSS **sen metelos nun `snprintf`**.

---

## Práctica 4. OTA: actualizar sen cable

**Piloto:** ______________  **Navegante:** ______________

Obxectivo: cambiar o programa da placa **sen tocar o cable USB**. É a práctica
que máis satisfacción dá e a que máis cousas require que estean ben.

> Requisitos: *Partition Scheme* **con OTA** (o `Default 4MB with spiffs` vale),
> e o **portátil na mesma rede que a placa**, é dicir, conectado á zona WiFi do
> móbil. Sen iso, o IDE non a atopa.

**1.** Ao sketch da práctica 3 engadídelle:

```cpp
#include <ArduinoOTA.h>

void setupOTA() {
  ArduinoOTA.setHostname("esp32-parella-07");     // <-- o voso número
  ArduinoOTA.setPassword("udc");

  ArduinoOTA.onStart   ([]{ Serial.println("\n[ota] comeza"); });
  ArduinoOTA.onProgress([](unsigned int f, unsigned int t) {
      Serial.printf("[ota] %u%%\r", f * 100 / t); });
  ArduinoOTA.onEnd     ([]{ Serial.println("\n[ota] feito, reiniciando"); });
  ArduinoOTA.onError   ([](ota_error_t e){ Serial.printf("[ota] erro %u\n", e); });

  ArduinoOTA.begin();
}
```

**2.** Chamade a `setupOTA();` ao final do `setup()`, e engadide
`ArduinoOTA.handle();` ao principio do `loop()`.

**3.** Gravade **por cable** esta versión. É a última vez que usades o cable
para gravar.

- [ ] *Tools* -> *Port* -> aparece un **porto de rede** co voso `hostname`.

Como aparece exactamente? **R:** ____________________________________________

**4.** Cambiade algo visible (por exemplo, o texto `parella ___` da páxina web,
ou facede que o LED parpadee) e dádelle a *Upload* **co porto de rede
seleccionado**. Pediravos o contrasinal.

- [ ] Actualizouse sen cable.

Canto tardou a subida? ______ s   Que pasou co servidor web mentres tanto? ______

**Experimento 4.1: o contrasinal.** Cambiade `setPassword` no IDE... ou mellor:
intentade subir escribindo un contrasinal **mal**.

Que erro dá? **R:** _______________________________________________________

**Experimento 4.2: a partición.** *Tools* -> *Partition Scheme* ->
**Huge APP (3MB No OTA/1MB SPIFFS)**, e intentade subir por rede.

Que pasa, e por que? **R:** ________________________________________________

_______________________________________________________________________________

> Volvede a `Default 4MB with spiffs` (e gravade por cable) antes de seguir.

**Discutide:** mentres se descarga o firmware novo, o vello segue executándose.
Onde se está escribindo o novo? E se apagásemos a placa xusto no 70 % da
descarga, que arrancaría ao volver a acendela?

**R:** _______________________________________________________________________

_______________________________________________________________________________

---

## Ampliación A: Bluetooth (opcional)

Obxectivo: a mesma consola da práctica 1, pero sen cable e desde o móbil.

> **Sketch aparte, non o de antes.** WiFi + servidor web + Bluetooth Classic non
> collen nos 1,2 MB de app que deixa o esquema `Default 4MB`. Se insistides,
> veredes o erro `Sketch too big`: é a lección da slide das particións en vivo.

```cpp
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;
const int LED = 2;

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  SerialBT.begin("ESP32-parella-07");          // <-- o voso número
  SerialBT.println("Comandos: on | off");
}

void loop() {
  if (SerialBT.available()) {
    String cmd = SerialBT.readStringUntil('\n');
    cmd.trim();
    if      (cmd == "on")  { digitalWrite(LED, HIGH); SerialBT.println("aceso"); }
    else if (cmd == "off") { digitalWrite(LED, LOW);  SerialBT.println("apagado"); }
  }
}
```

No móbil fai falla unha app de **terminal Bluetooth** (*Serial Bluetooth
Terminal* ou similar): emparellade primeiro desde os axustes do móbil.

- [ ] Funciona.  Emparellouse á primeira? ______

**Discutide:** o código é **o mesmo** que o da práctica 1 cambiando `Serial` por
`SerialBT`. Que quere dicir iso sobre a abstracción que ofrece Arduino? E que
vos oculta (consumo, alcance, emparellamento)?

**R:** _______________________________________________________________________

_______________________________________________________________________________

> Se tedes iPhone: non vos vai valer. iOS non fala Bluetooth Classic SPP, só
> BLE. Anotádeo como resultado, que tamén é información.

---

## Ampliación B: Dúas placas falando (xuntádevos con outra parella)

Agora sodes catro persoas e **dúas placas**. Decidide cal é a **placa A** e cal
a **placa B**.

Parella A: ____________________   Parella B: ____________________

### B.1: Unha placa fai de punto de acceso

Xa non fai falla o móbil: o ESP32 pode ser el o AP.

**Placa A** (servidor, o AP):

```cpp
#include <WiFi.h>
#include <WebServer.h>
WebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("esp32-A", "12345678");
  Serial.println(WiFi.softAPIP());                  // 192.168.4.1
  server.on("/", []{ server.send(200, "text/plain", "ola desde a placa A"); });
  server.on("/temp", []{ server.send(200, "text/plain", String(millis() / 1000)); });
  server.begin();
}
void loop() { server.handleClient(); }
```

**Placa B** (cliente, estación):

```cpp
#include <WiFi.h>
#include <HTTPClient.h>

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin("esp32-A", "12345678");
  while (WiFi.status() != WL_CONNECTED) { delay(250); Serial.print('.'); }
  Serial.printf("\nconectado, IP %s\n", WiFi.localIP().toString().c_str());
}

void loop() {
  HTTPClient http;
  http.begin("http://192.168.4.1/temp");
  int code = http.GET();
  Serial.printf("GET -> %d : %s\n", code, http.getString().c_str());
  http.end();
  delay(3000);
}
```

- [ ] A placa B le da placa A.

IP que lle deu A a B: ___ . ___ . ___ . ___

**Discutide:** cantos dos aparellos que usades a diario fan exactamente isto
(un cacharro que crea a súa propia rede para que outro se conecte)?

**R:** _______________________________________________________________________

### B.2: Sen radio: un cable entre as dúas placas

Tres cables *dupont* entre as placas:

```
  placa A            placa B
  GPIO17 (TX) ------ GPIO16 (RX)
  GPIO16 (RX) ------ GPIO17 (TX)
  GND         ------ GND
```

As dúas placas cargan **o mesmo** sketch (cambiade só o nome):

```cpp
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);   // RX=16, TX=17
}

void loop() {
  static uint32_t t = 0;
  if (millis() - t >= 2000) {
    t = millis();
    Serial2.printf("ola da placa A, t=%lu\n", t / 1000);   // <-- A ou B
  }
  while (Serial2.available()) Serial.write(Serial2.read());
}
```

- [ ] Cada consola amosa as mensaxes da **outra** placa.

**Probade a estragalo:**

| Cambio | Que pasa |
|---|---|
| Quitar o cable de GND | |
| Poñer TX con TX e RX con RX | |
| Poñer 115200 nunha placa e 9600 na outra | |

**Discutide:** por que fai falla o cable de masa se os datos van por outro fío?

**R:** _______________________________________________________________________

_______________________________________________________________________________

---

## Práctica Libre

Con todo isto, pensade nalgunha cousa interesante que se poda facer.
Pode ser algo útil, algo divertido, un xogo, etc.
Canto máis orixinal, mellor.

### Algunhas ideas

1. **Frío-caliente por RSSI** O LED parpadea máis rápido canto máis forte é o RSSI. Pódese usar para atopar o móbil (AP) agochado na aula.

2. **Cronómetro de reflexos**: A páxina web acende o LED nun instante aleatorio, hay que enviar un sinal a través dun dos pins o antes posible. Para isto fai falta un cable cunha resistencia pull-up ou pull-down.

3. **O SSID como canle de difusión**: O nome do AP cambia cada 10 segundos e vai producindo una frase visible para todos. Idea relacionada: rogue APs.

4. **Portal cautivo**: AP + DNSServer respondendo a todas as consultas coa IP da placa: é coma as WiFis de moitos hoteis.

5. **Chat de aula**: Con dúas ou máis placas, cada unha serve unha páxina cunha caixa de texto e as mensaxes viaxan de placa a placa.

6. **Semáforo de estado**: Consultar periódicamente unha páxina web e indicar co LED o seu estado. Un monitor de uptime de cinco euros.

---

## Resumo Final

### Que quedou funcionando

- [ ] P1: consola de comandos
- [ ] P2: WiFi contra o AP do móbil, con reconexión
- [ ] P3: servidor web accesible desde o móbil
- [ ] P4: actualización OTA sen cable
- [ ] A: Bluetooth (opcional)
- [ ] B: dúas placas (opcional)
- [ ] Práctica libre: _________________________________________________________

Anotade o que non conseguistes e ata onde chegastes. Isto **conta igual** que o
que funcionou: a metade do traballo nun sistema embebido é diagnosticar.

_______________________________________________________________________________

_______________________________________________________________________________

_______________________________________________________________________________

Identificas algunha das técnicas vistas hoxe que poida ter sido empregada no
escape room da semana pasada?

_______________________________________________________________________________

_______________________________________________________________________________

_______________________________________________________________________________


### Entrega

Subide **este ficheiro cuberto** á tarefa de Moodle, **un por parella**. Se
fixestes a ampliación B, indicade coa outra parella que traballastes.
