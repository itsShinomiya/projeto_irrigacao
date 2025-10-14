#include <DHT.h>

#define PINO_SOLO 4
#define PINO_DHT 22
#define PINO_NIVEL 2
#define PINO_RELE 23

#define TIPO_DHT DHT11

DHT dht(PINO_DHT, TIPO_DHT);

unsigned long tempoLeituraSolo = 0;
unsigned long tempoLeituraDHT = 0;
const unsigned long intervaloSolo = 20000;
const unsigned long intervaloDHT = 2000;

void setup() {
  Serial.begin(115200);
  pinMode(PINO_SOLO, INPUT);
  pinMode(PINO_NIVEL, INPUT);
  pinMode(PINO_RELE, OUTPUT);
  digitalWrite(PINO_RELE, LOW);

  dht.begin();
  Serial.println("Sistema de irrigação iniciado!");
}

void loop() {
  unsigned long agora = millis();
  
  if (agora - tempoLeituraDHT >= intervaloDHT) {
    tempoLeituraDHT = agora;
    float temp = dht.readTemperature();
    float umid = dht.readHumidity();
    if (isnan(temp) || isnan(umid)) {
      Serial.println("Falha na leitura do DHT11!");
    } else {
      Serial.print("Temperatura: ");
      Serial.print(temp);
      Serial.print(" °C | Umidade do ar: ");
      Serial.print(umid);
      Serial.println(" %");
    }

    int nivelAgua = analogRead(PINO_NIVEL);
    if (nivelAgua > 700)
      Serial.println("Nível de água: OK 💧");
    else
      Serial.println("Nível de água: CRÍTICO ⚠️");
  }

  if (agora - tempoLeituraSolo >= intervaloSolo) {
    tempoLeituraSolo = agora;

    int valorSolo = analogRead(PINO_SOLO);
    float umidadeFloat = (4095.0 - valorSolo) * 100.0 / (4095.0 - 1500.0);
    if (umidadeFloat < 0) umidadeFloat = 0;
    if (umidadeFloat > 100) umidadeFloat = 100;
    int umidadeSolo = (int)umidadeFloat;
    Serial.print("Umidade do solo: ");
    Serial.print(umidadeSolo);
    Serial.println(" %");

    int nivelAgua = analogRead(PINO_NIVEL);

    if (nivelAgua < 700) {
      Serial.println("Reservatório sem água! Bomba desativada.");
      digitalWrite(PINO_RELE, LOW);
    } 
    else if (umidadeSolo < 20) {
      Serial.println("Solo seco — ativando bomba...");
      digitalWrite(PINO_RELE, HIGH);
      delay(2000);
      digitalWrite(PINO_RELE, LOW);
    } 
    else {
      Serial.println("Solo úmido.");
    }
  }
}
