<h1 align="center">🌱 Smart Garden IoT — ESP32 com MQTT</h1>

<p align="center">
  <em>Sistema de irrigação inteligente e conectado à nuvem via MQTT (HiveMQ).<br>
  Monitore e controle sua planta de qualquer lugar do mundo!</em>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/ESP32-WiFi-blue" alt="ESP32 WiFi">
  <img src="https://img.shields.io/badge/Protocolo-MQTT-orange" alt="MQTT">
  <img src="https://img.shields.io/badge/Plataforma-HiveMQ-yellow" alt="HiveMQ">
</p>

---

<h2>🔧 Componentes Utilizados</h2>

<table>
  <tr><th>Componente</th><th>Descrição</th></tr>
  <tr><td><b>ESP32</b></td><td>Microcontrolador com Wi-Fi e suporte a SSL/TLS</td></tr>
  <tr><td><b>Sensor DHT11</b></td><td>Monitoramento de temperatura e umidade do ambiente</td></tr>
  <tr><td><b>Sensor de Umidade do Solo</b></td><td>Capacitivo ou Resistivo (entrada analógica)</td></tr>
  <tr><td><b>Sensor de Nível d’Água</b></td><td>Monitora se o reservatório está vazio</td></tr>
  <tr><td><b>Módulo Relé 5V</b></td><td>Acionamento da bomba d'água</td></tr>
  <tr><td><b>Bomba de Água</b></td><td>Atuador para irrigação</td></tr>
</table>

---

<h2>⚙️ Ligações dos Pinos (Hardware)</h2>

> **Atenção:** A pinagem foi atualizada para este código.

<table>
  <tr><th>Pino ESP32</th><th>Componente</th><th>Tipo</th></tr>
  <tr><td><b>32</b></td><td>Sensor de Umidade do Solo</td><td>Entrada Analógica (ADC)</td></tr>
  <tr><td><b>33</b></td><td>Sensor de Nível d’Água</td><td>Entrada Analógica (ADC)</td></tr>
  <tr><td><b>22</b></td><td>DHT11</td><td>Dados Digital</td></tr>
  <tr><td><b>23</b></td><td>Relé (Bomba)</td><td>Saída Digital</td></tr>
</table>

---

<h2>📡 Tópicos MQTT (Integração)</h2>

O sistema se comunica através de um Broker MQTT (neste código: HiveMQ). Utilize estes tópicos para configurar seu Dashboard (Node-RED, Home Assistant, IoT MQTT Panel, etc).

**ID da Planta:** `8` (Configurável no código em `#define PLANTA_ID`)

### 📤 Publicação (O ESP32 envia dados aqui)

| Tópico | Descrição | Exemplo de Valor |
| :--- | :--- | :--- |
| `planta/8/umidade` | Umidade atual do solo (%) | `45.5` |
| `planta/8/temperatura` | Temperatura ambiente (°C) | `24.0` |
| `planta/8/umidade_ar` | Umidade do ar (%) | `60.0` |
| `planta/8/reservatorio` | Status do tanque de água | `OK` ou `BAIXO` |
| `planta/8/bomba/status` | Confirmação se a bomba está ligada | `1` (Ligada) / `0` (Desl.) |

### 📥 Subscrição (O ESP32 recebe comandos aqui)

| Tópico | Payload | Função |
| :--- | :--- | :--- |
| `planta/8/bomba/comando` | `1` ou `0` | Liga/Desliga a bomba manualmente (Funciona apenas se o Modo Auto estiver OFF) |
| `planta/8/auto/modo` | `1` ou `0` | `1` = Ativa Modo Automático <br> `0` = Ativa Modo Manual |
| `planta/8/auto/meta` | `0` a `100` | Define a meta de umidade para o modo automático (Ex: `60`) |

---

<h2>🧠 Lógica de Funcionamento</h2>

### 1. Conexão Segura
O sistema utiliza `WiFiClientSecure` para conectar ao broker HiveMQ na porta **8883** (SSL), garantindo segurança na transmissão dos dados.

### 2. Ciclo de Leitura
A cada **2 segundos**, o sistema:
* Lê os sensores (Solo, Nível, DHT);
* Verifica a lógica de automação;
* Publica os dados atualizados nos tópicos MQTT.

### 3. Modos de Operação
* **Modo Manual (`modoAutomatico = false`):**
  * Você controla a bomba enviando `1` ou `0` para o tópico de comando.
* **Modo Automático (`modoAutomatico = true`):**
  * O sistema liga a bomba se: `Umidade Solo < Meta` **E** `Reservatório OK`.
  * O sistema desliga a bomba se: `Umidade Solo > (Meta + 20%)` **OU** `Reservatório Baixo`.

### 4. Sistema de Segurança (Fail-safe)
Se o sensor de nível detectar que o reservatório está **BAIXO** (`< 700` na leitura analógica), a bomba é desligada imediatamente (ou impedida de ligar), protegendo o hardware de queimar por trabalhar a seco.

---

<h2>📈 Calibração do Sensor de Solo</h2>

O código utiliza uma conversão linear para porcentagem. Se necessário, ajuste as constantes na linha 134:

```cpp
float umidadeSolo = (4095.0 - valorSolo) * 100.0 / (4095.0 - 1500.0);
