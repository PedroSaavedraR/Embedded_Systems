# Caderno de prácticas Sesión 2

## Sensores co ESP32


**Parella:** ______________________________

**Alumnos:** ______________________________  e  ______________________________

---

## Como se usa este caderno

1. **Renombrade este caderno como caderno-sesion2-<parella>**.
2. Nos bloques **Discutide** hai que escribir unha resposta breve **consensuada**.
   Se non vos poñedes de acordo, escribide as dúas posturas: iso tamén vale.
3. Ides **rotar roles** en cada práctica:
   - **Piloto**: teclado, IDE e placa.
   - **Navegante**: le os pasos en voz alta, anota no caderno e cablea.

   Marcade quen fai que en cada práctica.
4. **Entrega**: este mesmo ficheiro cuberto, un por parella, na tarefa de Moodle.

### Punto de partida

Necesitades:
- O **ESP32**
- O **kit de 16 sensores** e cables dupont **femia-macho**/**macho/macho**.
- Unha **protoboard**

**Antes de conectar nada, tres regras.** Están na presentación, pero aquí van
outra vez porque son as que estragan hardware:

- As entradas do ESP32 son de **3,3 V**. Asegurádevos de non meter 5 V nun GPIO.
- **Masa común** sempre: o GND do módulo ao GND da placa.
- Todo o **analóxico** (`AO`) vai a **GPIO 32-39**.

### Que pins podedes usar

| Pins | Podedes usalos? |
|---|---|
| 16-19, 21-23, 25-27, 32-33 | **Si**, para o que queirades |
| 34-39 | Si, pero **só como entrada** (perfectos para `AO`) |
| 0, 2, 12, 15 | Mellor non: deciden como arranca a placa |
| 1, 3 | Non: son o porto serie do USB |
| 6-11 | **Nunca**: van á memoria flash |

---

## Práctica 1: Un sensor dixital

**Piloto:** Pedro Saavedra Rubinos  **Navegante:** José Martínez Estévez

Obxectivo: traballar co sensor máis simple posible, con saída binaria.
Usade o módulo de **vibración** (ou o de **inclinación**, se preferides).

Conexión:

```
  módulo          ESP32
  VCC    -------- 3V3
  GND    -------- GND
  DO     -------- GPIO27
```

Escribide un sketch que lea o `DO` do módulo, acenda o LED da placa
(`GPIO2`) segundo o que lea, e imprima o valor por Serial.

As funcións que necesitades son as mesmas que na sesión anterior:
`pinMode`, `digitalRead`, `digitalWrite` e `Serial`

```cpp
const int SENSOR = 27;
const int LED    = 2;

void setup()
{
  //TODO: iniciar o Serial e configurar os pins
}

void loop()
{
  //TODO: ler o sensor, escribir o LED e imprimir por Serial
}
```

- [ ] A consola amosa números cando movedes o módulo.

**Experimento 1.1.**

Deixade o módulo quieto e mirade o valor. Despois axitádeo.

En repouso vale: 0    Ao detectar vale: 1

Está invertido (é `LOW` cando detecta)? No

Moitos módulos deste tipo sacan `HIGH` cando detectan, e outros ao revés.
Non se debe dar por suposto que funciona dun xeito determinado sen comprobalo.

**Experimento 1.2. O umbral**

Xirade o tornillo azul pouco a pouco mentres mirades a consola.

Que fai? **R:** Aumenta la sensibilidad, se activa más veces

**Experimento 1.3. O rebote.**

Modificade o sketch para **contar eventos** (baixadas a `LOW`).
Precisades gardar o **estado anterior** e comparalo co actual: se cambiou e detecta, hai un evento novo.
Para o antirrebote, engadide ademais un **intervalo mínimo** entre eventos aceptados (probade con 50 ms)
e ignorade calquera cambio que chegue antes de que pase ese tempo.

```cpp
const int SENSOR = 27;

int      eventos      = 0;
bool     anterior     = HIGH;
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

  //TODO: se cambiou respecto a 'anterior' E xa pasou ANTIRREBOTE dende
  //      'ultimoCambio': actualizar os dous, e se o novo estado detecta,
  //      incrementar 'eventos' e imprimilo
}
```

Dádelle **cinco golpes secos** á mesa e anotade o que conta. Despois quitade
o intervalo mínimo (poñédeo a 0 ms) e repetide con outros cinco golpes:

| Intervalo mínimo | Golpes reais | Eventos contados |
|---|---|---|
| 50 ms | 5 | 5 |
| 0 ms  | 5 | 25 |

**Discutide:** de onde saen os eventos de máis?

**R:** Del muelle del propio hardware rebotando tras un golpe


---

## Práctica 2: Un sensor analóxico

**Piloto:** José Martínez Estévez  **Navegante:** Pedro Saavedra Rubinos

Obxectivo: pasar do *si/non* a un **número**. Usade o módulo de **fotorresistencia (LDR)**,
que ten tanto `DO` como `AO`. Centrarémonos en `AO`.

```
  módulo          ESP32
  VCC    -------- 3V3
  GND    -------- GND
  AO     -------- GPIO34  (ADC1, e é un pin de só entrada)
```

Escribide un sketch que lea o `AO` e o imprima por Serial, tanto en bruto
(0-4095) coma xa convertido a milivoltios.
Precisades dúas funcións novas: 
`analogRead` para ler o ADC sen máis,
e `analogReadMilliVolts` que devolve directamente mV.

```cpp
const int LDR = 34;

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  // TODO: ler o valor cru e o valor en mV, e imprimilos por Serial

  delay(100);
}
```

**Caracterizade o sensor.**

| Condición                | Valor cru (0-4095) | mV |
|---|---|---|
| Tapado co dedo           |     120               |  240  |
| Luz da aula              |      930              |  910  |
| Lanterna do móbil pegada |       3200             | 2700   |

Sube ou baixa o valor coa luz? sube

**Experimento 2.1. O Serial Plotter.** *Tools* -> *Serial Plotter*
(pechade antes o Monitor. Non poden estar os dous abertos).
Movede a man por riba do sensor.

Que vedes que non se vía na consola de texto? **R:** un gráfico con los valores

**Experimento 2.2.** Cambiade o cable do `AO` de **GPIO34 a GPIO25**, axustade o código,
e comprobade que segue funcionando.

Agora engadide isto ao principio do `setup()`:

```cpp
#include <WiFi.h>
// ...
  WiFi.begin("calquera_cousa", "12345678"); // nin sequera ten que existir
```

| Pin usado | Lecturas antes do WiFi | Lecturas co WiFi |
|---|---|---|
| GPIO25    | normal | 0, 0|
| GPIO34    | normal | normal |

**Discutide:** que pasou, e por que só nun dos dous pins?
Que consecuencia ten isto para o proxecto final?

**R:** GPIO 25 interfiere a nivel de hardware con el Wi-Fi porque comparten el bloque ADC2, mientras que GPIO 34 es seguro de usar con Wi-Fi siempre que el circuito esté bien alimentado y protegido frente a ruido.


Volvede a **GPIO34** antes de seguir.


**Experimento 2.3. Histérese.** Facede que o LED se acenda ao escurecer,
pero sen que parpadee cando a luz queda xusto no límite.
Para iso, usade **dous umbrales** en vez dun só: un para acender (por debaixo del)
e outro máis alto para apagar (por riba del).
Mentres o valor quede entre os dous, o LED mantén o estado que xa tiña.

```cpp
const int LDR = 34;
const int LED = 2;

const int ESCURO = 0;      //TODO: axustade estes dous valores
const int CLARO  = 0;

bool aceso = false;

void setup()
{
  pinMode(LED, OUTPUT);
}

void loop()
{
  // TODO: ler o LDR e actualizar 'aceso' cos dous umbrales

  digitalWrite(LED, aceso);
  delay(20);
}
```

Probade primeiro cos dous umbrales **iguais** (é dicir, un só)
e tapade o sensor moi lentamente co dedo.

Que fai o LED xusto no umbral? **R:** ______________________________________

E cos dous umbrales separados? **R:** ______________________________________

---

## Práctica 3: Un sensor con biblioteca

**Piloto:** ______________  **Navegante:** ______________

Obxectivo: un sensor que **non se le cun `analogRead()`**.
O **DHT11** manda temperatura e humidade por un só fío, cunha temporización moi estrita.

**1.** *Tools* -> *Manage Libraries* -> buscade **DHT sensor library** (de Adafruit) e instaládea.
Pediravos instalar tamén **Adafruit Unified Sensor**: dicide que si.

**2.** Conexión:

```
  módulo          ESP32
  VCC (+) -------- 3V3
  GND (-) -------- GND
  DATA(S) -------- GPIO26
```

Escribide un sketch que:

- Cree un obxecto `DHT` para este pin e tipo de sensor
  (mirade a clase `DHT` que trae a biblioteca: *File → Examples → DHT sensor library*).
- Lea temperatura e humidade **sen bloquear** e sen pedir lecturas con maior frecuencia
  do que o sensor admite (cada 2 s).
- Conte cantas lecturas fallan fronte ao total de intentos (unha lectura
  falla cando o valor devolto **non é un número**), e o mostre por Serial
  xunto coa temperatura e a humidade.

```cpp
#include <DHT.h>

#define PIN_DHT  26
#define TIPO_DHT DHT11

DHT dht(PIN_DHT, TIPO_DHT);

uint32_t ultima   = 0;
int      lecturas = 0;
int      fallos   = 0;

void setup()
{
  Serial.begin(115200);
  dht.begin();
}

void loop()
{
  // TODO: cada 2000 ms (sen bloquear con delay):
  //   - ler temperatura e humidade
  //   - actualizar 'lecturas' e 'fallos'
  //   - imprimir todo por Serial
}
```

- [ ] Saen temperatura e humidade.

Temperatura da aula: ______ °C   Humidade: ______ %

Coincide coa que dá o móbil dalgún de vós, ou coa doutra parella? ______

**Experimento 3.1. O ritmo.** Cambiade o `2000` por `200` e deixádeo correr un minuto.

| Intervalo | Lecturas | Fallos | % de fallo |
|---|---|---|---|
| 2000 ms   | | | |
| 200 ms    | | | |

**Experimento 3.2. A inercia.** Sopradelle ao sensor ou pechádeo na man, e
cronometrade.

Canto tarda en subir 2 °C? ______ s   E en volver ao valor de antes? ______ s

Cal é o salto máis pequeno que lle vedes dar? ______ °C

**Discutide:** un sensor que tarda medio minuto en reaccionar, serve para
detectar a temperatura dun motor? E para a dunha aula? Por que credes que
**falla algunha lectura**, se o cable non se move?

**R:** _______________________________________________________________________

_______________________________________________________________________________

_______________________________________________________________________________

Se vos falla **máis da metade das veces**, probade a biblioteca *DHT sensor library for ESPx*,
que leva mellor as interrupcións do ESP32.

---

## Práctica 4: Ultrasóns

**Piloto:** ______________  **Navegante:** ______________

O **HC-SR04** mide distancia cronometrando un eco.

**Alimentación.** Este módulo adoita pedir 5 V, e daquela o pin `ECHO` saca 5 V.
**Non se poden meter directamente nun GPIO**.
Alternativas:
  1. Alimentalo a **3V3**: moitos funcionan, con menos alcance.
  2. Alimentalo a 5 V e poñer un **divisor de tensión** no `ECHO`
     (1 k ohm e 2 k ohm).

```
  módulo          ESP32
  VCC    -------- 3V3 (ou 5V, con divisor no ECHO)
  TRIG   -------- GPIO18
  ECHO   -------- GPIO19
  GND    -------- GND
```

Escribide unha función `medir()` que dispare un pulso curto en `TRIG` (uns 10 µs abondan)
e mida canto tarda en volver o eco en `ECHO`.
A función que precisades para iso é nova, `pulseIn()`:
  mirade que devolve e que parámetro de *timeout* acepta (útil para non quedar esperando un eco que non chega).
A partir do tempo medido, e sabendo a velocidade do son, **obtede a distancia en cm**.

Tomade tres medidas seguidas e quedade coa **mediana** das tres
(non a media: unha soa lectura estragada arrastra a media, pero non a mediana).
Imprimide por Serial tanto o valor cru coma o filtrado.

```cpp
const int TRIG = 18;
const int ECHO = 19;

long medir()
{
  // TODO: xerar o pulso de disparo en TRIG:
  //       1. Poñer TRIG a low por se acaso, e activalo
  //       2. A función delayMicroseconds(10) permite esperar os 10 ms de pulso
  //       3. Desactivar TRIG para finalizar o pulso 

  // TODO: medir a duración do eco con pulseIn(pin, valor, timeout) e convertela a cm
  //       1. Mediremos no pin ECHO, o valor HIGH. Un timeout de 30ms é suficiente para medir 5m
  //       (devolver -1 se non houbo eco)
}

int mediana3(int a, int b, int c)
{
  // TODO: devolver o valor central dos tres
}

void setup()
{
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop()
{
  long a = medir(); delay(60);
  long b = medir(); delay(60);
  long c = medir();

  Serial.printf("%ld,%ld\n", a, mediana3(a, b, c));   // cru e filtrado
  delay(200);
}
```

Medide cunha regra ou cunha cinta métrica e comparade:

| Distancia real | Medida (cru) | Medida (mediana) | Erro |
|---|---|---|---|
| 10 cm          | | | |
| 50 cm          | | | |
| 100 cm         | | | |

Probade tamén contra superficies distintas:

| Obxectivo                     | Que mide |
|---|---|
| Parede lisa                   | |
| Un xersei ou unha mochila     | |
| Unha superficie inclinada 45° | |

**Discutide:** por que falla coa tea e co ángulo? Que factor usastes para
converter o tempo en distancia, e de onde sae ese número?

**R:** _______________________________________________________________________

_______________________________________________________________________________

---

## Práctica 5: Detector de presenza (PIR)

**Piloto:** ______________  **Navegante:** ______________

O **PIR (HC-SR501)** detecta o infravermello que emiten os corpos.
Ten dous potenciómetros (sensibilidade e tempo) e un ponte de dous modos.

Avisos antes de empezar:

- Necesita entre **30 e 60 segundos de warmup** desde que se alimenta.
  Todo o que detecte nese tempo é mentira.
- A saída é un **pulso longo** (segundos): non esperedes un cambio instantáneo.
- Aliméntao a **5 V** (o pin da placa marcado `5V` ou `VIN`); a súa saída
  dixital xa é de 3,3 V.

Escribide un sketch que agarde o tempo de quecemento e despois lea o PIR,
imprimindo por Serial (ou como queirades) cando detecte movemento.

Como o pulso é longo, sen ningún control imprimiredes centos de veces a mesma detección.
Decidide como evitalo.

```cpp
const int PIR = 21;

void setup()
{
  Serial.begin(115200);
  pinMode(PIR, INPUT);
  Serial.println("Quentando...");
  // TODO: agardar o tempo de quecemento
  Serial.println("Listo");
}

void loop()
{
  // TODO: ler o PIR e imprimir só na transición de LOW a HIGH
  //       (non en cada volta mentres o pulso siga activo)
}
```

**Proba de falsos positivos.** Deixádeo dous minutos apuntando a unha zona
baleira, sen que ninguén pase.

Deteccións falsas en 2 minutos: ______

A que distancia vos detecta? ______ m   E se vos movedes moi amodo? ______

**Discutide:** un sensor que detecta movemento pero non presenza,
que problema ten nun aseo ou nun corredor?
Este problema segurísimo que xa o experimentástedes moitas veces.
Como o arranxariades por software?

**R:** _______________________________________________________________________

_______________________________________________________________________________

---

## Ampliación A — Un sensor vós sós

Escollede **un módulo do kit que non usásemos** e aplicade o método da
presentación, sen que ninguén vos dea o código.

Candidatos, co que hai que saber de cada un.
Os sensores de chama e de gas case mellor non usalos aquí na clase.

| Módulo                   | Cousas a ter en conta |
|---|---|
| Son (micrófono)          | Non mide volume: detecta se pasa dun umbral. Usa `AO` |
| Chuvia                   | Dúas pistas de cobre. **Corróese** se o deixas alimentado |
| Humidade de solo         | O mesmo problema: aliméntao só cando midas |
| Seguimento de liña (TCRT5000) | Reflexión infravermella: depende moitísimo da distancia |
| Infravermello (receptor) | Necesita a biblioteca `IRremote` e un mando a distancia |
| Láser                    | **Non apuntar aos ollos.** Só se acende e apaga |
| RTC DS1302               | Vén sen pila: perde a hora ao desconectar |
| Radio 315 MHz            | Biblioteca `RadioHead` ou `rc-switch`, e antena de 23 cm |

Código que vos funcionou:

```cpp

```

**Discutide:** canto tardastes desde que o collestes ata ter unha lectura fiable?
En que paso do método perdestes máis tempo?

**R:** _______________________________________________________________________

_______________________________________________________________________________

---

## Ampliación B — A estación meteorolóxica (opcional)

Obxectivo: xuntar as dúas sesións. Os sensores miden, e o **navegador do móbil** amosa os datos.

Deixade conectados os tres módulos:

| Módulo | Pin |
|---|---|
| Vibración (`DO`) | GPIO27 |
| LDR (`AO`)       | **GPIO34** |
| DHT11 (`DATA`)   | GPIO26 |

Escribide un sketch que:

- Se conecte á WiFi do voso móbil, coma na sesión 1.
- Lea os tres sensores cada un ao seu ritmo, sen bloquear: a vibración por
  cambio de estado con antirrebote (práctica 1), o LDR cando se pida, e o
  DHT11 non máis a miúdo de cada 2 s (práctica 3).
- Sirva, cun `WebServer` coma na sesión 1, **dous recursos distintos**:
  - un enderezo (por exemplo `/api/estado`) que devolva os catro valores en
    formato **JSON**;
  - a páxina principal (`/`), que desta vez **non se rexenera en cada
    petición**: é HTML+JS fixo que, cada segundo, pide `/api/estado` co
    `fetch` de JavaScript e actualiza os números na pantalla sen recargar.

Isto último é distinto do que fixestes na sesión 1, onde o HTML se xeraba
enteiro no servidor en cada petición: aquí a placa só manda datos, e é o
**navegador** quen debuxa.

```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char* SSID = "";        // o do voso móbil, coma na sesión 1
const char* PASS = "";

const int SENSOR = 27;
const int LDR    = 34;

DHT        dht(26, DHT11);
WebServer  server(80);

float    temp = 0, hum = 0;
int      eventos = 0;
uint32_t ultimaLectura = 0;

// TODO: páxina HTML+JS fixa cos elementos onde amosar temp/hum/luz/eventos.
// O <script> ten que pedir os datos cada segundo e actualizalos, coma:
//
//   async function actualizar() {
//     const resposta = await fetch('/api/estado');
//     const datos    = await resposta.json();
//     // TODO: usar datos.temp, datos.hum, datos.luz, datos.eventos
//     //       para actualizar os elementos da páxina
//   }
//   setInterval(actualizar, 1000);
//   actualizar();
//
static const char PAXINA[] = R"HTML(
<!DOCTYPE html>
...
)HTML";

void setup()
{
  Serial.begin(115200);
  pinMode(SENSOR, INPUT);
  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(250); Serial.print('.'); }
  Serial.println(WiFi.localIP());

  server.on("/", []{ server.send(200, "text/html", PAXINA); });

  server.on("/api/estado", []{
    // Un JSON coma {"temp":21.5,"hum":48.0,"luz":1234,"eventos":3}
    // constrúese como calquera outra cadea formateada:
    char json[128];
    snprintf(json, sizeof(json),
             "{\"temp\":%.1f,\"hum\":%.1f,\"luz\":%d,\"eventos\":%d}",
             /* TODO: os catro valores, na mesma orde ca no formato */
             );

    server.send(200, "application/json", json);
  });

  server.begin();
}

