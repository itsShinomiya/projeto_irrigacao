<h1 align="center">🌱 Sistema de Irrigação Automático — ESP32</h1>

<p align="center">
  <em>Automatize sua irrigação de forma inteligente usando sensores e um ESP32!</em>
</p>

---

<h2>🔧 Componentes Utilizados</h2>

<table>
  <tr><th>Componente</th><th>Descrição</th></tr>
  <tr><td><b>ESP32</b></td><td>Microcontrolador principal</td></tr>
  <tr><td><b>Sensor DHT11</b></td><td>Mede temperatura e umidade do ar</td></tr>
  <tr><td><b>Sensor de Umidade do Solo</b></td><td>Mede o nível de umidade no solo</td></tr>
  <tr><td><b>Sensor de Nível d’Água</b></td><td>Verifica se o reservatório possui água</td></tr>
  <tr><td><b>Módulo Relé 5V</b></td><td>Controla a bomba de irrigação</td></tr>
  <tr><td><b>Bomba de Água (opcional)</b></td><td>Realiza a irrigação</td></tr>
</table>

---

<h2>⚙️ Ligações dos Pinos</h2>

<table>
  <tr><th>Pino ESP32</th><th>Componente</th><th>Função</th></tr>
  <tr><td>4</td><td>Sensor de Umidade do Solo</td><td>Entrada analógica</td></tr>
  <tr><td>22</td><td>DHT11</td><td>Leitura de temperatura e umidade</td></tr>
  <tr><td>2</td><td>Sensor de Nível d’Água</td><td>Entrada analógica</td></tr>
  <tr><td>23</td><td>Relé</td><td>Saída digital (acionamento da bomba)</td></tr>
</table>

---

<h2>🧠 Funcionamento</h2>

<ol>
  <li>O ESP32 realiza leituras periódicas:
    <ul>
      <li>A cada <b>2 segundos</b>, lê temperatura e umidade do ar (DHT11);</li>
      <li>A cada <b>20 segundos</b>, lê a umidade do solo.</li>
    </ul>
  </li>
  <li>O nível de água é verificado constantemente:
    <ul><li>Se o nível estiver <b>crítico</b>, o sistema alerta no monitor serial.</li></ul>
  </li>
  <li>Se a umidade do solo estiver <b>abaixo de 40%</b> e o nível de água <b>OK</b>, o relé é acionado.</li>
  <li>Todas as informações são exibidas no <b>Monitor Serial</b> a 115200 baud.</li>
</ol>

---

<h2>🖥️ Exemplo de Saída</h2>

<pre>
Sistema de irrigação iniciado!
Temperatura: 27.5 °C | Umidade do ar: 62 %
Nível de água: OK 💧
Umidade do solo: 34 %
Solo seco! Bomba LIGADA 🚿
</pre>

---

<h2>📈 Ajustes Importantes</h2>

<p>Se o sensor de umidade do solo tiver comportamento diferente, ajuste os valores de calibração:</p>

<pre><code>float umidadeFloat = (4095.0 - valorSolo) * 100.0 / (4095.0 - 1500.0);
</code></pre>

<ul>
  <li><b>4095</b> → leitura no solo seco</li>
  <li><b>1500</b> → leitura no solo úmido</li>
</ul>

<p>Esses valores variam conforme o tipo de sensor e o solo.</p>

---

<h2>🚀 Como Usar</h2>

<ol>
  <li>Carregue o código no ESP32 via <b>Arduino IDE</b>.</li>
  <li>Abra o <b>Monitor Serial</b> com baud rate de <b>115200</b>.</li>
  <li>Observe as leituras e o acionamento automático da bomba.</li>
</ol>

---

<h2>🧾 Licença</h2>

<p>Este projeto é de uso livre para fins educacionais e pode ser adaptado conforme necessidade.</p>

<p align="center">Feito com 💧 e ☀️ por <b>Eduardo Serotini</b></p>

