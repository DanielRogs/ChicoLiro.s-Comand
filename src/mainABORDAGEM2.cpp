#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_ipc.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>

// Definindo as credenciais WiFi
const char* ssid = "Daniel R.";
const char* wifi_password = "senhafraca123";

// Definindo o URL do servidor para onde os dados serão enviados
const char* serverUrl = "https://chicoliro.xobengala.com.br/api/dados/receive-data";
const char* serverFailUrl = "https://chicoliro.xobengala.com.br/api/trilha/failed";

// Criando uma instância do servidor
AsyncWebServer server(80);

// Variáveis para armazenar os dados dos sensores
volatile int tensao = 0;

// Variáveis de controle de velocidade
const int velocidadeBase = 75;
const int velocidadeMaxima = 90;
const int ajusteVelocidade = 5;
const int limitePreto = 50;

// Motor A
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

// Pinos Interrupção Motores
const byte MOTOR_A = 15; // Pino de interrupção do motor A
const byte MOTOR_B = 3; // Pino de interrupção do motor B

// Variáveis para contar os pulsos lidos
volatile int counter_A, counter_B = 0;
unsigned long previousMillis = 0;
const unsigned long interval = 1000; // Intervalo de 1 segundo

// Variáveis para guardar RPM
float rotation_A = 0.0;
float rotation_B = 0.0;

// Float para o número de furos presente no disco de encoder
const float diskslots = 20.00;

// Float com o diâmetro da roda em centímetros
const float wheeldiameter = 6.50;

// Controle de tempo para envio de dados
const int limiteIdentificador = 20000;
int tempoPerdido = 0;
int tempoParado = 0;

// Declaração de variáveis globais
int adc_pin35;        // Variável que guardará o valor de tensão lido no pino D35 da Esp32, em bits
float voltage;        // Variável que guardará o valor real da tensão lida no pino D35 da Esp32

// Função Exemplo para ler os sensores e calcular as RPMs
void calculateSpeed() {
  rotation_A = ((counter_A / diskslots) * 60.00)/2;
  rotation_B = ((counter_B / diskslots) * 60.00)/2;

  counter_A = 0;
  counter_B = 0;
}

void calculateTension() {
  adc_pin35 = analogRead(35);                 // Leitura do pino ADC 35 da Esp, capaz de converter sinais analógicos para digitais
  voltage = ((adc_pin35 * 5)/4096) * 0.8333; 
}

// Função de interrupção para contar pulsos do motor A
void ISR_count1() {
  counter_A++;
}

// Função de interrupção para contar pulsos do motor B
void ISR_count2() {
  counter_B++;
}

// Função para controlar motores
void lerSensores() {

  // Leitura dos sensores infravermelhos
  int sensorMeio = analogRead(pinSensorMeio);
  int sensorEsq = analogRead(pinSensorEsq);
  int sensorDir = analogRead(pinSensorDir);

  // A lógica do movimento do carrinho
  if (sensorMeio > limitePreto && sensorEsq <= limitePreto && sensorDir <= limitePreto) {
    // Linha reta - ambos os motores com velocidade base
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(EN_A, velocidadeBase);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, velocidadeBase);
    tempoParado = 0;
    tempoPerdido = 0;

  } else if (sensorEsq > limitePreto && sensorDir <= limitePreto) {
    // Virar à esquerda - reduzir velocidade do motor esquerdo
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(EN_A, velocidadeMaxima - ajusteVelocidade);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, velocidadeMaxima);
    tempoParado = 0;
    tempoPerdido = 0;

  } else if (sensorDir > limitePreto && sensorEsq <= limitePreto) {
    // Virar à direita - reduzir velocidade do motor direito
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(EN_A, velocidadeMaxima);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(EN_B, velocidadeMaxima - ajusteVelocidade);
    tempoParado = 0;
    tempoPerdido = 0;

  } else if (sensorDir > limitePreto && sensorEsq <= limitePreto && sensorMeio > limitePreto) {
    // Virar à direita - reduzir velocidade do motor direito
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(EN_A, velocidadeBase);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(EN_B, velocidadeBase - ajusteVelocidade);
    tempoParado = 0;
    tempoPerdido = 0;

  } else if (sensorDir <= limitePreto && sensorEsq > limitePreto && sensorMeio > limitePreto) {
    // Virar à esquerda - reduzir velocidade do motor esquerdo
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(EN_A, velocidadeBase - ajusteVelocidade);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, velocidadeBase);
    tempoPerdido = 0;
    tempoParado = 0;

  } else if (sensorMeio > limitePreto && sensorEsq > limitePreto && sensorDir > limitePreto) {
    // Fim do percurso - parar o carrinho
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(EN_A, 0);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, 0);
    tempoPerdido = 0;
    tempoParado++;

    if (tempoParado >= limiteIdentificador){
      //REQUISIÇÃO AQUI
    }

  } else {
    // Não detectado - analisar caso
    if (tempoPerdido >= limiteIdentificador){
      // Carro está perdido - parar o carrinho
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      analogWrite(EN_A, 0);

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      analogWrite(EN_B, 0);
      tempoParado = 0;
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
      tempoParado = 0;
    }
  }
}

// Função para enviar os dados para a API REST
void enviarDados() {
  if(WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");

      String jsonData = "{\"rpmMotorDir\": " + String(rotation_A) +
                        ", \"rpmMotorEsq\": " + String(rotation_B) +
                        ", \"tensao\": " + String(voltage) +
                        ", \"isMoving\": " + String(true) + "}";

      int httpResponseCode = http.POST(jsonData);
      http.end();
    }
}

// Função para enviar os dados para a API REST DE FALHA
void enviarFalha() {
   if(WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverFailUrl);
      http.addHeader("Content-Type", "application/json");

      String jsonData = "{}";

      int httpResponseCode = http.PATCH(jsonData);
      http.end();
    }
}

// Configuração inicial
void setup() {
  Serial.begin(9600);

  // // Conecte-se ao WiFi
  // WiFi.begin(ssid, wifi_password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  // }

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
  analogWrite(EN_A, 0);
  analogWrite(EN_B, 0);

  attachInterrupt(digitalPinToInterrupt(MOTOR_A), ISR_count1, RISING);
  attachInterrupt(digitalPinToInterrupt(MOTOR_B), ISR_count2, RISING);
}

// Loop principal
void loop() {
  // Função para fazer o carro andar na rota.
  lerSensores();

  if (millis() - previousMillis >= interval) {
    calculateSpeed();
    previousMillis = millis();
  }
}