void loop()
{
  server.handleClient();

  // TODO: actualizar 'eventos', co mesmo patrón de antirrebote da práctica 1

  // TODO: cada 2000 ms, actualizar 'temp' e 'hum', co mesmo patrón da práctica 3
}

```

- [ ] A páxina ábrese desde o móbil e os números cámbianse sós.

IP da placa: ___ . ___ . ___ . ___

**Proba de latencia.** Tapade o sensor de luz e sacudide a mesa mentres mirades a
páxina no móbil.

Cantos segundos tarda en verse o cambio? ______ s   Por que ese tempo?

**R:** _______________________________________________________________________

**Discutide:** a placa xa non xera a páxina en cada petición: manda só catro
números e o HTML constrúese no navegador. Que gañades con iso? E por que o
sensor de luz **ten** que estar en GPIO34 neste sketch?

**R:** _______________________________________________________________________

_______________________________________________________________________________

_______________________________________________________________________________

---

## Práctica Libre

Con todo isto, pensade nalgunha cousa interesante que se poda facer.
Canto máis orixinal, mellor.

---

## Resumo Final

### Que quedou funcionando

- [ ] P1 — sensor dixital e antirrebote
- [ ] P2 — sensor analóxico, ADC1/ADC2 e histérese
- [ ] P3 — DHT11 cunha biblioteca
- [ ] P4 — ultrasóns
- [ ] P5 — PIR
- [ ] A — un sensor por vós mesmos (opcional)
- [ ] B — estación cos tres sensores na web (opcional)
- [ ] Práctica libre: _________________________________________________________

### O que non saíu

Anotade o que non conseguistes e ata onde chegastes. Isto **conta igual** que o
que funcionou: a metade do traballo cun sensor é descubrir por que mente.

_______________________________________________________________________________

_______________________________________________________________________________

_______________________________________________________________________________

### Entrega

Subide **este ficheiro cuberto** á tarefa de Moodle, **un por parella**.
