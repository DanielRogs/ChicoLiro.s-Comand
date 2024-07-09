/*
  Título: SISTEMA CONTROLADOR DO CARRINHO
  Autores: Daniel Rodrigues da Rocha, Leandro de Almeida
  Descrição: Código para controlar um carrinho seguidor de linha utilizando sensores infravermelhos e controle PWM.
  Data de criação: 19/06/2024
*/

/* MAPEAMENTO DE HARDWARE */
// Conexões do motor (Os pinos de Enable EN_A e EN_B devem ser pinos de PWM para podermos controlar a velocidade dos motores)

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Motor A - CORRIGIR
#define IN1 12  // Os pinos com o prefixo "IN" são pinos da Ponte H que determinam as entradas nos terminais dos motores, e consequentemente,
#define IN2 14  // a direção de rotação deles
#define EN_A 13 // Os pinos com o prefixo "EN" são pinos da Ponte H que vão determinar quanto de tensão os motores irão receber, assim determi-
                // nando a velocidade com a qual eles vão girar. Esses pinos irão receber um sinal PWM, que é um sinal digital que emula um
                // sinal analógico
     
// Motor B
#define IN3 27
#define IN4 26
#define EN_B 25

// Conexões do infravermelho
#define pinSensorMeio 2
#define pinSensorEsq 15
#define pinSensorDir 4

// Variáveis de controle de velocidade
const int velocidadeBase = 75; // Velocidade base dos motores
const int velocidadeMaxima = 90; // Velocidade máxima dos motores
const int ajusteVelocidade = 5; // Ajuste de velocidade para correções
const int limitePreto =50; // Valor limite para detectar a linha preta (ajuste conforme necessário)
const int limitePerdido = 20000;
int tempoPerdido = 0;

const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA";
const char* serverUrl = "https://chicoliro.xobengala.com.br/api/dados/receive-data";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;  // Intervalo de tempo para envio de dados (5 segundos)
unsigned long wifiCheckInterval = 10000;  // Intervalo de tempo para verificar o WiFi (10 segundos)
unsigned long lastWifiCheckTime = 0;

void setup() {
  // Configuração dos pinos dos motores
  pinMode(EN_A, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN_B, OUTPUT);

  // Configuração dos pinos dos sensores infravermelhos
  pinMode(pinSensorMeio, INPUT);
  pinMode(pinSensorEsq, INPUT);
  pinMode(pinSensorDir, INPUT);

  // Inicializa todos os pinos dos motores em nível lógico baixo
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

// Esses pinos abaixo determinam a velocidade na ponte H
  analogWrite(EN_A, 0); // Os pinos de saída analógicos do Arduino tem 8 bits de definição, logo variam entre 0 e 255 (2^8 = 256)
  analogWrite(EN_B, 0); // Mas no caso do motor, ela só pode variar de 0 até 100, pois se não, ele QUEIMA. Isso porque o que tá sendo controlado é a tensao.
                        // 255 significa 12v e o motor aguenta até 6v

  Serial.begin(9600);
  conectarWiFi();
}

