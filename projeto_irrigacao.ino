#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <WiFiClientSecure.h>

// === PINOS ===
#define PINO_SOLO 32
#define PINO_DHT 22
#define PINO_NIVEL 33
#define PINO_RELE 23
#define TIPO_DHT DHT11

// === WIFI & MQTT ===
const char* ssid = "Desktop_F2824662";
const char* password = "Dud112011";
const char* mqtt_server = "2594f46051674273aabeb437a13ee315.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "itsshinomiya";
const char* mqtt_pass = "123@Mqtt";

#define PLANTA_ID "8"

// === TÓPICOS ===
#define TOPIC_UMIDADE_SOLO_OUT      "planta/" PLANTA_ID "/umidade"
#define TOPIC_RESERVATORIO_OUT      "planta/" PLANTA_ID "/reservatorio"
#define TOPIC_BOMBA_STATUS_OUT      "planta/" PLANTA_ID "/bomba/status"
#define TOPIC_TEMP_OUT              "planta/" PLANTA_ID "/temperatura"
#define TOPIC_UMIDADE_AR_OUT        "planta/" PLANTA_ID "/umidade_ar"

#define TOPIC_CMD_BOMBA             "planta/" PLANTA_ID "/bomba/comando"
#define TOPIC_CFG_AUTO_MODO         "planta/" PLANTA_ID "/auto/modo"
#define TOPIC_CFG_AUTO_META         "planta/" PLANTA_ID "/auto/meta"

// === VARIÁVEIS ===
bool modoAutomatico = false;
int metaUmidade = 20;
bool bombaLigada = false;
bool reservatorioBaixo = false;

// === OBJETOS ===
WiFiClientSecure espClient;
PubSubClient client(espClient);
DHT dht(PINO_DHT, TIPO_DHT);

unsigned long lastLoop = 0;

void conectarWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

// ================= MQTT CALLBACK =================
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) msg += (char)payload[i];
  String strTopic = String(topic);

  if (strTopic == TOPIC_CMD_BOMBA) {
    if (!modoAutomatico) {
      if (msg == "1" && !reservatorioBaixo) {
        digitalWrite(PINO_RELE, HIGH);
        client.publish(TOPIC_BOMBA_STATUS_OUT, "1");
        bombaLigada = true;
      } else {
        digitalWrite(PINO_RELE, LOW);
        client.publish(TOPIC_BOMBA_STATUS_OUT, "0");
        bombaLigada = false;
      }
    }
  }

  else if (strTopic == TOPIC_CFG_AUTO_MODO) {
    if (msg == "1") {
      modoAutomatico = true;
    } else {
      modoAutomatico = false;
      digitalWrite(PINO_RELE, LOW);
      client.publish(TOPIC_BOMBA_STATUS_OUT, "0");
    }
  }

  else if (strTopic == TOPIC_CFG_AUTO_META) {
    metaUmidade = msg.toInt();
  }
}

void reconnectMQTT() {
  while (!client.connected()) {
    if (client.connect("ESP32ClientApp", mqtt_user, mqtt_pass)) {
      client.subscribe(TOPIC_CMD_BOMBA);
      client.subscribe(TOPIC_CFG_AUTO_MODO);
      client.subscribe(TOPIC_CFG_AUTO_META);
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PINO_SOLO, INPUT);
  pinMode(PINO_NIVEL, INPUT);
  pinMode(PINO_RELE, OUTPUT);
  digitalWrite(PINO_RELE, LOW);

  dht.begin();
  conectarWiFi();

  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();

  // ============================================================
  //                     LEITURAS E ENVIO MQTT
  // ============================================================

  unsigned long agora = millis();
  if (agora - lastLoop > 2000) {
    lastLoop = agora;

    // --- SENSOR DE SOLO ---
    int valorSolo = analogRead(PINO_SOLO);
    float umidadeSolo = (4095.0 - valorSolo) * 100.0 / (4095.0 - 1500.0);
    umidadeSolo = constrain(umidadeSolo, 0, 100);

    // --- SENSOR DE NÍVEL ---
    int nivelRaw = analogRead(PINO_NIVEL);
    reservatorioBaixo = (nivelRaw < 700);

    client.publish(TOPIC_UMIDADE_SOLO_OUT, String(umidadeSolo).c_str());
    client.publish(TOPIC_RESERVATORIO_OUT, reservatorioBaixo ? "BAIXO" : "OK");

    // --- DHT11 (Temperatura e Umidade do Ar) ---
    float temp = dht.readTemperature();
    float umidadeAr = dht.readHumidity();

    if (!isnan(temp) && !isnan(umidadeAr)) {
      client.publish(TOPIC_TEMP_OUT, String(temp).c_str());
      client.publish(TOPIC_UMIDADE_AR_OUT, String(umidadeAr).c_str());
      Serial.printf("Temp: %.1f  |  Umidade Ar: %.1f\n", temp, umidadeAr);
    }

    // --- MODO AUTOMÁTICO ---
    if (modoAutomatico) {
      if (umidadeSolo < metaUmidade && !reservatorioBaixo && !bombaLigada) {
        digitalWrite(PINO_RELE, HIGH);
        client.publish(TOPIC_BOMBA_STATUS_OUT, "1");
        bombaLigada = true;
      }

      else if (umidadeSolo > (metaUmidade + 20) && bombaLigada) {
        digitalWrite(PINO_RELE, LOW);
        client.publish(TOPIC_BOMBA_STATUS_OUT, "0");
        bombaLigada = false;
      }

      else if (reservatorioBaixo && bombaLigada) {
        digitalWrite(PINO_RELE, LOW);
        client.publish(TOPIC_BOMBA_STATUS_OUT, "0");
        bombaLigada = false;
      }
    }
  }
}