void loop() {
  // Leitura dos sensores infravermelhos
  int sensorMeio = analogRead(pinSensorMeio);
  int sensorEsq = analogRead(pinSensorEsq);
  int sensorDir = analogRead(pinSensorDir);

  if (millis() - lastTime > timerDelay) {
    enviarDados();
    lastTime = millis();
  }

  if (millis() - lastWifiCheckTime > wifiCheckInterval) {
    if (WiFi.status() != WL_CONNECTED) {
      conectarWiFi();
    }
    lastWifiCheckTime = millis();
  }

 if (sensorMeio > limitePreto && sensorEsq <= limitePreto && sensorDir <= limitePreto) {
    // Linha reta - ambos os motores com velocidade base
    // moverFrente(velocidadeBase, velocidadeBase);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(EN_A, velocidadeBase);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, velocidadeBase);
    tempoPerdido = 0;
  }
  else if (sensorEsq > limitePreto && sensorDir <= limitePreto) {
    // Virar à esquerda - reduzir velocidade do motor esquerdo
    // moverFrente(0, velocidadeMaxima);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(EN_A, velocidadeMaxima - ajusteVelocidade);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, velocidadeMaxima);
    tempoPerdido = 0;
  }
  else if (sensorDir > limitePreto && sensorEsq <= limitePreto) {
    // Virar à direita - reduzir velocidade do motor direito
    // moverFrente(velocidadeMaxima, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(EN_A, velocidadeMaxima);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(EN_B, velocidadeMaxima - ajusteVelocidade);
    tempoPerdido = 0;
  }
  else if (sensorDir > limitePreto && sensorEsq <= limitePreto && sensorMeio > limitePreto) {
    // Virar à direita - reduzir velocidade do motor direito
    // moverFrente(velocidadeBase, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(EN_A, velocidadeBase);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(EN_B, velocidadeBase - ajusteVelocidade);
    tempoPerdido = 0;
  }
  else if (sensorDir <= limitePreto && sensorEsq > limitePreto && sensorMeio > limitePreto) {
    // Virar à direita - reduzir velocidade do motor esquerdo
    // moverFrente(0, velocidadeBase);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(EN_A, velocidadeBase - ajusteVelocidade);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, velocidadeBase);
    tempoPerdido = 0;
  }
  else if (sensorMeio > limitePreto && sensorEsq > limitePreto && sensorDir > limitePreto) {
    // Fim do percurso - parar o carrinho
    pararMotores();
    tempoPerdido = 0;
  }
  else {
    // Não detectado - parar o carrinho
    if (tempoPerdido >= limitePerdido){
      // Carro está perdido - parar o carrinho
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      analogWrite(EN_A, 0);

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      analogWrite(EN_B, 0);
    }
    else {
      // Carro está perdido, mas talvez consiga se encontrar - ande devagar
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      analogWrite(EN_A, velocidadeBase - ajusteVelocidade);

      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      analogWrite(EN_B, velocidadeBase - ajusteVelocidade);
      tempoPerdido++;
    }
  }

  // Serial.println(sensorMeio);
  // Serial.print(" ");
  // Serial.println(sensorEsq); // esse aqui pode dar um valor um pouco diferente deles por conta da sombra que s1 e s3 dão
  // Serial.print(" ");
  // Serial.println(sensorDir);
  // Serial.println(" ");
}
/*
A funcao abaixo faz o carrinho girar pra direita ou esquerda. Ela faz isso, girando uma roda pra frente, enquanto gira outra pra traz
A ponte H possui 6 pinos. Enable A, pinA1, pinA2, Enable B, pinB1, pinB2. Os enables controlam a velocidade
e os outros dois definem o sentido pra qual o motor vai girar. Por exemplo:

pinA1 = HIGH
pinA2 = LOW

Assim o motor vai pra FRENTE. Se inverter:

pinA1 = LOW
pinA2 = HIGH

o motor vai girar pra TRÁS. Se o dois estiverem iguais, o motor não funciona.
Nesse código, pinA1, pinA2, pinB1, pinB2, correspondem respectivamente,
aos IN1, IN2, IN3, IN4.

*/

// Função para parar os motore100s
void pararMotores() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(EN_A, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(EN_B, 0);
}

void enviarDados() {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);

      http.addHeader("Content-Type", "application/json");

      String jsonData = "{\"rpmMotorDir\": " + String(200) +
                      ", \"rpmMotorEsq\": " + String(400) +
                      ", \"tensao\": " + String(9) +
                      ", \"isMoving\": " + String(true) + "}";
      int httpResponseCode = http.POST(jsonData);

      http.end();
  }
}

void conectarWiFi() {
  WiFi.begin(ssid, password);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {  // Tenta conectar por 10 segundos
    delay(500);
  }
}